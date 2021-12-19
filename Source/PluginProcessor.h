/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <torch/torch.h>
#include <torch/script.h>
//#include <torch/csrc/jit/runtime/graph_executor.h>
//#include <torch/csrc/jit/serialization/export.h>
#include "FifoBuffer.h"
#include "Rave.h"

#define DEFAULT_FIFO_LENGTH 2048

namespace rave_parameters
{
    const String param_name_temperature {"temperature"};
    const String param_name_outputgain {"output_gain"};
    const String param_name_toggleprior {"toggle_prior"};
}
//==============================================================================
/**
*/
class RAVEAuditionAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    RAVEAuditionAudioProcessor();
    ~RAVEAuditionAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    auto createParameterLayout() -> AudioProcessorValueTreeState::ParameterLayout;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    
    juce::AudioProcessorValueTreeState mAVTS;
    std::unique_ptr<RAVE> mRave;
//    torch::Tensor mInputTensor;

    
    /*
    *Allocate some memory to use as the FifoBuffer storage
    *for each of the FifoBuffer types to be created
    */
    const int mFifoSize { DEFAULT_FIFO_LENGTH };
    float mInputFifoBuffer[DEFAULT_FIFO_LENGTH];
    float mOutputFifoBuffer[DEFAULT_FIFO_LENGTH];
    float mTempBuffer[DEFAULT_FIFO_LENGTH];
    
    float* mInFifoBuffer { nullptr };
    float* mOutFifoBuffer { nullptr };

    /*
    * Call the macro which creates a
    * type-specific FifoBuffer definition
    */
    FifoBuffer_typedef(float,FloatFifo);

    FloatFifo mInputFifo;
    FloatFifo mOutputFifo;
    
    std::atomic<float>* mTemperatureParameterValue;
    std::atomic<float>* mOutputGainParameterValue;
    std::atomic<float>* mTogglePriorParameterValue;
    
    LinearSmoothedValue<float> mSmoothedOutputGain;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RAVEAuditionAudioProcessor)
};
