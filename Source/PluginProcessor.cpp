#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"

MyReverbAudioProcessor::MyReverbAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    parameters(*this, nullptr, "Parameters", createParameterLayout()),
    lastCorners(3) // Initialize lastCorners to the default

{
    generateRoomWalls(4, 10.0f, 5.0f); // Default to 4 corners
}

MyReverbAudioProcessor::~MyReverbAudioProcessor() {}
// Add this public method to MyReverbAudioProcessor
juce::AudioProcessorValueTreeState::ParameterLayout MyReverbAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;


    layout.add(std::make_unique<juce::AudioParameterFloat>("roomSize", "Room Size", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("damping", "Damping", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("wetLevel", "Wet Level", 0.0f, 1.0f, 0.33f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("dryLevel", "Dry Level", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("width", "Width", 0.0f, 1.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("freezeMode", "Freeze Mode", 0.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("HighPassFilter", "High-pass Filter", 20.0f, 2000.0f, 20.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowPassFilter", "Low-pass Filter", 200.0f, 20000.0f, 20000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("preDelay", "Pre-Delay", 0.0f, 1000.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("listenerX", "Listener X", 0.0f, 10.0f, 5.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("listenerY", "Listener Y", 0.0f, 5.0f, 2.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("sourceX", "Source X", 0.0f, 10.0f, 2.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("sourceY", "Source Y", 0.0f, 5.0f, 2.5f));
    layout.add(std::make_unique<juce::AudioParameterInt>("roomCorners", "Room Corners", 3, 20, 4));

    return layout;
}

void MyReverbAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();

    reverb.prepare(spec);

    // Initialize pre-delay line (1 second maximum delay)
    preDelayLine.reset();
    preDelayLine.setMaximumDelayInSamples(static_cast<int>(sampleRate));  // Ensure 1s max delay

    smoothedWetLevel.reset(sampleRate, 0.1);  // Smooth over 100ms
    smoothedDryLevel.reset(sampleRate, 0.1);
}



float MyReverbAudioProcessor::calculateAverageReflectionTime()
{
    float totalTime = 0.0f;
    int numReflections = 0;

    // Speed of sound (in meters per second)
    const float speedOfSound = 343.0f;

    // Iterate through each wall and calculate reflection time
    for (const Wall& wall : room.walls)
    {
        // Calculate distance from source to the wall and then to the listener
        float sourceToWallDistance = room.source.getDistanceFrom(wall.start);
        float wallToListenerDistance = wall.end.getDistanceFrom(room.listener);

        // Total distance traveled by the reflection
        float totalDistance = sourceToWallDistance + wallToListenerDistance;

        // Calculate time taken for the reflection
        float reflectionTime = totalDistance / speedOfSound;
        reflectionTime = juce::jlimit(0.0f, 1.0f, reflectionTime);// Limit between 0 and 1 second
        if (reflectionTime < 0.0f || reflectionTime > 1.0f)
        {
            jassertfalse;  // Debug check
            return 0.0f;
        }

        totalTime += reflectionTime;
        numReflections++;
    }

    // Return the average reflection time
    return (numReflections > 0) ? totalTime / numReflections : 0.0f;
}



void MyReverbAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // Safety check: Ensure buffer is valid
    if (buffer.getNumSamples() == 0 || buffer.getNumChannels() == 0)
        return;  // Avoid processing if buffer is invalid

    // Calculate the average reflection time and convert it to milliseconds
    float reflectionTime = calculateAverageReflectionTime();
    float preDelayTimeMs = reflectionTime * 1000.0f;
    int preDelaySamples = static_cast<int>((preDelayTimeMs / 1000.0f) * getSampleRate());
    preDelaySamples = juce::jlimit(0, static_cast<int>(getSampleRate()), preDelaySamples);
    preDelayLine.setDelay(static_cast<float>(preDelaySamples));

    // Get parameter values with null checks
    auto* wetLevelPtr = parameters.getRawParameterValue("wetLevel");
    auto* dryLevelPtr = parameters.getRawParameterValue("dryLevel");
    if (wetLevelPtr == nullptr || dryLevelPtr == nullptr)
    {
        jassertfalse;  // Should never happen unless the parameters are not initialized properly
        return;
    }

    smoothedWetLevel.setTargetValue(*wetLevelPtr);
    smoothedDryLevel.setTargetValue(*dryLevelPtr);

    // Create an AudioBlock and context for the entire buffer
    auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);

    // Safety check: Ensure preDelayLine is properly initialized
    if (preDelayLine.getDelay() > 0)
    {
        // Process the entire buffer through the delay line
        preDelayLine.process(context);
    }

    // Reverb processing (on the delayed signal)
    juce::AudioBuffer<float> wetBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    wetBuffer.makeCopyOf(buffer);
    auto wetBlock = juce::dsp::AudioBlock<float>(wetBuffer);
    reverb.process(juce::dsp::ProcessContextReplacing<float>(wetBlock));

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        buffer.applyGain(channel, 0, buffer.getNumSamples(), smoothedDryLevel.getNextValue());
        wetBuffer.applyGain(channel, 0, wetBuffer.getNumSamples(), smoothedWetLevel.getNextValue());
        buffer.addFrom(channel, 0, wetBuffer, channel, 0, wetBuffer.getNumSamples());
    }

    // Optional stereo width adjustment
    adjustStereoWidth(buffer);
}




   


void MyReverbAudioProcessor::adjustStereoWidth(juce::AudioBuffer<float>& buffer)
{
    float widthValue = *parameters.getRawParameterValue("width");

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float left = buffer.getSample(0, sample);
        float right = buffer.getSample(1, sample);

        float mid = (left + right) * 0.5f;
        float side = (left - right) * 0.5f * widthValue;

        buffer.setSample(0, sample, mid + side);
        buffer.setSample(1, sample, mid - side);
    }
}

void MyReverbAudioProcessor::generateRoomWalls(int corners, float width, float height)
{
    room.walls.clear();
    float angleStep = juce::MathConstants<float>::twoPi / corners;

    for (int i = 0; i < corners; ++i)
    {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        juce::Point<float> start(width * 0.5f * std::cos(angle1), height * 0.5f * std::sin(angle1));
        juce::Point<float> end(width * 0.5f * std::cos(angle2), height * 0.5f * std::sin(angle2));

        room.walls.push_back({ start, end });
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyReverbAudioProcessor();
}

int MyReverbAudioProcessor::getNumPrograms() { return 1; }
int MyReverbAudioProcessor::getCurrentProgram() { return 0; }
void MyReverbAudioProcessor::setCurrentProgram(int) {}
const juce::String MyReverbAudioProcessor::getProgramName(int) { return {}; }
void MyReverbAudioProcessor::changeProgramName(int, const juce::String&) {}
double MyReverbAudioProcessor::getTailLengthSeconds() const { return 0.0; }
bool MyReverbAudioProcessor::isMidiEffect() const { return false; }
bool MyReverbAudioProcessor::producesMidi() const { return false; }
bool MyReverbAudioProcessor::acceptsMidi() const { return false; }
const juce::String MyReverbAudioProcessor::getName() const { return "MyReverb"; }
bool MyReverbAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* MyReverbAudioProcessor::createEditor() {
    return new MyReverbAudioProcessorEditor(*this);
}

void MyReverbAudioProcessor::releaseResources() {}

void MyReverbAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MyReverbAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr && xmlState->hasTagName(parameters.state.getType())) {
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}


