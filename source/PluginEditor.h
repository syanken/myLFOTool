/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "FPLookAndFeels.h"

//==============================================================================
/**
*/
class LFODrawableScreen :public Component
{
public:
    LFODrawableScreen() {};
    LFODrawableScreen(std::vector<std::vector<float>> &points, std::vector<float> &waveToPaint) ;
    ~LFODrawableScreen() {};
    int height;
    int width;

    int drawareaheight;
    int drawareawidth;
    int drawareapadding;
    int mousecloseindex;

    void paint(juce::Graphics& g) override;
    float translateX(float x);
    float translateY(float y);
    float Xtranslate(float x);
    float Ytranslate(float y);
    void initscreen();
    std::vector<std::vector<float>> *points;
    //std::vector<std::vector<float>> midpoints;
    std::vector<float>* waveToPaint;

    void mouseMove(const MouseEvent& event)override;
    void mouseDrag(const MouseEvent& event)override;
    void mouseDoubleClick(const MouseEvent& event)override;
    std::vector<std::vector<std::vector<float>>> basepoints;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LFODrawableScreen);
};

class MyLFOToolAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer
{
public:
    MyLFOToolAudioProcessorEditor (MyLFOToolAudioProcessor&);
    ~MyLFOToolAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback()override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MyLFOToolAudioProcessor& audioProcessor;
    LFODrawableScreen LFOScreen;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyLFOToolAudioProcessorEditor)
};
