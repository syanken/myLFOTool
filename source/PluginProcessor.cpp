/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#define PI 3.14159265358979
//==============================================================================
//problems:
// while bpm changing,the beats incurrect
// repaint() area
// waveToPaint length fit to bpm
// bar change function
// shuzujishumoshi
//==============================================================================
MyLFOToolAudioProcessor::MyLFOToolAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	), waveToPaint(6000, 0)
#endif
{
	points = { {0,0},{0.5,0},{0.5,1}, {1,1} ,{1,0} };
}

MyLFOToolAudioProcessor::~MyLFOToolAudioProcessor()
{
}

//==============================================================================
const juce::String MyLFOToolAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool MyLFOToolAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool MyLFOToolAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool MyLFOToolAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double MyLFOToolAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int MyLFOToolAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int MyLFOToolAudioProcessor::getCurrentProgram()
{
	return 0;
}

void MyLFOToolAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String MyLFOToolAudioProcessor::getProgramName(int index)
{
	return {};
}

void MyLFOToolAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void MyLFOToolAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
	if (getPlayHead()->getPosition()->getBpm())
	{	if (getPlayHead()->getPosition()->getPpqPosition())
	ppqPosition = *getPlayHead()->getPosition()->getPpqPosition();
	if (auto bpmFromHost = *getPlayHead()->getPosition()->getBpm())
		bpm = bpmFromHost;
	waveToPaint.resize(48000 * 120 / bpm / 8);
	waveToPaintIndex = (ppqPosition - (int)ppqPosition) * 48000 * 120 / bpm / 8;
	isplaying = false;
	}
	//DBG("sampleRate:"<<sampleRate<<"samplesPerBlock"<< samplesPerBlock<<"waveToPaint.size" << waveToPaint.size());
}

void MyLFOToolAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MyLFOToolAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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

void MyLFOToolAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...
	// Make sure to reset the state if your inner loop is processing
	// the samples and the outer loop is handling the channels.
	// Alternatively, you can process the samples with the channels
	// interleaved by keeping the same state.
	for (auto sample = 0; sample < buffer.getNumSamples(); sample++)
	{
		ppqPosition = *getPlayHead()->getPosition()->getPpqPosition();
		if (getPlayHead()->getPosition()->getIsPlaying())
		{
			if (!isplaying)
			{
				isplaying = true;
				waveToPaintIndex = (ppqPosition - (int)ppqPosition) * 6000 * 120 / bpm ;
			}
		}
		else
		{
			isplaying = false;
		}
		if (auto bpmFromHost = *getPlayHead()->getPosition()->getBpm())
		{
			if (bpm != bpmFromHost)
			{
				waveToPaint.resize(48000 * 120 / bpmFromHost / 8);
				waveToPaintIndex = (ppqPosition - (int)ppqPosition) * 6000 * 120 / bpmFromHost;
				bpm = bpmFromHost;
			}			
		}
		if (waveToPaintIndex >= waveToPaint.size()|| waveToPaintIndex<0)
		{
			waveToPaintIndex = 0;
			//if (getPlayHead()->getPosition()->getBpm())
			//{
			//	if (getPlayHead()->getPosition()->getPpqPosition())
			//		ppqPosition = *getPlayHead()->getPosition()->getPpqPosition();								
			//	if (getPlayHead()->getPosition()->getTimeInSamples())
			//		timeInSamples = *getPlayHead()->getPosition()->getTimeInSamples();
			//	if (getPlayHead()->getPosition()->getTimeInSeconds())
			//		timeInSeconds = *getPlayHead()->getPosition()->getTimeInSeconds();
			//	if (getPlayHead()->getPosition()->getBarCount())
			//		barCount = *getPlayHead()->getPosition()->getBarCount();
			//	if (getPlayHead()->getPosition()->getPpqPositionOfLastBarStart())
			//		barStart = *getPlayHead()->getPosition()->getPpqPositionOfLastBarStart();								
			//}
		}
		auto* inBufferleft = buffer.getReadPointer(0);
		auto* inBufferright = buffer.getReadPointer(1);
		auto* outBufferleft = buffer.getWritePointer(0);
		auto* outBufferright = buffer.getWritePointer(1);
		int index = 1;
		while (waveToPaintIndex * 1.0f / waveToPaint.size() > points[index][0])
		{
			index += 1;
		}
		float emp = myLinearInterpolation(points[index - 1][0], points[index][0], points[index - 1][1], points[index][1], waveToPaintIndex * 1.0f / waveToPaint.size());
		outBufferleft[sample] = inBufferleft[sample] * emp;
		outBufferright[sample] = inBufferright[sample] * emp;
		//outBuffer[sample] = std::sin(sample * 0.01f* s3.getValue())/200*level*emp ;
		//outBuffer[sample] = (random.nextFloat()-0.5) /200*level*emp ;
		//outBuffer[sample] = std::sin(sample *  PI/ 120) / 200 * perSampleNum * emp;
		//outBuffer[sample] = std::sin(sample * M_PI / 240)>0?  level * emp/ 200 :-level * emp / 200;
		if ((sample + 1) % 8 == 0)
		{
			float ave = 0;
			for (int i = 0; i < 8; i++)
			{
				ave += outBufferleft[sample - i] * 1.0 / 8 / 2;
			}
			waveToPaint[waveToPaintIndex] = ave;
			waveToPaintIndex++;
		}
		//waveToPaint[buffer.getNumSamples() * counttime + sample ] = outBuffer[sample] * 0.5;
	}
}

//==============================================================================
bool MyLFOToolAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MyLFOToolAudioProcessor::createEditor()
{
	return new MyLFOToolAudioProcessorEditor(*this);
}

//==============================================================================
void MyLFOToolAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void MyLFOToolAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

float MyLFOToolAudioProcessor::myLinearInterpolation(float x1, float x2, float y1, float y2, float x0)
{
	if (x1 == x0 || y1 == y2)return y1;
	float x2_x1 = x2 - x1;
	float y2_y1 = y2 - y1;
	float x0_x1 = x0 - x1;
	return y1 + y2_y1 * x0_x1 / x2_x1;
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new MyLFOToolAudioProcessor();
}
