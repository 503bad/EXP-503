#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (300, 500);
    
    titleLabel.setText("EXP-503", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(24.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);
    
    sparkSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    sparkSlider.setRange(0.0, 100.0, 0.1);
    sparkSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    sparkSlider.setValue(audioProcessor.sparkValue);
    sparkSlider.onValueChange = [this]() {
        audioProcessor.sparkValue = static_cast<float>(sparkSlider.getValue());
    };
    sparkSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::red);
    sparkSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    addAndMakeVisible(sparkSlider);
    
    sparkLabel.setText("SPARK", juce::dontSendNotification);
    sparkLabel.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    sparkLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    sparkLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(sparkLabel);
    
    explodeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    explodeSlider.setRange(0.0, 100.0, 0.1);
    explodeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    explodeSlider.setValue(audioProcessor.explodeValue);
    explodeSlider.onValueChange = [this]() {
        audioProcessor.explodeValue = static_cast<float>(explodeSlider.getValue());
    };
    explodeSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::red);
    explodeSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    addAndMakeVisible(explodeSlider);
    
    explodeLabel.setText("EXPLODE", juce::dontSendNotification);
    explodeLabel.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    explodeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    explodeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(explodeLabel);
    
    sparkValueLabel.setText("0", juce::dontSendNotification);
    sparkValueLabel.setFont(juce::FontOptions(12.0f));
    sparkValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    sparkValueLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(sparkValueLabel);
    
    explodeValueLabel.setText("0", juce::dontSendNotification);
    explodeValueLabel.setFont(juce::FontOptions(12.0f));
    explodeValueLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    explodeValueLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(explodeValueLabel);
    
    
    logoImage = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);
    illustrationImage = juce::ImageCache::getFromMemory(BinaryData::igarashi_exp_png, BinaryData::igarashi_exp_pngSize);
    
    startTimer(50);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::Colour backgroundColor = getCurrentBackgroundColour();
    g.fillAll(backgroundColor);
    
    if (logoImage.isValid())
    {
        g.drawImageWithin(logoImage, 20, 10, 120, 60, 
                         juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);
    }
    else
    {
        g.setColour(juce::Colour(0xff666666));
        g.drawRect(20, 20, 120, 60, 2);
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(10.0f));
        g.drawText("LOGO", 20, 20, 120, 60, juce::Justification::centred);
    }
    
    if (illustrationImage.isValid())
    {
        g.drawImageWithin(illustrationImage, 75, 100, 150, 280, 
                         juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);
    }
    else
    {
        g.setColour(juce::Colour(0xff666666));
        g.drawRect(75, 100, 150, 280, 2);
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(14.0f));
        g.drawText("EXPLOSION\\nILLUSTRATION", 75, 100, 150, 280, juce::Justification::centred);
    }
}

void AudioPluginAudioProcessorEditor::resized()
{
    titleLabel.setBounds(150, 20, 130, 40);
    
    sparkSlider.setBounds(20, 350, 100, 100);
    sparkLabel.setBounds(20, 450, 100, 20);
    sparkValueLabel.setBounds(20, 470, 100, 20);
    
    explodeSlider.setBounds(180, 350, 100, 100);
    explodeLabel.setBounds(180, 450, 100, 20);
    explodeValueLabel.setBounds(180, 470, 100, 20);
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    sparkValueLabel.setText(juce::String(static_cast<int>(audioProcessor.sparkValue)), juce::dontSendNotification);
    
    float explodeDb = (audioProcessor.explodeValue / 100.0f) * 80.0f - 60.0f;
    juce::String explodeText = (explodeDb >= 0.0f ? "+" : "") + juce::String(explodeDb, 1) + "dB";
    explodeValueLabel.setText(explodeText, juce::dontSendNotification);
    
    sparkSlider.setValue(audioProcessor.sparkValue, juce::dontSendNotification);
    explodeSlider.setValue(audioProcessor.explodeValue, juce::dontSendNotification);
    
    float newExplodeValue = audioProcessor.explodeValue;
    if (std::abs(newExplodeValue - currentExplodeValue) > 0.1f)
    {
        currentExplodeValue = newExplodeValue;
        repaint();
    }
}

juce::Colour AudioPluginAudioProcessorEditor::getCurrentBackgroundColour()
{
    float normalizedValue = currentExplodeValue / 100.0f;
    
    juce::Colour startColor(0xff2c2c2c);
    juce::Colour endColor(0xffc20000);
    
    return startColor.interpolatedWith(endColor, normalizedValue);
}