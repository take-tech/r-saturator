#include "Parameters.h"

#include <functional>
#include <memory>
#include <vector>

namespace
{
    std::unique_ptr<juce::AudioParameterFloat> makeFloatParameter(
        const juce::ParameterID& id,
        const juce::String& name,
        juce::NormalisableRange<float> range,
        float defaultValue,
        const juce::String& label = {},
        std::function<juce::String(float, int)> valueToText = {},
        std::function<float(const juce::String&)> textToValue = {})
    {
        auto attributes = juce::AudioParameterFloatAttributes()
            .withLabel(label)
            .withStringFromValueFunction(std::move(valueToText))
            .withValueFromStringFunction(std::move(textToValue));

        return std::make_unique<juce::AudioParameterFloat>(
            id,
            name,
            range,
            defaultValue,
            attributes);
    }
}

namespace rsat::param
{
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

        auto dbToText = [] (float value, int) { return juce::String(value, 1) + " dB"; };
        auto textToDb = [] (const juce::String& text) { return text.getFloatValue(); };
        auto percentToText = [] (float value, int) { return juce::String(juce::roundToInt(value * 100.0f)) + " %"; };
        auto textToPercent = [] (const juce::String& text) { return text.getFloatValue() / 100.0f; };

        params.push_back(makeFloatParameter(
            { inputGain, 1 },
            "Input Gain",
            { -24.0f, 24.0f, 0.01f },
            0.0f,
            "dB",
            dbToText,
            textToDb));

        params.push_back(makeFloatParameter(
            { drive, 1 },
            "Drive",
            { 0.0f, 1.0f, 0.001f },
            0.2f,
            {},
            percentToText,
            textToPercent));

        params.push_back(makeFloatParameter(
            { tone, 1 },
            "Tone",
            { 0.0f, 1.0f, 0.001f },
            0.5f,
            {},
            percentToText,
            textToPercent));

        params.push_back(makeFloatParameter(
            { outputGain, 1 },
            "Output Gain",
            { -24.0f, 12.0f, 0.01f },
            0.0f,
            "dB",
            dbToText,
            textToDb));

        params.push_back(makeFloatParameter(
            { mix, 1 },
            "Mix",
            { 0.0f, 1.0f, 0.001f },
            1.0f,
            {},
            percentToText,
            textToPercent));

        return { params.begin(), params.end() };
    }
}
