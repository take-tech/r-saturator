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
        {
            state.warmthLowState = 0.0f;
            state.dampingLowState = 0.0f;
            state.airLowState = 0.0f;
        }

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
            const auto tone = toneAmount.getNextValue();
            const auto outGain = outputGain.getNextValue();
            const auto wetMix = mixAmount.getNextValue();

            const auto internalDrive = 1.0f + drive * 8.0f;
            const auto driveCompensation = 1.0f / std::sqrt(internalDrive);

            for (int channel = 0; channel < numChannels; ++channel)
            {
                auto* samples = buffer.getWritePointer(channel);
                auto& state = channelStates[static_cast<size_t>(channel)];
                const auto dry = samples[sampleIndex];

                auto wet = dry * inGain;
                wet = softClip(wet * internalDrive) * driveCompensation;
                wet = processTone(wet, tone, state);
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

    float Saturator::getOnePoleCoefficient(float cutoffHz, double sampleRate) noexcept
    {
        const auto safeCutoff = juce::jlimit(20.0f, 20000.0f, cutoffHz);
        return 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * safeCutoff
                                / static_cast<float>(sampleRate));
    }

    float Saturator::processTone(float sample, float tone, ChannelState& state) const noexcept
    {
        const auto clampedTone = juce::jlimit(0.0f, 1.0f, tone);
        const auto warmth = juce::jlimit(0.0f, 1.0f, (0.5f - clampedTone) * 2.0f);
        const auto air = juce::jlimit(0.0f, 1.0f, (clampedTone - 0.5f) * 2.0f);

        const auto warmthCoefficient = getOnePoleCoefficient(420.0f + warmth * 260.0f, currentSampleRate);
        state.warmthLowState += warmthCoefficient * (sample - state.warmthLowState);

        const auto dampingCutoff = juce::jmap(clampedTone, 0.0f, 0.5f, 4200.0f, 18000.0f);
        const auto dampingCoefficient = getOnePoleCoefficient(dampingCutoff, currentSampleRate);
        state.dampingLowState += dampingCoefficient * (sample - state.dampingLowState);

        const auto airCoefficient = getOnePoleCoefficient(3600.0f, currentSampleRate);
        state.airLowState += airCoefficient * (sample - state.airLowState);

        const auto lowMid = state.warmthLowState;
        const auto damped = state.dampingLowState + (sample - state.dampingLowState) * 0.42f;
        const auto airBand = sample - state.airLowState;

        auto result = sample;

        if (warmth > 0.0f)
        {
            const auto body = lowMid * (0.34f * warmth);
            const auto roundedLow = (softClip(lowMid * (1.0f + warmth * 3.2f)) - lowMid) * (0.18f * warmth);
            result = result * (1.0f - warmth * 0.28f) + damped * (warmth * 0.28f);
            result += body + roundedLow;
        }

        if (air > 0.0f)
        {
            const auto sparkle = softClip(airBand * (1.0f + air * 5.0f)) * (0.20f * air);
            const auto lift = airBand * (0.32f * air);
            result += lift + sparkle;
        }

        return juce::jlimit(-1.8f, 1.8f, result);
    }
}
