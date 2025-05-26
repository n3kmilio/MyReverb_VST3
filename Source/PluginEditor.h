#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class MyReverbAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    MyReverbAudioProcessorEditor(MyReverbAudioProcessor&);
    ~MyReverbAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    // Mouse interaction
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;
    void mouseMove(const juce::MouseEvent&) override;

private:
    MyReverbAudioProcessor& audioProcessor;

    // UI components
    juce::Slider roomSizeSlider;
    juce::Slider dampingSlider;
    juce::Slider wetLevelSlider;
    juce::Slider dryLevelSlider;
    juce::Slider preDelaySlider;
    juce::Slider widthSlider;
    juce::Slider highPassSlider;
    juce::Slider lowPassSlider;
    juce::Slider decayTimeSlider;
    juce::Slider diffusionSlider;
    juce::Slider earlyReflectionsSlider;
    juce::Slider lateReflectionsSlider;
    juce::Slider listenerXSlider, listenerYSlider;
    juce::Slider sourceXSlider, sourceYSlider;
    juce::Slider roomCornersSlider;

    // Labels
    juce::Label roomSizeLabel;
    juce::Label dampingLabel;
    juce::Label wetLevelLabel;
    juce::Label dryLevelLabel;
    juce::Label preDelayLabel;
    juce::Label highPassLabel;
    juce::Label widthLabel;
    juce::Label lowPassLabel;
    juce::Label decayTimeLabel;
    juce::Label diffusionLabel;
    juce::Label earlyReflectionsLabel;
    juce::Label lateReflectionsLabel;
    juce::Label listenerXLabel, listenerYLabel;
    juce::Label sourceXLabel, sourceYLabel;
    juce::Label roomCornersLabel;

    // Attachments
    juce::dsp::Reverb::Parameters reverbParameters;
    juce::AudioProcessorValueTreeState::SliderAttachment roomSizeAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment dampingAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment wetLevelAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment dryLevelAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment widthAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment highPassAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment lowPassAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment decayTimeAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment diffusionAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment earlyReflectionsAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment lateReflectionsAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment listenerXAttachment, listenerYAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment sourceXAttachment, sourceYAttachment;

    juce::Point<float> constrainToPolygon(float centerX, float centerY, float radiusX, float radiusY, int corners, juce::Point<float> point);

    // Dragging state
    bool isDraggingListener = false;
    bool isDraggingSource = false;
    bool hoverListener = false;
    bool hoverSource = false;

    // Dot positions
    juce::Point<float> listenerPos;
    juce::Point<float> sourcePos;

    // Scaling helper methods
    float scaleX(float x) const;  // Declaration only
    float scaleY(float y) const;  // Declaration only
    float reverseScaleX(float mouseX) const;  // Declaration only
    float reverseScaleY(float mouseY) const;  // Declaration only
    float centerX = 0.0f;
    float centerY = 0.0f;
    float radiusX = 0.0f;
    float radiusY = 0.0f;
    int corners = 3;


    // Background box properties for grouping
    const int boxWidth = 400;
    const int boxHeight = 100;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyReverbAudioProcessorEditor)
};
