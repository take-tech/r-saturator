#include "PluginEditor.h"
#include "UserSettings.h"

#include <cmath>

namespace
{
    constexpr int editorWidth = 620;
    constexpr int editorHeight = 284;

    RSaturatorAudioProcessorEditor::Theme getThemeForIndex(int index)
    {
        switch (index)
        {
            case 1:
                return {
                    juce::Colour(0xff12110f),
                    juce::Colour(0xff211d17),
                    juce::Colour(0xffffb23f),
                    juce::Colour(0xffffd27a),
                    juce::Colour(0xfff5efe5),
                    juce::Colour(0xffc9b79c),
                    juce::Colour(0xff181612)
                };

            case 2:
                return {
                    juce::Colour(0xff120d10),
                    juce::Colour(0xff21151b),
                    juce::Colour(0xffff3f66),
                    juce::Colour(0xffff8a9d),
                    juce::Colour(0xfff7edf1),
                    juce::Colour(0xffd0a8b3),
                    juce::Colour(0xff181014)
                };

            case 3:
                return {
                    juce::Colour(0xff101112),
                    juce::Colour(0xff1c1e20),
                    juce::Colour(0xffd8d8d8),
                    juce::Colour(0xffffffff),
                    juce::Colour(0xfff2f2f2),
                    juce::Colour(0xffb6b8ba),
                    juce::Colour(0xff17191b)
                };

            case 4:
                return {
                    juce::Colour(0xfffff8fb),
                    juce::Colour(0xffffffff),
                    juce::Colour(0xffff5c9a),
                    juce::Colour(0xffff9dc3),
                    juce::Colour(0xff30242a),
                    juce::Colour(0xff8c6574),
                    juce::Colour(0xffffedf4)
                };

            default:
                return {
                    juce::Colour(0xff111113),
                    juce::Colour(0xff1d1d21),
                    juce::Colour(0xffff4f1f),
                    juce::Colour(0xffff9a3d),
                    juce::Colour(0xfff2f2f2),
                    juce::Colour(0xffd8d8d8),
                    juce::Colour(0xff17171a)
                };
        }
    }
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
    configureEffectButton();
    configureThemeButton();
    audioProcessor.getValueTreeState().addParameterListener(rsat::param::theme, this);
    applyTheme();

    setSize(editorWidth, editorHeight);
}

RSaturatorAudioProcessorEditor::~RSaturatorAudioProcessorEditor()
{
    audioProcessor.getValueTreeState().removeParameterListener(rsat::param::theme, this);
}

void RSaturatorAudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto theme = getCurrentTheme();

    g.fillAll(theme.background);

    auto bounds = getLocalBounds().toFloat();
    g.setColour(theme.panel);
    g.fillRoundedRectangle(bounds.reduced(12.0f), 8.0f);

    g.setColour(theme.accent);
    g.setFont(juce::FontOptions(24.0f, juce::Font::bold));
    g.drawText("R-Saturator", 28, 20, getWidth() - 56, 32, juce::Justification::centredLeft);

    g.setColour(theme.mutedText);
    g.setFont(juce::FontOptions(13.0f));
    g.drawText("Analog color, softly driven", 30, 52, getWidth() - 60, 22, juce::Justification::centredLeft);
}

void RSaturatorAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(24);
    auto header = area.removeFromTop(72);
    auto controls = header.removeFromRight(154).removeFromTop(30);
    themeButton.setBounds(controls.removeFromRight(30));
    controls.removeFromRight(10);
    effectButton.setBounds(controls);

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
    knob.slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(knob.slider);

    knob.label.setText(labelText, juce::dontSendNotification);
    knob.label.setJustificationType(juce::Justification::centred);
    knob.label.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    addAndMakeVisible(knob.label);

    knob.attachment = std::make_unique<SliderAttachment>(
        audioProcessor.getValueTreeState(),
        parameterId,
        knob.slider);
}

void RSaturatorAudioProcessorEditor::applyTheme()
{
    const auto theme = getCurrentTheme();
    const auto trackColour = getCurrentThemeIndex() == 4
        ? juce::Colour(0xffd8b9c8)
        : theme.panel.brighter(0.55f);

    for (auto& knob : knobs)
    {
        knob.slider.setColour(juce::Slider::rotarySliderFillColourId, theme.accent);
        knob.slider.setColour(juce::Slider::rotarySliderOutlineColourId, trackColour);
        knob.slider.setColour(juce::Slider::thumbColourId, theme.accentAlt);
        knob.slider.setColour(juce::Slider::textBoxTextColourId, theme.text);
        knob.slider.setColour(juce::Slider::textBoxBackgroundColourId, theme.control);
        knob.label.setColour(juce::Label::textColourId, theme.text);
    }

    effectButton.setColour(juce::ToggleButton::textColourId, theme.text);
    effectButton.setColour(juce::ToggleButton::tickColourId, theme.accentAlt);
    effectButton.setColour(juce::ToggleButton::tickDisabledColourId, theme.mutedText.withAlpha(0.45f));
    themeButton.setTheme(theme);

    repaint();
}

void RSaturatorAudioProcessorEditor::configureEffectButton()
{
    effectButton.setButtonText("Effect");
    effectButton.setClickingTogglesState(true);
    addAndMakeVisible(effectButton);

    effectAttachment = std::make_unique<ButtonAttachment>(
        audioProcessor.getValueTreeState(),
        rsat::param::effectOn,
        effectButton);
}

void RSaturatorAudioProcessorEditor::configureThemeButton()
{
    themeSelector.addItem("Ember", 1);
    themeSelector.addItem("Amber", 2);
    themeSelector.addItem("Ruby", 3);
    themeSelector.addItem("Graphite", 4);
    themeSelector.addItem("Blush", 5);
    themeSelector.onChange = [this] { applyTheme(); };
    addChildComponent(themeSelector);

    themeAttachment = std::make_unique<ComboBoxAttachment>(
        audioProcessor.getValueTreeState(),
        rsat::param::theme,
        themeSelector);

    themeButton.onClick = [this] { showThemeMenu(); };
    addAndMakeVisible(themeButton);
}

void RSaturatorAudioProcessorEditor::showThemeMenu()
{
    juce::PopupMenu menu;
    const auto selectedIndex = getCurrentThemeIndex();

    menu.addItem(1, "Ember", true, selectedIndex == 0);
    menu.addItem(2, "Amber", true, selectedIndex == 1);
    menu.addItem(3, "Ruby", true, selectedIndex == 2);
    menu.addItem(4, "Graphite", true, selectedIndex == 3);
    menu.addItem(5, "Blush", true, selectedIndex == 4);

    menu.showMenuAsync(
        juce::PopupMenu::Options().withTargetComponent(themeButton),
        [this] (int result)
        {
            if (result > 0)
                setThemeIndex(result - 1);
        });
}

void RSaturatorAudioProcessorEditor::setThemeIndex(int themeIndex)
{
    rsat::settings::saveDefaultThemeIndex(themeIndex);
    themeSelector.setSelectedId(themeIndex + 1, juce::sendNotificationSync);
}

void RSaturatorAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    juce::ignoreUnused(newValue);

    if (parameterID == rsat::param::theme)
        triggerAsyncUpdate();
}

void RSaturatorAudioProcessorEditor::handleAsyncUpdate()
{
    applyTheme();
}

int RSaturatorAudioProcessorEditor::getCurrentThemeIndex() const
{
    const auto selectedId = themeSelector.getSelectedId();

    if (selectedId > 0)
        return juce::jlimit(0, 4, selectedId - 1);

    if (auto* value = audioProcessor.getValueTreeState().getRawParameterValue(rsat::param::theme))
        return juce::jlimit(0, 4, static_cast<int>(std::round(value->load())));

    return 0;
}

RSaturatorAudioProcessorEditor::Theme RSaturatorAudioProcessorEditor::getCurrentTheme() const
{
    return getThemeForIndex(getCurrentThemeIndex());
}

RSaturatorAudioProcessorEditor::ThemeIconButton::ThemeIconButton()
    : juce::Button("Theme")
{
    setTitle("Theme");
}

void RSaturatorAudioProcessorEditor::ThemeIconButton::setTheme(Theme newTheme)
{
    theme = newTheme;
    repaint();
}

void RSaturatorAudioProcessorEditor::ThemeIconButton::paintButton(
    juce::Graphics& g,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    const auto bounds = getLocalBounds().toFloat().reduced(1.0f);
    const auto alpha = shouldDrawButtonAsDown ? 0.95f : (shouldDrawButtonAsHighlighted ? 0.85f : 0.72f);

    g.setColour(theme.control.withAlpha(alpha));
    g.fillRoundedRectangle(bounds, 7.0f);

    g.setColour(theme.accent.withAlpha(0.8f));
    g.drawRoundedRectangle(bounds, 7.0f, 1.2f);

    const auto dotArea = bounds.reduced(7.0f, 6.0f);
    const auto radius = 3.5f;
    g.setColour(theme.accent);
    g.fillEllipse(dotArea.getX(), dotArea.getY() + 4.0f, radius * 2.0f, radius * 2.0f);
    g.setColour(theme.accentAlt);
    g.fillEllipse(dotArea.getCentreX() - radius, dotArea.getY(), radius * 2.0f, radius * 2.0f);
    g.setColour(theme.mutedText);
    g.fillEllipse(dotArea.getRight() - radius * 2.0f, dotArea.getY() + 4.0f, radius * 2.0f, radius * 2.0f);
}
