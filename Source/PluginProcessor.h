#pragma once

#include <JuceHeader.h>
#include <vector>

// Define Wall struct
struct Wall {
    juce::Point<float> start;
    juce::Point<float> end;
};

// Define Room struct
struct Room {
    std::vector<Wall> walls;
    juce::Point<float> listener = { 5.0f, 2.5f }; // Default listener position
    juce::Point<float> source = { 2.0f, 2.5f };   // Default source position
};

class MyReverbAudioProcessor : public juce::AudioProcessor
{
public:
    MyReverbAudioProcessor();
    ~MyReverbAudioProcessor() override;

    

    // Room and Reverb as public members for direct access
    Room room;
    juce::dsp::Reverb reverb;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void adjustStereoWidth(juce::AudioBuffer<float>& buffer);
    float calculateAverageReflectionTime();


    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Parameter management
    juce::AudioProcessorValueTreeState parameters;

    // Filter parameter smoothing
    juce::SmoothedValue<float> highPassSmooth{ 20.0f };  // Default HPF frequency
    juce::SmoothedValue<float> lowPassSmooth{ 20000.0f }; // Default LPF frequency

    // Generates room walls based on the number of corners
    void generateRoomWalls(int corners, float width, float height);

    //Generate Dry and wet level
    juce::SmoothedValue<float> smoothedWetLevel;
    juce::SmoothedValue<float> smoothedDryLevel;


private:
    // Create parameter layout
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::dsp::ProcessorChain<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Filter<float>> inputFilterChain;
    juce::dsp::Reverb::Parameters reverbParameters;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> preDelayLine;
    // DSP processing objects
    juce::dsp::ProcessorChain<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Filter<float>> filterChain;

    // Tracks the last number of corners to avoid redundant wall updates
    int lastCorners = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyReverbAudioProcessor)
};
