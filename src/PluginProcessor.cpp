#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <memory>

RSaturatorAudioProcessor::RSaturatorAudioProcessor()
    : AudioProcessor(BusesProperties()
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", rsat::param::createParameterLayout())
{
}

void RSaturatorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    saturator.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void RSaturatorAudioProcessor::releaseResources()
{
    saturator.reset();
}

bool RSaturatorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainOutput = layouts.getMainOutputChannelSet();
    const auto& mainInput = layouts.getMainInputChannelSet();

    if (mainOutput != juce::AudioChannelSet::mono()
        && mainOutput != juce::AudioChannelSet::stereo())
        return false;

    return mainInput == mainOutput;
}

void RSaturatorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto channel = totalNumInputChannels; channel < totalNumOutputChannels; ++channel)
        buffer.clear(channel, 0, buffer.getNumSamples());

    const auto inputGain = apvts.getRawParameterValue(rsat::param::inputGain)->load();
    const auto drive = apvts.getRawParameterValue(rsat::param::drive)->load();
    const auto tone = apvts.getRawParameterValue(rsat::param::tone)->load();
    const auto outputGain = apvts.getRawParameterValue(rsat::param::outputGain)->load();
    const auto mix = apvts.getRawParameterValue(rsat::param::mix)->load();

    saturator.setParameters(inputGain, drive, tone, outputGain, mix);
    saturator.process(buffer);
}

juce::AudioProcessorEditor* RSaturatorAudioProcessor::createEditor()
{
    return new RSaturatorAudioProcessorEditor(*this);
}

bool RSaturatorAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String RSaturatorAudioProcessor::getName() const
{
    return "R-Saturator";
}

bool RSaturatorAudioProcessor::acceptsMidi() const
{
    return false;
}

bool RSaturatorAudioProcessor::producesMidi() const
{
    return false;
}

bool RSaturatorAudioProcessor::isMidiEffect() const
{
    return false;
}

double RSaturatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RSaturatorAudioProcessor::getNumPrograms()
{
    return 1;
}

int RSaturatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RSaturatorAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String RSaturatorAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void RSaturatorAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void RSaturatorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); state.isValid())
    {
        std::unique_ptr<juce::XmlElement> xml(state.createXml());
        copyXmlToBinary(*xml, destData);
    }
}

void RSaturatorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RSaturatorAudioProcessor();
}
