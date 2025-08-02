#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

const juce::String AudioPluginAudioProcessor::getName() const
{
    return "EXP-503";
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
    return false;
}

bool AudioPluginAudioProcessor::producesMidi() const
{
    return false;
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
    return false;
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 1;
    
    leftDelay.prepare(spec);
    leftDelay.setMaximumDelayInSamples(static_cast<int>(sampleRate * 0.1)); // 100ms max
    leftDelay.setDelay(static_cast<float>(sampleRate * 0.001)); // 1ms
    
    rightDelay.prepare(spec);
    rightDelay.setMaximumDelayInSamples(static_cast<int>(sampleRate * 0.1));
    rightDelay.setDelay(static_cast<float>(sampleRate * 0.02)); // 20ms
    
    lowpassFilter.prepare(spec);
    auto coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 500.0f);
    lowpassFilter.coefficients = coefficients;
    
    pitchBuffer1.setSize(1, static_cast<int>(sampleRate * 0.1)); // 100ms buffer
    pitchBuffer2.setSize(1, static_cast<int>(sampleRate * 0.1));
    pitchBuffer1.clear();
    pitchBuffer2.clear();
    pitchBufferIndex1 = 0;
    pitchBufferIndex2 = 0;
}

void AudioPluginAudioProcessor::releaseResources()
{
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    juce::ignoreUnused (midiMessages);

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (totalNumInputChannels >= 2)
    {
        auto* leftChannel = buffer.getWritePointer(0);
        auto* rightChannel = buffer.getWritePointer(1);
        const int numSamples = buffer.getNumSamples();
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float inputLeft = leftChannel[sample];
            float inputRight = rightChannel[sample];
            float inputCenter = (inputLeft + inputRight) * 0.5f;
            
            leftDelay.pushSample(0, inputLeft);
            float delayedLeft = leftDelay.popSample(0);
            
            rightDelay.pushSample(0, inputRight);
            float delayedRight = rightDelay.popSample(0);
            
            float filtered = lowpassFilter.processSample(inputCenter);
            
            pitchBuffer1.getWritePointer(0)[pitchBufferIndex1] = filtered;
            int readIndex1 = (pitchBufferIndex1 - static_cast<int>(currentSampleRate * 0.05)) % pitchBuffer1.getNumSamples();
            if (readIndex1 < 0) readIndex1 = pitchBuffer1.getNumSamples() + readIndex1;
            float pitch1 = pitchBuffer1.getReadPointer(0)[readIndex1];
            pitchBufferIndex1 = (pitchBufferIndex1 + 1) % pitchBuffer1.getNumSamples();
            
            pitchBuffer2.getWritePointer(0)[pitchBufferIndex2] = filtered;
            int readIndex2 = (pitchBufferIndex2 - static_cast<int>(currentSampleRate * 0.075)) % pitchBuffer2.getNumSamples();
            if (readIndex2 < 0) readIndex2 = pitchBuffer2.getNumSamples() + readIndex2;
            float pitch2 = pitchBuffer2.getReadPointer(0)[readIndex2];
            pitchBufferIndex2 = (pitchBufferIndex2 + 1) % pitchBuffer2.getNumSamples();
            
            float sparkMix = sparkValue / 100.0f;
            float explodeDb = (explodeValue / 100.0f) * 80.0f - 60.0f;
            float explodeGain = powf(10.0f, explodeDb / 20.0f);
            
            float system1 = delayedLeft * sparkMix;
            float system2 = delayedRight * sparkMix;
            float wetSignal = (pitch1 + pitch2) * 0.5f;
            float system3 = (filtered * 0.5f + wetSignal * 0.5f) * explodeGain;
            
            leftChannel[sample] = inputLeft * (1.0f - sparkMix) + system1 + system3 * 0.5f;
            rightChannel[sample] = inputRight * (1.0f - sparkMix) + system2 + system3 * 0.5f;
        }
    }
}

bool AudioPluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(sparkValue);
    stream.writeFloat(explodeValue);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    sparkValue = stream.readFloat();
    explodeValue = stream.readFloat();
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}