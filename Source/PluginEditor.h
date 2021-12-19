/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

using namespace juce;

static const juce::Colour APPLE_BLACK { Colour::fromRGBA(28, 28, 30, 255) };

//==============================================================================
/**
*/
class RAVEAuditionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
    
typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    
public:
    RAVEAuditionAudioProcessorEditor (RAVEAuditionAudioProcessor&, AudioProcessorValueTreeState&);
    ~RAVEAuditionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    RAVEAuditionAudioProcessor& audioProcessor;
    AudioProcessorValueTreeState& mAVTS;
    
    std::unique_ptr<ImageComponent> mLogo;
    
    Slider mWetGainSlider;
    std::unique_ptr<SliderAttachment> mWetSliderAttachment;
    
    Slider mDryGainSlider;
    std::unique_ptr<SliderAttachment> mDrySliderAttachment;
    
    Slider mTemperatureSlider;
    std::unique_ptr<SliderAttachment> mTemperatureSliderAttachment;
    
    ToggleButton mTogglePrior;
    std::unique_ptr<ButtonAttachment> mTogglePriorAttachment;
    
    TextButton mImportButton;
    std::unique_ptr<ButtonAttachment> mImportButtonAttachment;
    
    Label mWetGainLabel;
    Label mDryGainLabel;
    Label mTemperatureLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RAVEAuditionAudioProcessorEditor)
};
