#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace rsat::param
{
    static constexpr auto inputGain  = "inputGain";
    static constexpr auto drive      = "drive";
    static constexpr auto tone       = "tone";
    static constexpr auto outputGain = "outputGain";
    static constexpr auto mix        = "mix";
    static constexpr auto effectOn   = "effectOn";
    static constexpr auto theme      = "theme";

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
}
