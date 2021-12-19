/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RAVEAuditionAudioProcessor::RAVEAuditionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), mAVTS(*this, nullptr, Identifier ("RAVEValueTree"), createParameterLayout())
#endif
{
    mRave.reset(new RAVE("/Users/andrewfyfe/Developer/projects/qosmo/rave-audition/Resources/speech/speech_realtime.ts"));
    
//    unsigned int inputSize = mRave->decode_explosion;
    
    FloatFifo* inputfifo_p=&mInputFifo;
    FloatFifo* outputfifo_p=&mOutputFifo;

    FifoBuffer_init(inputfifo_p,DEFAULT_FIFO_LENGTH,float, mInputFifoBuffer);
    
    FifoBuffer_init(outputfifo_p,DEFAULT_FIFO_LENGTH,float, mOutputFifoBuffer);
    
    mTemperatureParameterValue = mAVTS.getRawParameterValue(rave_parameters::param_name_temperature);
    mOutputGainParameterValue = mAVTS.getRawParameterValue(rave_parameters::param_name_outputgain);
    mTogglePriorParameterValue = mAVTS.getRawParameterValue(rave_parameters::param_name_toggleprior);

}

RAVEAuditionAudioProcessor::~RAVEAuditionAudioProcessor()
{
}

//==============================================================================
const juce::String RAVEAuditionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RAVEAuditionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RAVEAuditionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RAVEAuditionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RAVEAuditionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RAVEAuditionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RAVEAuditionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RAVEAuditionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RAVEAuditionAudioProcessor::getProgramName (int index)
{
    return {};
}

void RAVEAuditionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RAVEAuditionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    FloatFifo* inputfifo_p=&mInputFifo;
    FloatFifo* outputfifo_p=&mOutputFifo;
    FifoBuffer_flush(inputfifo_p);
    FifoBuffer_flush(outputfifo_p);
    mSmoothedOutputGain.reset(sampleRate, 0.1);
}

void RAVEAuditionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RAVEAuditionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void RAVEAuditionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    const int nSamples = buffer.getNumSamples();
    const int nChannels = buffer.getNumChannels();
    
    FloatFifo* inputfifo_p=&mInputFifo;
    FloatFifo* outputfifo_p=&mOutputFifo;
    
    float* channelL = buffer.getWritePointer(0);
    float* channelR = buffer.getWritePointer(1);
    
    const float temperatureVal = mTemperatureParameterValue->load();
    const bool usePrior = static_cast<bool>(mTogglePriorParameterValue->load());
    mSmoothedOutputGain.setTargetValue(mOutputGainParameterValue->load());

//    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
//        buffer.clear (i, 0, buffer.getNumSamples());

    FloatVectorOperations::add(channelL, channelR, nSamples);
    FloatVectorOperations::multiply(channelL, 0.5f, nSamples);
    FloatVectorOperations::copy(channelR, channelL, nSamples);
    
    for (int i = 0; i < nSamples; ++i) {
        FifoBuffer_write(inputfifo_p, channelL[i]);
    }

    while (FifoBuffer_is_full(inputfifo_p)) {
        
        for (int i = 0; i < mFifoSize; i++) {
            FifoBuffer_read(inputfifo_p, mTempBuffer[i]);
        }
        
        // size of the process buffer data
        int64_t sizes = {mFifoSize};
        at::Tensor output;
        
        if (usePrior) {
            output = mRave->sample_from_prior(1, temperatureVal);
        }
        else
        {
            at::Tensor frame = torch::from_blob(mTempBuffer, sizes);

            frame = torch::reshape(frame, {1,1,mFifoSize});

            std::vector<torch::jit::IValue> inputs_rave { frame };
            output = mRave->model.forward(inputs_rave).toTensor();
        }

        for (int64_t i = 0; i < sizes; ++i) {
            auto outputData = output.index({0,torch::indexing::Slice()});      // index the proper output channel
            auto outputDataPtr = outputData.data_ptr<float>();
            
            FifoBuffer_write(outputfifo_p, outputDataPtr[i]);
        }
        
    }
    
    if (FifoBuffer_count(outputfifo_p) >= nSamples)
    {
        for (int i = 0; i < nSamples; ++i) {
            float outSample;
            FifoBuffer_read(outputfifo_p, outSample);
            channelL[i] = channelR[i] = outSample * mSmoothedOutputGain.getNextValue();
        }
        
    }
}

//==============================================================================
bool RAVEAuditionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RAVEAuditionAudioProcessor::createEditor()
{
    return new RAVEAuditionAudioProcessorEditor (*this, mAVTS);
}

//==============================================================================
void RAVEAuditionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    auto state = mAVTS.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void RAVEAuditionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (mAVTS.state.getType()))
            mAVTS.replaceState (ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RAVEAuditionAudioProcessor();
}

auto RAVEAuditionAudioProcessor::createParameterLayout() -> AudioProcessorValueTreeState::ParameterLayout
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    params.push_back (std::make_unique<AudioParameterFloat> (rave_parameters::param_name_outputgain, rave_parameters::param_name_outputgain, 0.0f, 1.0f, 1.f));
    
    NormalisableRange<float> normrange{-15.f, 15.f, 0.01f};
    params.push_back (std::make_unique<AudioParameterFloat> (rave_parameters::param_name_temperature, rave_parameters::param_name_temperature, normrange, 0.1f));
    
    params.push_back (std::make_unique<AudioParameterBool> (rave_parameters::param_name_toggleprior, rave_parameters::param_name_toggleprior, false));
    
    return { params.begin(), params.end() };
    
}
