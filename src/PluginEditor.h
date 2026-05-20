#pragma once

#include <array>
#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>

#include "PluginProcessor.h"

class RSaturatorAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit RSaturatorAudioProcessorEditor(RSaturatorAudioProcessor&);
    ~RSaturatorAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<SliderAttachment> attachment;
    };

    void configureKnob(Knob& knob, const juce::String& labelText, const juce::String& parameterId);

    RSaturatorAudioProcessor& audioProcessor;
    std::array<Knob, 5> knobs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RSaturatorAudioProcessorEditor)
};
