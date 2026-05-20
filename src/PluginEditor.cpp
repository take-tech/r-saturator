#include "PluginEditor.h"

namespace
{
    constexpr int editorWidth = 620;
    constexpr int editorHeight = 260;
}

RSaturatorAudioProcessorEditor::RSaturatorAudioProcessorEditor(RSaturatorAudioProcessor& ownerProcessor)
    : AudioProcessorEditor(&ownerProcessor),
      audioProcessor(ownerProcessor)
{
    configureKnob(knobs[0], "Input", rsat::param::inputGain);
    configureKnob(knobs[1], "Drive", rsat::param::drive);
    configureKnob(knobs[2], "Tone", rsat::param::tone);
    configureKnob(knobs[3], "Output", rsat::param::outputGain);
    configureKnob(knobs[4], "Mix", rsat::param::mix);

    setSize(editorWidth, editorHeight);
}

void RSaturatorAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff111113));

    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(0xff1d1d21));
    g.fillRoundedRectangle(bounds.reduced(12.0f), 8.0f);

    g.setColour(juce::Colour(0xffff4f1f));
    g.setFont(juce::FontOptions(24.0f, juce::Font::bold));
    g.drawText("R-Saturator", 28, 20, getWidth() - 56, 32, juce::Justification::centredLeft);

    g.setColour(juce::Colour(0xffd8d8d8));
    g.setFont(juce::FontOptions(13.0f));
    g.drawText("Light saturation", 30, 52, getWidth() - 60, 22, juce::Justification::centredLeft);
}

void RSaturatorAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(24);
    area.removeFromTop(72);

    const auto gap = 12;
    const auto knobWidth = (area.getWidth() - gap * 4) / 5;

    for (auto& knob : knobs)
    {
        auto item = area.removeFromLeft(knobWidth);
        area.removeFromLeft(gap);

        knob.slider.setBounds(item.removeFromTop(118).reduced(4));
        knob.label.setBounds(item.removeFromTop(28));
    }
}

void RSaturatorAudioProcessorEditor::configureKnob(
    Knob& knob,
    const juce::String& labelText,
    const juce::String& parameterId)
{
    knob.slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob.slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 76, 22);
    knob.slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffff4f1f));
    knob.slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff3a3a40));
    knob.slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffff9a3d));
    knob.slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xfff2f2f2));
    knob.slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    knob.slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff17171a));
    addAndMakeVisible(knob.slider);

    knob.label.setText(labelText, juce::dontSendNotification);
    knob.label.setJustificationType(juce::Justification::centred);
    knob.label.setColour(juce::Label::textColourId, juce::Colour(0xfff2f2f2));
    knob.label.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    addAndMakeVisible(knob.label);

    knob.attachment = std::make_unique<SliderAttachment>(
        audioProcessor.getValueTreeState(),
        parameterId,
        knob.slider);
}
