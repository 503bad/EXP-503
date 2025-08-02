#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class AudioPluginAudioProcessor : public juce::AudioProcessor
{
public:
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // パラメータアクセス
    float sparkValue = 0.0f;
    float explodeValue = 0.0f;      // 3系統目のデシベル制御

private:
    // ディレイライン
    juce::dsp::DelayLine<float> leftDelay;   // 1系統目: 1msディレイ
    juce::dsp::DelayLine<float> rightDelay;  // 2系統目: 20msディレイ
    
    // 3系統目用フィルター
    juce::dsp::IIR::Filter<float> lowpassFilter;
    
    // ピッチシフト用の簡易バッファ
    juce::AudioBuffer<float> pitchBuffer1;   // 1オクターブ下げ用
    juce::AudioBuffer<float> pitchBuffer2;   // 2オクターブ下げ用
    int pitchBufferIndex1 = 0;
    int pitchBufferIndex2 = 0;
    
    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
};