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
    const Colour tempColour(0xfffe00ee);
    mTemperatureSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, tempColour);
    mTemperatureSlider.setColour(Slider::ColourIds::thumbColourId, Colours::white);
    mTemperatureSliderAttachment.reset(new SliderAttachment(mAVTS,
                                                            rave_parameters::param_name_temperature,
                                                            mTemperatureSlider));
    
    addAndMakeVisible(&mWetGainSlider);
    mWetGainSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mWetGainSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    const Colour wetColour(0xff00fef2);
    mWetGainSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, wetColour);
    mWetGainSlider.setColour(Slider::ColourIds::thumbColourId, Colours::white);
    mWetSliderAttachment.reset(new SliderAttachment(mAVTS,
                                                     rave_parameters::param_name_wetgain,
                                                     mWetGainSlider));
    
    addAndMakeVisible(&mDryGainSlider);
    mDryGainSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mDryGainSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    mDryGainSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::lime);
    mDryGainSlider.setColour(Slider::ColourIds::thumbColourId, Colours::white);
    mDrySliderAttachment.reset(new SliderAttachment(mAVTS,
                                                     rave_parameters::param_name_drygain,
                                                     mDryGainSlider));
    
    addAndMakeVisible(&mTogglePrior);
    mTogglePrior.setButtonText("Use Prior");
    mTogglePriorAttachment.reset(new ButtonAttachment(mAVTS,
                                                      rave_parameters::param_name_toggleprior,
                                                      mTogglePrior));
    
    addAndMakeVisible(&mImportButton);
    mImportButton.setButtonText("IMPORT");
    mImportButton.setClickingTogglesState(true);
    mImportButtonAttachment.reset(new ButtonAttachment(mAVTS,
                                                       rave_parameters::param_name_importbutton,
                                                       mImportButton));
    
    addAndMakeVisible(&mWetGainLabel);
    mWetGainLabel.setText("WET", NotificationType::dontSendNotification);
    mWetGainLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(&mDryGainLabel);
    mDryGainLabel.setText("DRY", NotificationType::dontSendNotification);
    mDryGainLabel.setJustificationType(Justification::centred);
    
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
}

void RAVEAuditionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    mLogo->setBoundsRelative(0.f, 0.f, 1.f, 0.7f);
    
    Rectangle<float> mRelBounds { 0.f, 0.f, 1.f, 1.f };
    mRelBounds.removeFromBottom(0.075f);
    auto bottomBounds = mRelBounds.removeFromBottom(0.3f);
    auto wetLabelBounds = mRelBounds.removeFromBottom(0.1f);
    auto temperatureLabelBounds = wetLabelBounds;
    auto dryLabelBounds = temperatureLabelBounds;
    
    wetLabelBounds.removeFromLeft(0.6666666f);
    mWetGainLabel.setBoundsRelative(wetLabelBounds);
    
    temperatureLabelBounds.removeFromRight(0.3333333f);
    temperatureLabelBounds.removeFromLeft(0.3333333f);
    mTemperatureLabel.setBoundsRelative(temperatureLabelBounds);
    
    dryLabelBounds.removeFromRight(0.6666666f);
    mDryGainLabel.setBoundsRelative(dryLabelBounds);
    
    mRelBounds.removeFromTop(0.05f);
    auto topBounds = mRelBounds.removeFromTop(0.05f);
    auto toggleBounds = topBounds;
    topBounds.removeFromLeft(0.85f);
    topBounds.removeFromRight(0.05f);
    mImportButton.setBoundsRelative(topBounds);
    
    toggleBounds.removeFromLeft(0.05f);
    toggleBounds.removeFromRight(0.8f);
    mTogglePrior.setBoundsRelative(toggleBounds);
    
    mWetGainSlider.setBoundsRelative(bottomBounds.removeFromRight(0.3333333f));
    mTemperatureSlider.setBoundsRelative(bottomBounds.removeFromRight(0.3333333f));
    mDryGainSlider.setBoundsRelative(bottomBounds.removeFromRight(0.3333333f));
    
}
