/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//==============================================================================
/**
*/
static double bpm;
static std::vector<std::vector<int>> timeTable{ {1,1},{1,2},{3,4},{1,3},{1,4},{3,8},{1,6} };

class Delay {
public:
	enum class delayAlgorithm{kNormal,kPingPong};
	enum class delayUpdateType{ kLeftAndRight, kLeftPlusRatio};

	void initialize(int numInputChannels,int sampleRate, int samplesPerBlock);
	float LinearInterpolation(float v1, float v2, float fraction);
	float CubicHermiteInterpolation(float A, float B, float C, float D, float t);
	int readPosishMaker(int delayInSamples);
	float readBuffer(int channel);
	void writeToBuffer(juce::AudioBuffer<float>* mainBuffer);
	float getDelayTime();
	void setDelayTime(float newTime);
	void setWetness(float newWetness);
	void setFeedback(float newFeedback);
	void setReverse(const bool& val);
	juce::AudioBuffer<float>* getDelayBuffer();
	std::vector<int> currentDelayedSample{ 0, 0 };
	void setPan(const float& panPosistion);
	void setSync(const bool& val);
	bool getSync();
	void setSyncTimeDivision(const float& time);
	using Filter = juce::dsp::IIR::Filter<float>;
	using CutFilter = juce::dsp::ProcessorChain<Filter>;
	using MonoChain = juce::dsp::ProcessorChain<CutFilter, CutFilter>;
	MonoChain leftChain,rightChain;
	juce::IIRFilter lowPassFilter;
	juce::IIRFilter highPassFilter;
	juce::IIRCoefficients lowPassCoeffs;
	juce::IIRCoefficients highPassCoeffs;
	juce::SmoothedValue<float> delayTimeSmoothed;

private:
	juce::AudioBuffer<float> buffer;
	bool isReverse{ false };
	int writePosish{ 0 };
	int readPosish{ 0 };
	float delayTime{ 200 };
	int wrapMask{ 0 };
	int delayBuffLength{ 0 };
	float feedback{ 0.7 };
	float delayInSamples{ 0 };
	float currentSampleRate{ 44100 };
	float wet{ 0.5 };
	std::vector<float> pans{ 1.0f,1.0f };
	bool sync{ false };
	float syncTimeDivision;
};

class PluginPrototypeAudioProcessor : public juce::AudioProcessor
{
public:
	//==============================================================================
	PluginPrototypeAudioProcessor();
	~PluginPrototypeAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;
	void updateParams();
	
	juce::AudioBuffer<float>* getBuffer(const int& id);
	juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	juce::AudioProcessorValueTreeState apvts{ *this,nullptr,"Params",createParameterLayout() };
	std::vector<int>* currentSample(const int& id);
	
	juce::AudioPlayHead::CurrentPositionInfo currentPosish;
	bool sync{ false };

	Delay fDelay;
	Delay sDelay;

private:
	//juce::AudioBuffer<float> circBuff;
	//int writePosish{ 0 };
	//int readPosish{ 0 };
	//float delayTime{ 300.f };
	//int wrapMask{ 0 };
	//int delayBuffLength{ 0 };
	juce::Array<float> noiseBuffer;
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginPrototypeAudioProcessor)
};
