#pragma once

#include <juce_dsp/juce_dsp.h>

#include <vector>

namespace rsat::dsp
{
    class Saturator
    {
    public:
        void prepare(double sampleRate, int maximumBlockSize, int numberOfChannels);
        void reset();

        void setParameters(float inputGainDb, float drive, float tone, float outputGainDb, float mix);
        void process(juce::AudioBuffer<float>& buffer);

    private:
        struct ChannelState
        {
            float toneLowpassState = 0.0f;
        };

        static float softClip(float sample) noexcept;
        float processTone(float sample, ChannelState& state) const noexcept;

        double currentSampleRate = 44100.0;
        int currentNumChannels = 0;

        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> inputGain;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> driveAmount;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> toneAmount;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> outputGain;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mixAmount;

        std::vector<ChannelState> channelStates;
    };
}
