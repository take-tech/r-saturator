#pragma once

#include <array>
#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>

#include "PluginProcessor.h"

class RSaturatorAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                             private juce::AudioProcessorValueTreeState::Listener,
                                             private juce::AsyncUpdater
{
public:
    explicit RSaturatorAudioProcessorEditor(RSaturatorAudioProcessor&);
    ~RSaturatorAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    struct Theme
    {
        juce::Colour background;
        juce::Colour panel;
        juce::Colour accent;
        juce::Colour accentAlt;
        juce::Colour text;
        juce::Colour mutedText;
        juce::Colour control;
    };

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    class ThemeIconButton final : public juce::Button
    {
    public:
        ThemeIconButton();
        void setTheme(Theme newTheme);
        void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    private:
        Theme theme;
    };

    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<SliderAttachment> attachment;
    };

    void configureKnob(Knob& knob, const juce::String& labelText, const juce::String& parameterId);
    void configureEffectButton();
    void configureThemeButton();
    void showThemeMenu();
    void setThemeIndex(int themeIndex);
    void applyTheme();
    int getCurrentThemeIndex() const;
    Theme getCurrentTheme() const;
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void handleAsyncUpdate() override;

    RSaturatorAudioProcessor& audioProcessor;
    std::array<Knob, 5> knobs;
    juce::ToggleButton effectButton;
    std::unique_ptr<ButtonAttachment> effectAttachment;
    ThemeIconButton themeButton;
    juce::ComboBox themeSelector;
    std::unique_ptr<ComboBoxAttachment> themeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RSaturatorAudioProcessorEditor)
};
