/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginPrototypeAudioProcessorEditor::PluginPrototypeAudioProcessorEditor(PluginPrototypeAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), fDelayGui("F"), sDelayGui("S")

{
    setSize(420, 620);
    dpShadow.offset = { 175, 130 };
    dpShadow.radius = 2.5f;
    fDelayGui.sync = audioProcessor.fDelay.getSync();
    fDelayGui.visualizer.writeFromBuffer(audioProcessor.getBuffer(0), audioProcessor.currentSample(0));

    fDelayGui.syncBtn.onClick = [&]() 
    {
        fDelayGui.sync = audioProcessor.apvts.getRawParameterValue("fDelaySync")->load();
    };


    sDelayGui.sync = audioProcessor.sDelay.getSync();
    sDelayGui.visualizer.writeFromBuffer(audioProcessor.getBuffer(1), audioProcessor.currentSample(1));

    sDelayGui.syncBtn.onClick = [&]()
    {
        sDelayGui.sync = audioProcessor.apvts.getRawParameterValue("sDelaySync")->load();
    };

    addAndMakeVisible(fDelayGui);
    addAndMakeVisible(sDelayGui);
    
    fDelayTimeSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"fDelayTime", *fDelayGui.basicParams[0]);
    fDelayFeedbackSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "fDelayFeedback", *fDelayGui.basicParams[2]);
    fDelayWetSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "fDelayWet", *fDelayGui.basicParams[3]);
    fDelayRevAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "fDelayRev", fDelayGui.revBtn);
    fDelayPanAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "fDelayPan", fDelayGui.panSlider);
    fDelaySync = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "fDelaySync", fDelayGui.syncBtn);
    fDelaySyncTimes = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "fDelaySyncTimes", *fDelayGui.basicParams[1]);
    fDelayLP = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "fDelayLP", fDelayGui.lowPassSlider);
    fDelayHP = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "fDelayHP", fDelayGui.highPassSlider);

    sDelayTimeSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "sDelayTime", *sDelayGui.basicParams[0]);
    sDelayFeedbackSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "sDelayFeedback", *sDelayGui.basicParams[2]);
    sDelayWetSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "sDelayWet", *sDelayGui.basicParams[3]);
    sDelayRevAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "sDelayRev", sDelayGui.revBtn);
    sDelayPanAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "sDelayPan", sDelayGui.panSlider);
    sDelaySync = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "sDelaySync", sDelayGui.syncBtn);
    sDelaySyncTimes = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "sDelaySyncTimes", *sDelayGui.basicParams[1]);
    sDelayLP = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "sDelayLP", sDelayGui.lowPassSlider);
    sDelayHP = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "sDelayHP", sDelayGui.highPassSlider);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
}

PluginPrototypeAudioProcessorEditor::~PluginPrototypeAudioProcessorEditor()
{
}

//==============================================================================
void PluginPrototypeAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black);
    juce::Rectangle<float> allBounds = getBounds().toFloat();
    g.drawImage(bg,allBounds);

    juce::Rectangle<int> bounds = getBounds();
    fDelayGui.setBounds(bounds.removeFromLeft(getWidth()/2));
    sDelayGui.setBounds(bounds);

    g.drawImageWithin(logo, 175, 130, 70, 70, juce::RectanglePlacement::centred,false);
}

void PluginPrototypeAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
