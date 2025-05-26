#include "PluginProcessor.h"
#include "PluginEditor.h"

MyReverbAudioProcessorEditor::MyReverbAudioProcessorEditor(MyReverbAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),

    listenerPos({ *audioProcessor.parameters.getRawParameterValue("listenerX"),
                   *audioProcessor.parameters.getRawParameterValue("listenerY") }),
    sourcePos({ *audioProcessor.parameters.getRawParameterValue("sourceX"),
                *audioProcessor.parameters.getRawParameterValue("sourceY") }),
    roomSizeAttachment(audioProcessor.parameters, "roomSize", roomSizeSlider),
    dampingAttachment(audioProcessor.parameters, "damping", dampingSlider),
    wetLevelAttachment(audioProcessor.parameters, "wetLevel", wetLevelSlider),
    dryLevelAttachment(audioProcessor.parameters, "dryLevel", dryLevelSlider),
    widthAttachment(audioProcessor.parameters, "width", widthSlider),
    highPassAttachment(audioProcessor.parameters, "HighPassFilter", highPassSlider),
    lowPassAttachment(audioProcessor.parameters, "LowPassFilter", lowPassSlider),
    decayTimeAttachment(audioProcessor.parameters, "decayTime", decayTimeSlider),
    diffusionAttachment(audioProcessor.parameters, "diffusion", diffusionSlider),
    earlyReflectionsAttachment(audioProcessor.parameters, "earlyReflections", earlyReflectionsSlider),
    lateReflectionsAttachment(audioProcessor.parameters, "lateReflections", lateReflectionsSlider), 
    listenerXAttachment(audioProcessor.parameters, "listenerX", listenerXSlider),
    listenerYAttachment(audioProcessor.parameters, "listenerY", listenerYSlider),
    sourceXAttachment(audioProcessor.parameters, "sourceX", sourceXSlider),
    sourceYAttachment(audioProcessor.parameters, "sourceY", sourceYSlider)
{
    // Configure Room Size Slider and Label
    roomSizeSlider.setSliderStyle(juce::Slider::Rotary);
    roomSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(roomSizeSlider);
    roomSizeLabel.setText("Room Size", juce::dontSendNotification);
    roomSizeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(roomSizeLabel);

    // Width Slider
    widthSlider.setSliderStyle(juce::Slider::Rotary);
    widthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(widthSlider);

    widthLabel.setText("Width", juce::dontSendNotification);
    widthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(widthLabel);

    // Configure Damping Slider and Label
    dampingSlider.setSliderStyle(juce::Slider::Rotary);
    dampingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(dampingSlider);
    dampingLabel.setText("Damping", juce::dontSendNotification);
    dampingLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dampingLabel);

    // Configure Wet Level Slider and Label
    wetLevelSlider.setSliderStyle(juce::Slider::Rotary);
    wetLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(wetLevelSlider);
    wetLevelLabel.setText("Wet Level", juce::dontSendNotification);
    wetLevelLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(wetLevelLabel);

    // Configure Dry Level Slider and Label
    dryLevelSlider.setSliderStyle(juce::Slider::Rotary);
    dryLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(dryLevelSlider);
    dryLevelLabel.setText("Dry Level", juce::dontSendNotification);
    dryLevelLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dryLevelLabel);

    // Configure Pre-Delay Slider and Label
    preDelaySlider.setSliderStyle(juce::Slider::Rotary);
    preDelaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(preDelaySlider);
    preDelayLabel.setText("Pre-Delay", juce::dontSendNotification);
    preDelayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(preDelayLabel);

    // Decay Time
    decayTimeSlider.setSliderStyle(juce::Slider::Rotary);
    decayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    decayTimeSlider.setRange(0.1, 10.0, 0.1); // Reverb tail length in seconds
    decayTimeSlider.textFromValueFunction = [](double value) { return juce::String(value, 1) + " s"; };
    addAndMakeVisible(decayTimeSlider);
    decayTimeLabel.setText("Decay Time", juce::dontSendNotification);
    decayTimeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(decayTimeLabel);

    //Corners
    roomCornersSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    roomCornersSlider.setRange(0, 20, 1);
    roomCornersSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(roomCornersSlider);
    roomCornersSlider.onValueChange = [this]() {
        auto cornersValue = static_cast<int>(roomCornersSlider.getValue());
        auto* cornersParam = audioProcessor.parameters.getParameter("roomCorners");
        if (cornersParam != nullptr)
        {
            cornersParam->setValueNotifyingHost(cornersValue / roomCornersSlider.getMaximum());
            audioProcessor.generateRoomWalls(cornersValue, 10.0f, 5.0f);
        }
        repaint();
        };

    roomCornersLabel.setText("Room Corners", juce::dontSendNotification);
    roomCornersLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(roomCornersLabel);

    // Diffusion
    diffusionSlider.setSliderStyle(juce::Slider::Rotary);
    diffusionSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    diffusionSlider.setRange(0.0, 1.0, 0.01); // Smoothness of reverb
    addAndMakeVisible(diffusionSlider);
    diffusionLabel.setText("Diffusion", juce::dontSendNotification);
    diffusionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(diffusionLabel);

    // Early Reflections
    earlyReflectionsSlider.setSliderStyle(juce::Slider::Rotary);
    earlyReflectionsSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    earlyReflectionsSlider.setRange(0.0, 1.0, 0.01); // Density of initial reflections
    addAndMakeVisible(earlyReflectionsSlider);
    earlyReflectionsLabel.setText("Early Reflections", juce::dontSendNotification);
    earlyReflectionsLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(earlyReflectionsLabel);

    // Late Reflections
    lateReflectionsSlider.setSliderStyle(juce::Slider::Rotary);
    lateReflectionsSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    lateReflectionsSlider.setRange(0.0, 1.0, 0.01); // Density of late reflections
    addAndMakeVisible(lateReflectionsSlider);
    lateReflectionsLabel.setText("Late Reflections", juce::dontSendNotification);
    lateReflectionsLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lateReflectionsLabel);

    // Configure Low Pass Filter Slider and Label
    lowPassSlider.setSliderStyle(juce::Slider::Rotary);
    lowPassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    lowPassSlider.setRange(200.0, 20000.0, 1.0); // Set frequency range
    lowPassSlider.setSkewFactorFromMidPoint(2000.0); // Logarithmic scaling
    lowPassSlider.textFromValueFunction = [](double value) {
        return juce::String(value, 1) + " Hz"; // Append " Hz" to the value
        };
    addAndMakeVisible(lowPassSlider);


    lowPassLabel.setText("Low Pass", juce::dontSendNotification);
    lowPassLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lowPassLabel);

    // Configure High Pass Filter Slider and Label
    highPassSlider.setSliderStyle(juce::Slider::Rotary);
    highPassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    highPassSlider.setRange(20.0, 2000.0, 1.0); // Filters out frequencies below the cutoff
    highPassSlider.setSkewFactorFromMidPoint(500.0); // Logarithmic scaling for better control
    highPassSlider.textFromValueFunction = [](double value) {
        return juce::String(value, 1) + " Hz"; // Append " Hz" to display value
        };
    addAndMakeVisible(highPassSlider);

    highPassLabel.setText("High Pass", juce::dontSendNotification);
    highPassLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(highPassLabel);


    // Configure Listener X Slider and Label
    listenerXSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    listenerXSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(listenerXSlider);
    listenerXLabel.setText("Listener X", juce::dontSendNotification);
    listenerXLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(listenerXLabel);
    listenerXSlider.onValueChange = [this] { repaint(); };

    // Configure Listener Y Slider and Label
    listenerYSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    listenerYSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(listenerYSlider);
    listenerYLabel.setText("Listener Y", juce::dontSendNotification);
    listenerYLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(listenerYLabel);
    listenerYSlider.onValueChange = [this] { repaint(); };

    // Configure Source X Slider and Label
    sourceXSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    sourceXSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(sourceXSlider);
    sourceXLabel.setText("Source X", juce::dontSendNotification);
    sourceXLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(sourceXLabel);
    sourceXSlider.onValueChange = [this] { repaint(); };

    // Configure Source Y Slider and Label
    sourceYSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    sourceYSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(sourceYSlider);
    sourceYLabel.setText("Source Y", juce::dontSendNotification);
    sourceYLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(sourceYLabel);
    sourceYSlider.onValueChange = [this] { repaint(); };


    setSize(1000, 600);
}

MyReverbAudioProcessorEditor::~MyReverbAudioProcessorEditor() {}

juce::Point<float> MyReverbAudioProcessorEditor::constrainToPolygon(float centerX, float centerY,
    float radiusX, float radiusY,
    int corners, juce::Point<float> point)
{
    float distance = std::sqrt(std::pow(point.x - centerX, 2) + std::pow(point.y - centerY, 2));

    // Generate the polygon points
    std::vector<juce::Point<float>> polygonPoints;
    for (int i = 0; i < corners; ++i)
    {
        float angle = juce::MathConstants<float>::twoPi * (float)i / (float)corners;
        float x = centerX + radiusX * std::cos(angle);
        float y = centerY + radiusY * std::sin(angle);
        polygonPoints.emplace_back(x, y);
    }

    // Find the closest segment of the polygon
    juce::Point<float> closestPoint = point;
    float minDistance = std::numeric_limits<float>::max();

    auto dotProduct = [](juce::Point<float> a, juce::Point<float> b) {
        return (a.x * b.x + a.y * b.y);
        };

    auto lengthSquared = [](juce::Point<float> a) {
        return (a.x * a.x + a.y * a.y);
        };

    for (size_t i = 0; i < polygonPoints.size(); ++i)
    {
        juce::Point<float> p1 = polygonPoints[i];
        juce::Point<float> p2 = polygonPoints[(i + 1) % polygonPoints.size()];

        auto closest = p1 + (p2 - p1) * std::clamp(dotProduct(point - p1, p2 - p1) / lengthSquared(p2 - p1), 0.0f, 1.0f);
        float dist = closest.getDistanceFrom(point);

        if (dist < minDistance)
        {
            minDistance = dist;
            closestPoint = closest;
        }
    }

    // Constrain to polygon boundary if outside the radius
    if (distance > std::min(radiusX, radiusY))
        return closestPoint;

    return point;
}



void MyReverbAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Room properties
    float roomSize = *audioProcessor.parameters.getRawParameterValue("roomSize");
    corners = static_cast<int>(*audioProcessor.parameters.getRawParameterValue("roomCorners")); // Store corners

    const int visualizerX = 200;
    const int visualizerY = getHeight() - 220;
    const int visualizerWidth = getWidth() - 400;
    const int visualizerHeight = 200;

    // Center of the room visualizer
    centerX = visualizerX + visualizerWidth / 2.0f; // Store centerX
    centerY = visualizerY + visualizerHeight / 2.0f; // Store centerY

    radiusX = (visualizerWidth / 2.0f) * roomSize; // Store radiusX
    radiusY = (visualizerHeight / 2.0f) * roomSize; // Store radiusY

    // Generate polygon points
    juce::Path roomShape;
    for (int i = 0; i < corners; ++i)
    {
        float angle = juce::MathConstants<float>::twoPi * (float)i / (float)corners;

        float x = centerX + radiusX * std::cos(angle);
        float y = centerY + radiusY * std::sin(angle);

        if (i == 0)
            roomShape.startNewSubPath(x, y);
        else
            roomShape.lineTo(x, y);
    }
    roomShape.closeSubPath();

    // Draw the polygon
    g.setColour(juce::Colours::yellow);
    g.strokePath(roomShape, juce::PathStrokeType(2.0f));

    // Listener and source positions
    g.setColour(juce::Colours::blue);
    juce::Point<float> listenerPoint = constrainToPolygon(centerX, centerY, radiusX, radiusY, corners, listenerPos);
    g.fillEllipse(listenerPoint.x - 7, listenerPoint.y - 7, 14.0f, 14.0f);

    g.setColour(juce::Colours::red);
    juce::Point<float> sourcePoint = constrainToPolygon(centerX, centerY, radiusX, radiusY, corners, sourcePos);
    g.fillEllipse(sourcePoint.x - 7, sourcePoint.y - 7, 14.0f, 14.0f);

    // Add room labels
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("Room Simulation", visualizerX, visualizerY - 30, visualizerWidth, 20, juce::Justification::centred, 1);
}

float MyReverbAudioProcessorEditor::scaleX(float x) const
{
    float innerRoomX = 200 + ((getWidth() - 400) - (getWidth() - 400) * *audioProcessor.parameters.getRawParameterValue("roomSize")) / 2.0f;
    float scaledRoomWidth = (getWidth() - 400) * *audioProcessor.parameters.getRawParameterValue("roomSize");
    return innerRoomX + (x / 10.0f) * scaledRoomWidth;
}
float MyReverbAudioProcessorEditor::scaleY(float y) const
{
    float innerRoomY = (getHeight() - 220) + ((200 - 200 * *audioProcessor.parameters.getRawParameterValue("roomSize")) / 2.0f);
    float scaledRoomHeight = 200 * *audioProcessor.parameters.getRawParameterValue("roomSize");
    return innerRoomY + (y / 5.0f) * scaledRoomHeight;
}


float MyReverbAudioProcessorEditor::reverseScaleX(float mouseX) const
{
    float innerRoomX = 200 + ((getWidth() - 400) - (getWidth() - 400) * *audioProcessor.parameters.getRawParameterValue("roomSize")) / 2.0f;
    float scaledRoomWidth = (getWidth() - 400) * *audioProcessor.parameters.getRawParameterValue("roomSize");
    return (mouseX - innerRoomX) / scaledRoomWidth * 10.0f;
}


float MyReverbAudioProcessorEditor::reverseScaleY(float mouseY) const
{
    float innerRoomY = (getHeight() - 220) + ((200 - 200 * *audioProcessor.parameters.getRawParameterValue("roomSize")) / 2.0f);
    float scaledRoomHeight = 200 * *audioProcessor.parameters.getRawParameterValue("roomSize");
    return (mouseY - innerRoomY) / scaledRoomHeight * 5.0f;
}




void MyReverbAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    auto checkHit = [&](juce::Point<float> dotPos, float mouseX, float mouseY) {
        // Scale logical position to visual position
        float dotX = scaleX(dotPos.x);
        float dotY = scaleY(dotPos.y);
        return std::abs(mouseX - dotX) <= 10 && std::abs(mouseY - dotY) <= 10;
        };

    // Determine if the listener or source dot was clicked
    if (checkHit(listenerPos, event.x, event.y))
    {
        isDraggingListener = true;
    }
    else if (checkHit(sourcePos, event.x, event.y))
    {
        isDraggingSource = true;
    }
}


void MyReverbAudioProcessorEditor::mouseDrag(const juce::MouseEvent& event)
{
    if (isDraggingListener)
    {
        // Update listener position
        listenerPos.x = juce::jlimit(0.0f, 10.0f, reverseScaleX(event.x));
        listenerPos.y = juce::jlimit(0.0f, 5.0f, reverseScaleY(event.y));

        // Constrain to the polygon
        listenerPos = constrainToPolygon(centerX, centerY, radiusX, radiusY, corners, listenerPos);

        // Notify host
        audioProcessor.parameters.getParameter("listenerX")->setValueNotifyingHost(listenerPos.x / 10.0f);
        audioProcessor.parameters.getParameter("listenerY")->setValueNotifyingHost(listenerPos.y / 5.0f);
    }
    else if (isDraggingSource)
    {
        // Update source position
        sourcePos.x = juce::jlimit(0.0f, 10.0f, reverseScaleX(event.x));
        sourcePos.y = juce::jlimit(0.0f, 5.0f, reverseScaleY(event.y));

        // Constrain to the polygon
        sourcePos = constrainToPolygon(centerX, centerY, radiusX, radiusY, corners, sourcePos);

        // Notify host
        audioProcessor.parameters.getParameter("sourceX")->setValueNotifyingHost(sourcePos.x / 10.0f);
        audioProcessor.parameters.getParameter("sourceY")->setValueNotifyingHost(sourcePos.y / 5.0f);
    }

    repaint(); // Refresh the UI
}



void MyReverbAudioProcessorEditor::mouseUp(const juce::MouseEvent&)
{
    isDraggingListener = false;
    isDraggingSource = false;
}

void MyReverbAudioProcessorEditor::mouseMove(const juce::MouseEvent& event)
{
    auto checkHover = [&](juce::Point<float> dotPos, float mouseX, float mouseY) {
        float dotX = scaleX(dotPos.x);
        float dotY = scaleY(dotPos.y);
        return std::abs(mouseX - dotX) < 10 && std::abs(mouseY - dotY) < 10;
        };

    if (checkHover(listenerPos, event.x, event.y))
        hoverListener = true;
    else if (checkHover(sourcePos, event.x, event.y))
        hoverSource = true;
    else
    {
        hoverListener = false;
        hoverSource = false;
    }

    repaint();
}



void MyReverbAudioProcessorEditor::resized()
{
    const int padding = 20;
    const int mediumKnobWidth = 70;
    const int mediumKnobHeight = 70;
    const int largeKnobWidth = 80;
    const int largeKnobHeight = 80;
    const int labelHeight = 20;
    const int rowSpacing = 20;

    // Separator X-coordinate for dividing the UI into left and right sections
    const int separatorX = getWidth() / 2;

    // Room Controls on the Right Side (Medium Knobs)
    roomSizeSlider.setBounds(separatorX + padding, padding, mediumKnobWidth, mediumKnobHeight);
    roomSizeLabel.setBounds(separatorX + padding, padding + mediumKnobHeight + 5, mediumKnobWidth, labelHeight);

    dampingSlider.setBounds(separatorX + mediumKnobWidth + 2 * padding, padding, mediumKnobWidth, mediumKnobHeight);
    dampingLabel.setBounds(separatorX + mediumKnobWidth + 2 * padding, padding + mediumKnobHeight + 5, mediumKnobWidth, labelHeight);

    decayTimeSlider.setBounds(separatorX + padding, padding + mediumKnobHeight + labelHeight + rowSpacing, mediumKnobWidth, mediumKnobHeight);
    decayTimeLabel.setBounds(separatorX + padding, padding + 2 * mediumKnobHeight + labelHeight + rowSpacing + 5, mediumKnobWidth, labelHeight);

    diffusionSlider.setBounds(separatorX + mediumKnobWidth + 2 * padding, padding + mediumKnobHeight + labelHeight + rowSpacing, mediumKnobWidth, mediumKnobHeight);
    diffusionLabel.setBounds(separatorX + mediumKnobWidth + 2 * padding, padding + 2 * mediumKnobHeight + labelHeight + rowSpacing + 5, mediumKnobWidth, labelHeight);

    // Left Side Sliders (Medium Knobs)
    earlyReflectionsSlider.setBounds(padding, padding, mediumKnobWidth, mediumKnobHeight);
    earlyReflectionsLabel.setBounds(padding, padding + mediumKnobHeight + 5, mediumKnobWidth, labelHeight);

    lateReflectionsSlider.setBounds(padding + mediumKnobWidth + padding, padding, mediumKnobWidth, mediumKnobHeight);
    lateReflectionsLabel.setBounds(padding + mediumKnobWidth + padding, padding + mediumKnobHeight + 5, mediumKnobWidth, labelHeight);

    preDelaySlider.setBounds(padding, padding + mediumKnobHeight + labelHeight + rowSpacing, mediumKnobWidth, mediumKnobHeight);
    preDelayLabel.setBounds(padding, padding + 2 * mediumKnobHeight + labelHeight + rowSpacing + 5, mediumKnobWidth, labelHeight);

    widthSlider.setBounds(padding + mediumKnobWidth + padding, padding + mediumKnobHeight + labelHeight + rowSpacing, mediumKnobWidth, mediumKnobHeight);
    widthLabel.setBounds(padding + mediumKnobWidth + padding, padding + 2 * mediumKnobHeight + labelHeight + rowSpacing + 5, mediumKnobWidth, labelHeight);

    // Bottom Row - Large Knobs (Wet Level, Dry Level, Low Pass, High Pass)
    const int bottomRowY = getHeight() - largeKnobHeight - labelHeight - padding;

    wetLevelSlider.setBounds(padding, bottomRowY, largeKnobWidth, largeKnobHeight);
    wetLevelLabel.setBounds(padding, bottomRowY + largeKnobHeight + 5, largeKnobWidth, labelHeight);

    dryLevelSlider.setBounds(padding + largeKnobWidth + padding, bottomRowY, largeKnobWidth, largeKnobHeight);
    dryLevelLabel.setBounds(padding + largeKnobWidth + padding, bottomRowY + largeKnobHeight + 5, largeKnobWidth, labelHeight);

    const int bottomRightX = getWidth() - largeKnobWidth - padding;

    lowPassSlider.setBounds(bottomRightX - largeKnobWidth - padding, bottomRowY, largeKnobWidth, largeKnobHeight);
    lowPassLabel.setBounds(bottomRightX - largeKnobWidth - padding, bottomRowY + largeKnobHeight + 5, largeKnobWidth, labelHeight);

    highPassSlider.setBounds(bottomRightX, bottomRowY, largeKnobWidth, largeKnobHeight);
    highPassLabel.setBounds(bottomRightX, bottomRowY + largeKnobHeight + 5, largeKnobWidth, labelHeight);

    // Visualizer
    const int visualizerWidth = getWidth() - 2 * padding;
    const int visualizerHeight = 180;
    const int visualizerX = padding;
    const int visualizerY = getHeight() - visualizerHeight - largeKnobHeight - 3 * padding;

    // Room Corners Slider Below Visualizer
    const int sliderWidth = 200;
    const int sliderHeight = 40;
    const int sliderLabelHeight = 20;

    const int sliderX = visualizerX + (visualizerWidth - sliderWidth) / 2;
    const int sliderY = visualizerY + visualizerHeight + padding;  // Position below the visualizer

    roomCornersSlider.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);
    roomCornersLabel.setBounds(sliderX, sliderY + sliderHeight + 5, sliderWidth, sliderLabelHeight);
}





