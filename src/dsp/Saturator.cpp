#include "dsp/Saturator.h"

#include <algorithm>
#include <cmath>

namespace rsat::dsp
{
    void Saturator::prepare(double sampleRate, int maximumBlockSize, int numberOfChannels)
    {
        juce::ignoreUnused(maximumBlockSize);

        currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
        currentNumChannels = std::max(0, numberOfChannels);
        channelStates.assign(static_cast<size_t>(currentNumChannels), {});

        const auto smoothingTimeSeconds = 0.02;
        inputGain.reset(currentSampleRate, smoothingTimeSeconds);
        driveAmount.reset(currentSampleRate, smoothingTimeSeconds);
        toneAmount.reset(currentSampleRate, smoothingTimeSeconds);
        outputGain.reset(currentSampleRate, smoothingTimeSeconds);
        mixAmount.reset(currentSampleRate, smoothingTimeSeconds);

        inputGain.setCurrentAndTargetValue(1.0f);
        driveAmount.setCurrentAndTargetValue(0.2f);
        toneAmount.setCurrentAndTargetValue(0.5f);
        outputGain.setCurrentAndTargetValue(1.0f);
        mixAmount.setCurrentAndTargetValue(1.0f);
    }

    void Saturator::reset()
    {
        for (auto& state : channelStates)
            state.toneLowpassState = 0.0f;

        inputGain.reset(currentSampleRate, 0.02);
        driveAmount.reset(currentSampleRate, 0.02);
        toneAmount.reset(currentSampleRate, 0.02);
        outputGain.reset(currentSampleRate, 0.02);
        mixAmount.reset(currentSampleRate, 0.02);
    }

    void Saturator::setParameters(float inputGainDb, float drive, float tone, float outputGainDb, float mix)
    {
        inputGain.setTargetValue(juce::Decibels::decibelsToGain(inputGainDb));
        driveAmount.setTargetValue(juce::jlimit(0.0f, 1.0f, drive));
        toneAmount.setTargetValue(juce::jlimit(0.0f, 1.0f, tone));
        outputGain.setTargetValue(juce::Decibels::decibelsToGain(outputGainDb));
        mixAmount.setTargetValue(juce::jlimit(0.0f, 1.0f, mix));
    }

    void Saturator::process(juce::AudioBuffer<float>& buffer)
    {
        const auto numChannels = std::min(buffer.getNumChannels(), currentNumChannels);
        const auto numSamples = buffer.getNumSamples();

        for (int sampleIndex = 0; sampleIndex < numSamples; ++sampleIndex)
        {
            const auto inGain = inputGain.getNextValue();
            const auto drive = driveAmount.getNextValue();
            const auto outGain = outputGain.getNextValue();
            const auto wetMix = mixAmount.getNextValue();

            juce::ignoreUnused(toneAmount.getNextValue());

            const auto internalDrive = 1.0f + drive * 8.0f;
            const auto driveCompensation = 1.0f / std::sqrt(internalDrive);

            for (int channel = 0; channel < numChannels; ++channel)
            {
                auto* samples = buffer.getWritePointer(channel);
                auto& state = channelStates[static_cast<size_t>(channel)];
                const auto dry = samples[sampleIndex];

                auto wet = dry * inGain;
                wet = softClip(wet * internalDrive) * driveCompensation;
                wet = processTone(wet, state);
                wet *= outGain;

                samples[sampleIndex] = dry + (wet - dry) * wetMix;
            }
        }

        for (int channel = numChannels; channel < buffer.getNumChannels(); ++channel)
            buffer.clear(channel, 0, numSamples);
    }

    float Saturator::softClip(float sample) noexcept
    {
        return sample / (1.0f + std::abs(sample));
    }

    float Saturator::processTone(float sample, ChannelState& state) const noexcept
    {
        const auto tone = toneAmount.getCurrentValue();
        const auto cutoff = juce::jmap(tone, 0.0f, 1.0f, 1200.0f, 18000.0f);
        const auto coefficient = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * cutoff
                                                / static_cast<float>(currentSampleRate));

        state.toneLowpassState += coefficient * (sample - state.toneLowpassState);

        const auto brightness = juce::jmap(tone, 0.0f, 1.0f, 0.0f, 0.35f);
        return state.toneLowpassState + (sample - state.toneLowpassState) * brightness;
    }
}
