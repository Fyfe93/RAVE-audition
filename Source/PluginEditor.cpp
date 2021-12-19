/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RAVEAuditionAudioProcessorEditor::RAVEAuditionAudioProcessorEditor (RAVEAuditionAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), mAVTS(vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    
    mLogo.reset(new ImageComponent());
    mLogo->setImage(ImageCache::getFromMemory(BinaryData::rave_logo_transparent_white_png, BinaryData::rave_logo_transparent_white_pngSize));
    addAndMakeVisible(mLogo.get());
    
    addAndMakeVisible(&mTemperatureSlider);
    mTemperatureSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mTemperatureSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    mTemperatureSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::deeppink);
    mTemperatureSlider.setColour(Slider::ColourIds::thumbColourId, Colours::white);
    mTemperatureSliderAttachment.reset(new SliderAttachment(mAVTS,
                                                            rave_parameters::param_name_temperature,
                                                            mTemperatureSlider));
    
    addAndMakeVisible(&mOutputGainSlider);
    mOutputGainSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mOutputGainSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    mOutputGainSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::lime);
    mOutputGainSlider.setColour(Slider::ColourIds::thumbColourId, Colours::white);
    mGainSliderAttachment.reset(new SliderAttachment(mAVTS,
                                                     rave_parameters::param_name_outputgain,
                                                     mOutputGainSlider));
    
    addAndMakeVisible(&mTogglePrior);
    mTogglePrior.setButtonText("Use Prior");
    mTogglePriorAttachment.reset(new ButtonAttachment(mAVTS,
                                                      rave_parameters::param_name_toggleprior,
                                                      mTogglePrior));
    
    addAndMakeVisible(&mImportButton);
    mImportButton.setButtonText("IMPORT");
    
    addAndMakeVisible(&mOutputGainLabel);
    mOutputGainLabel.setText("GAIN", NotificationType::dontSendNotification);
    mOutputGainLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(&mTemperatureLabel);
    mTemperatureLabel.setText("HEAT", NotificationType::dontSendNotification);
    mTemperatureLabel.setJustificationType(Justification::centred);

    
    setResizable(true, true);
    getConstrainer()->setMinimumSize(400, 200);
    
    setSize (800, 500);
}

RAVEAuditionAudioProcessorEditor::~RAVEAuditionAudioProcessorEditor()
{
}

//==============================================================================
void RAVEAuditionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (APPLE_BLACK);

//    g.setColour (juce::Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void RAVEAuditionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
//    auto bounds = getLocalBounds();
//    const int h = bounds.getHeight();
//    const int w = bounds.getWidth();
    
    mLogo->setBoundsRelative(0.f, 0.f, 1.f, 0.7f);
    
    Rectangle<float> mRelBounds { 0.f, 0.f, 1.f, 1.f };
    mRelBounds.removeFromBottom(0.075f);
    auto bottomBounds = mRelBounds.removeFromBottom(0.3f);
    auto gainLabelBounds = mRelBounds.removeFromBottom(0.1f);
    auto temperatureLabelBounds = gainLabelBounds;
    
    gainLabelBounds.removeFromLeft(0.6666666f);
    mOutputGainLabel.setBoundsRelative(gainLabelBounds);
    
    temperatureLabelBounds.removeFromRight(0.6666666f);
    mTemperatureLabel.setBoundsRelative(temperatureLabelBounds);
    
    mRelBounds.removeFromTop(0.05f);
    auto topBounds = mRelBounds.removeFromTop(0.05f);
    auto toggleBounds = topBounds;
    topBounds.removeFromLeft(0.85f);
    topBounds.removeFromRight(0.05f);
    mImportButton.setBoundsRelative(topBounds);
    
    toggleBounds.removeFromLeft(0.05f);
    toggleBounds.removeFromRight(0.8f);
    mTogglePrior.setBoundsRelative(toggleBounds);
    
    mTemperatureSlider.setBoundsRelative(bottomBounds.removeFromLeft(0.3333333f));
    mOutputGainSlider.setBoundsRelative(bottomBounds.removeFromRight(0.3333333f));
    
}
