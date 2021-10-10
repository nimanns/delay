/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


static juce::Array<float> getSimpleNoise(int numSamples)
{
	juce::Random r = juce::Random::getSystemRandom();
	juce::Array<float> noise;

	for (int s = 0; s < numSamples; s++)
	{
		noise.add((r.nextFloat() - .5) * 2);
	}

	return noise;
}


//==============================================================================
PluginPrototypeAudioProcessor::PluginPrototypeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)

#endif

{
}

PluginPrototypeAudioProcessor::~PluginPrototypeAudioProcessor()
{
}

//==============================================================================
const juce::String PluginPrototypeAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool PluginPrototypeAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool PluginPrototypeAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool PluginPrototypeAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double PluginPrototypeAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int PluginPrototypeAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int PluginPrototypeAudioProcessor::getCurrentProgram()
{
	return 0;
}

void PluginPrototypeAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String PluginPrototypeAudioProcessor::getProgramName(int index)
{
	return {};
}

void PluginPrototypeAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void PluginPrototypeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	const int numInputChannels = getTotalNumInputChannels();
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
	fDelay.initialize(numInputChannels, sampleRate, samplesPerBlock);
	sDelay.initialize(numInputChannels, sampleRate, samplesPerBlock);
}

void PluginPrototypeAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginPrototypeAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void PluginPrototypeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();
	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
		buffer.clear(i, 0, buffer.getNumSamples());
	}
	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...
	// Make sure to reset the state if your inner loop is processing
	// the samples and the outer loop is handling the channels.
	// Alternatively, you can process the samples with the channels
	// interleaved by keeping the same state.

	updateParams();

	if (auto* ph = getPlayHead())
	{
		ph->getCurrentPosition(currentPosish);
		bpm = currentPosish.bpm;
	}
	else
	{
		bpm = 120.0;
	}

	float* channelData = buffer.getWritePointer(0);
	float* channelDataR = buffer.getWritePointer(1);
	const int bufferLength = buffer.getNumSamples();
	const float* mainReadPointer = buffer.getReadPointer(0);
	const float* mainReadPointerR = buffer.getReadPointer(1);

	fDelay.writeToBuffer(&buffer);
	sDelay.writeToBuffer(&buffer);
}

//==============================================================================
bool PluginPrototypeAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginPrototypeAudioProcessor::createEditor()
{
	return new PluginPrototypeAudioProcessorEditor(*this);
}

//==============================================================================
void PluginPrototypeAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
	juce::MemoryOutputStream mos(destData, true);
	apvts.state.writeToStream(mos);
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginPrototypeAudioProcessor::createParameterLayout() {
	juce::AudioProcessorValueTreeState::ParameterLayout layout;
	layout.add(std::make_unique<juce::AudioParameterFloat>("fDelayTime", "FDelayTime", 200.0f, 2000.0f, 350.0f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("fDelayFeedback", "FDelayFeedback", juce::NormalisableRange<float>(0.1f, 2.0f, 0.1), 0.5f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("fDelayWet", "FDelayWet", juce::NormalisableRange<float>(0.0f, 1.0f, 0.1), 0.5f));
	layout.add(std::make_unique<juce::AudioParameterBool>("fDelayRev", "FDelayRev", false, "Reverse"));
	layout.add(std::make_unique<juce::AudioParameterFloat>("fDelayPan", "FDelayPan", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.0f), 0.0f));
	layout.add(std::make_unique<juce::AudioParameterBool>("fDelaySync", "FDelaySync", false, "Sync"));
	juce::StringArray choices; 
	choices.addArray({ "1/4","1/8","1/8d","1/8t","1/16","1/16d","1/16t" });
	layout.add(std::make_unique<juce::AudioParameterChoice>("fDelaySyncTimes", "FDelaySyncTimes", choices, 1));
	layout.add(std::make_unique<juce::AudioParameterFloat>("fDelayLP", "FDelayLP", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f,0.17f), 20000.0f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("fDelayHP", "FDelayHP", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f,0.25f), 20.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>("sDelayTime", "SDelayTime", 200.0f, 2000.0f, 350.0f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("sDelayFeedback", "SDelayFeedback", juce::NormalisableRange<float>(0.1f, 2.0f, 0.1), 0.5f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("sDelayWet", "SDelayWet", juce::NormalisableRange<float>(0.0f, 1.0f, 0.1), 0.5f));
	layout.add(std::make_unique<juce::AudioParameterBool>("sDelayRev", "SDelayRev", false, "Reverse"));
	layout.add(std::make_unique<juce::AudioParameterFloat>("sDelayPan", "SDelayPan", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.0f), 0.0f));
	layout.add(std::make_unique<juce::AudioParameterBool>("sDelaySync", "SDelaySync", false, "Sync"));
	layout.add(std::make_unique<juce::AudioParameterChoice>("sDelaySyncTimes", "SDelaySyncTimes", choices, 1));
	layout.add(std::make_unique<juce::AudioParameterFloat>("sDelayLP", "SDelayLP", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.17f), 20000.0f));
	layout.add(std::make_unique<juce::AudioParameterFloat>("sDelayHP", "SDelayHP", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f), 20.0f));

	layout.add(std::make_unique<juce::AudioParameterFloat>("drySignal", "DrySignal", juce::NormalisableRange<float>(0.0f, 1.0f, 0.1), 0.5f));

	return layout;
}

void PluginPrototypeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.

	auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
	if (tree.isValid())
	{
		apvts.replaceState(tree);
		updateParams();
	}
}

void PluginPrototypeAudioProcessor::updateParams()
{

	if (!fDelay.getSync())
	{
		fDelay.delayTimeSmoothed.setTargetValue(apvts.getRawParameterValue("fDelayTime")->load());
	}
	fDelay.setFeedback(apvts.getRawParameterValue("fDelayFeedback")->load());
	fDelay.setWetness(apvts.getRawParameterValue("fDelayWet")->load());
	fDelay.setPan(apvts.getRawParameterValue("fDelayPan")->load());
	fDelay.setReverse(apvts.getRawParameterValue("fDelayRev")->load());
	fDelay.setSync(apvts.getRawParameterValue("fDelaySync")->load());
	fDelay.setSyncTimeDivision(apvts.getRawParameterValue("fDelaySyncTimes")->load());
	fDelay.lowPassCoeffs = juce::IIRCoefficients::makeLowPass(getSampleRate(), apvts.getRawParameterValue("fDelayLP")->load());
	fDelay.highPassCoeffs = juce::IIRCoefficients::makeHighPass(getSampleRate(), apvts.getRawParameterValue("fDelayHP")->load());

	if (!sDelay.getSync())
	{
		sDelay.delayTimeSmoothed.setTargetValue(apvts.getRawParameterValue("sDelayTime")->load());
	}
	sDelay.setFeedback(apvts.getRawParameterValue("sDelayFeedback")->load());
	sDelay.setWetness(apvts.getRawParameterValue("sDelayWet")->load());
	sDelay.setPan(apvts.getRawParameterValue("sDelayPan")->load());
	sDelay.setReverse(apvts.getRawParameterValue("sDelayRev")->load());
	sDelay.setSync(apvts.getRawParameterValue("sDelaySync")->load());
	sDelay.setSyncTimeDivision(apvts.getRawParameterValue("sDelaySyncTimes")->load());
	sDelay.lowPassCoeffs = juce::IIRCoefficients::makeLowPass(getSampleRate(), apvts.getRawParameterValue("sDelayLP")->load());
	sDelay.highPassCoeffs = juce::IIRCoefficients::makeHighPass(getSampleRate(), apvts.getRawParameterValue("sDelayHP")->load());
}

juce::AudioBuffer<float>* PluginPrototypeAudioProcessor::getBuffer(const int& id)
{
	switch (id)
	{
	case(0):
		return fDelay.getDelayBuffer();
		break;
	case(1):
		return sDelay.getDelayBuffer();
	default:
		return nullptr;
		break;
	}
}

std::vector<int>* PluginPrototypeAudioProcessor::currentSample(const int& id)
{
	switch (id)
	{
	case(0):
		return &fDelay.currentDelayedSample;

		break;
	case(1):
		return &sDelay.currentDelayedSample;

		break;
	default:
		return nullptr;
		break;
	}
}

void Delay::initialize(int numInputChannels, int sampleRate, int samplesPerBlock)
{
	int bufferLength = (unsigned int)(2000 * (sampleRate / 1000))
		+ 1;
	const int size = (unsigned int)(pow(2, ceil(log(bufferLength) /
		log(2))));
	wrapMask = size - 1;
	delayInSamples = delayTime * sampleRate / 1000.0f;
	writePosish = 0;
	currentSampleRate = sampleRate;
	buffer.setSize(numInputChannels, size);
	buffer.clear();
	lowPassCoeffs = juce::IIRCoefficients::makeLowPass(sampleRate, 20000);
	highPassCoeffs = juce::IIRCoefficients::makeHighPass(sampleRate, 20);
	lowPassFilter.setCoefficients(lowPassCoeffs);
	highPassFilter.setCoefficients(highPassCoeffs);
	delayTimeSmoothed.reset(sampleRate);
}

void Delay::writeToBuffer(juce::AudioBuffer<float>* mainBuffer)
{
	if (sync)
	{
		auto newTime = (60000.0f / bpm);
		newTime *= timeTable[syncTimeDivision][0];
		newTime /= timeTable[syncTimeDivision][1];
		delayTimeSmoothed.setTargetValue(newTime);
	}

	for (int i = 0; i < mainBuffer->getNumSamples(); ++i) {
		lowPassFilter.setCoefficients(lowPassCoeffs);
		highPassFilter.setCoefficients(highPassCoeffs);
		setDelayTime(delayTimeSmoothed.getNextValue());
		delayInSamples = delayTime * currentSampleRate / 1000.0f;

		for (int channel = 0; channel < mainBuffer->getNumChannels(); ++channel)
		{
			buffer.getWritePointer(channel)[writePosish] = (this->readBuffer(channel)) + mainBuffer->getReadPointer(channel)[i];
			mainBuffer->getWritePointer(channel)[i] = (wet*(pans[channel] * highPassFilter.processSingleSampleRaw(lowPassFilter.processSingleSampleRaw(this->readBuffer(channel))))) + mainBuffer->getReadPointer(channel)[i];
		}
		writePosish++;
		writePosish &= wrapMask;
	}
}

float Delay::readBuffer(int channel)
{

	if (!isReverse)
	{

		currentDelayedSample[0] = (writePosish - int(delayInSamples)) & wrapMask;
		currentDelayedSample[1] = (writePosish - int(delayInSamples + 1)) & wrapMask;
		
		auto prevPoint = (writePosish - int(delayInSamples + 2)) & wrapMask;
		auto nextPoint = (writePosish - int(delayInSamples - 1)) & wrapMask;

		float frac = delayInSamples - int(delayInSamples);

		return ((feedback * CubicHermiteInterpolation(buffer.getReadPointer(channel)[prevPoint], buffer.getReadPointer(channel)[currentDelayedSample[1]], buffer.getReadPointer(channel)[currentDelayedSample[0]], buffer.getReadPointer(channel)[nextPoint], frac)));

	}
	else {


		currentDelayedSample[0] = (int(delayInSamples) - writePosish) & wrapMask;
		currentDelayedSample[1] = (int(delayInSamples + 1) - writePosish) & wrapMask;

		auto prevPoint = (int(delayInSamples + 2) - writePosish) & wrapMask;
		auto nextPoint = (int(delayInSamples - 1) - writePosish) & wrapMask;

		float frac = delayInSamples - int(delayInSamples);

		return ((feedback * CubicHermiteInterpolation(buffer.getReadPointer(channel)[prevPoint], buffer.getReadPointer(channel)[currentDelayedSample[1]], buffer.getReadPointer(channel)[currentDelayedSample[0]], buffer.getReadPointer(channel)[nextPoint], frac)));
	}
}

float Delay::LinearInterpolation(float v1, float v2, float fraction)
{
	if (fraction >= 1.0)
		return v2;
	return fraction * v2 + (1.0 - fraction) * v1;
}

float Delay::CubicHermiteInterpolation(float A, float B, float C, float D, float t)
{
	float a = -A / 2.0f + (3.0f * B) / 2.0f - (3.0f * C) / 2.0f + D / 2.0f;
	float b = A - (5.0f * B) / 2.0f + 2.0f * C - D / 2.0f;
	float c = -A / 2.0f + C / 2.0f;
	float d = B;

	return a * t * t * t + b * t * t + c * t + d;
}

int Delay::readPosishMaker(int delayInSamples)
{
	int readPosish = writePosish - delayInSamples;
	readPosish &= wrapMask;
	return readPosish;
}

float Delay::getDelayTime()
{
	return delayTime;
}

void Delay::setDelayTime(float newTime)
{
	delayTime = newTime;
}

void Delay::setWetness(float newWetness)
{
	wet = newWetness;
}
void Delay::setFeedback(float newFeedback)
{
	feedback = newFeedback;
}

void Delay::setReverse(const bool& val)
{
	isReverse = val;
}

void Delay::setSync(const bool& val)
{
	sync = val;
}

bool Delay::getSync()
{
	return sync;
}

void Delay::setSyncTimeDivision(const float& time)
{
	syncTimeDivision = int(time);
}

juce::AudioBuffer<float>* Delay::getDelayBuffer()
{
	return &buffer;
}

void Delay::setPan(const float& panPosistion)
{
	if (panPosistion < 0)
	{
		pans[1] = 1 - abs(panPosistion);
	}
	else if (panPosistion > 0)
	{
		pans[0] = 1 - panPosistion;
	}
	else
	{
		pans = { 1.0f,1.0f };
	}
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new PluginPrototypeAudioProcessor();
}
