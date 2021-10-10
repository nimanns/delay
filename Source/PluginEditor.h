/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "DelayGuiComp.h"
//==============================================================================
/**
*/

class PluginPrototypeAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
	PluginPrototypeAudioProcessorEditor(PluginPrototypeAudioProcessor&);
	~PluginPrototypeAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;

	std::vector<juce::String> labels = { "ms","%","dB" };
	std::vector<juce::String> paramNames = { "Time","Feedback","Wet" };
	juce::Image logo = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);
	juce::DropShadow dpShadow;

private:
	juce::Image bg = juce::ImageCache::getFromMemory(BinaryData::bg3_png, BinaryData::bg3_pngSize);

	DelayGuiComp fDelayGui;
	DelayGuiComp sDelayGui;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fDelayTimeSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fDelayFeedbackSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fDelayWetSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fDelayRevAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fDelayPanAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fDelaySync;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fDelaySyncTimes;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fDelayLP;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fDelayHP;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sDelayTimeSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sDelayFeedbackSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sDelayWetSliderAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> sDelayRevAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sDelayPanAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> sDelaySync;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sDelaySyncTimes;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sDelayLP;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sDelayHP;

	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	PluginPrototypeAudioProcessor& audioProcessor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginPrototypeAudioProcessorEditor)
};
