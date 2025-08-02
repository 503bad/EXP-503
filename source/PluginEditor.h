#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    AudioPluginAudioProcessor& audioProcessor;
    
    juce::Slider sparkSlider;
    juce::Slider explodeSlider;
    juce::Label sparkLabel;
    juce::Label explodeLabel;
    juce::Label sparkValueLabel;
    juce::Label explodeValueLabel;
    juce::Label titleLabel;
    
    juce::Image logoImage;
    juce::Image illustrationImage;
    
    float currentExplodeValue = 0.0f;
    juce::Colour getCurrentBackgroundColour();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};