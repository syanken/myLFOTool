/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyLFOToolAudioProcessorEditor::MyLFOToolAudioProcessorEditor (MyLFOToolAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),LFOScreen(audioProcessor.points,audioProcessor.waveToPaint)
{
    setSize(800, 400);
	LFOScreen.setBounds(80, 0, LFOScreen.width, LFOScreen.height);
	addAndMakeVisible(LFOScreen);
}

MyLFOToolAudioProcessorEditor::~MyLFOToolAudioProcessorEditor()
{
}

//==============================================================================
void MyLFOToolAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::white);
	//g.setColour(Colours::red);
	//g.drawText(String(audioProcessor.timeInSamples), 0, 0, 80, 40, Justification::centred);
	//g.drawText(String(audioProcessor.timeInSeconds), 0, 40, 80, 40, Justification::centred);
	//g.drawText(String(audioProcessor.barCount), 0, 80, 80, 40, Justification::centred);
	//g.drawText(String(audioProcessor.barStart), 0, 120, 80, 40, Justification::centred);
	//g.drawText(String(audioProcessor.ppqPosition), 0, 160, 80, 40, Justification::centred);
	//g.drawText(String((int)audioProcessor.isplaying), 0,200, 80, 40, Justification::centred);
	//g.drawText(String(audioProcessor.waveToPaintIndex), 0,240, 80, 40, Justification::centred);
	//
	//repaint();
}

void MyLFOToolAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void MyLFOToolAudioProcessorEditor::timerCallback()
{
}


 LFODrawableScreen::LFODrawableScreen(std::vector<std::vector<float>> &points, std::vector<float>& waveToPaint)
{  
	this->points = &points; 
	this->waveToPaint = &waveToPaint;
	initscreen();
	height = 240;
	width = 400;
	drawareapadding = 5;
	drawareaheight = 230;
	drawareawidth = 390;
}

void LFODrawableScreen::paint(juce::Graphics& g)
{
	g.fillAll(Colours::yellow);
	Path wavepath;
	wavepath.startNewSubPath(0, height / 2);
	for (int i = 0; i < waveToPaint->size(); i++)
	{
		wavepath.lineTo(width * i * 1.0 / waveToPaint->size(), height * ((*waveToPaint)[i] + 0.5));
	}
	g.setColour(Colours::black);
	g.strokePath(wavepath, juce::PathStrokeType(1.0f));
	g.setColour(Colours::black);
	g.drawRect(0, 0, width, height);
	//g.drawLine(0, height / 2, width, height / 2);

	g.setColour(Colours::red);
	Path p;
	p.startNewSubPath(translateX((*points)[0][0]), translateY((*points)[0][1]));
	for (int i = 0; i < points->size(); i++)
	{
		float x = translateX((*points)[i][0]);
		float y = translateY((*points)[i][1]);
		//if (i > 0)
		//{
		//float bx = translateX(midpoints[i-1][0]);
		//float by = translateY(midpoints[i-1][1]);
		//p.quadraticTo(bx, by, x, y);
		//g.setColour(Colours::red);
		//g.drawEllipse(bx - 5, by - 5, 10.0f, 10.0f,1.0f);
		//}
		p.lineTo(x, y);
		if (i == mousecloseindex)
		{
			g.setColour(Colours::orange);
			g.fillEllipse(x - 8, y - 8, 16.0f, 16.0f);
		}

		g.setColour(Colours::red);
		g.fillEllipse(x - 4, y - 4, 8.0f, 8.0f);
	}
	g.strokePath(p, juce::PathStrokeType(2.0f));
	repaint();
}

float LFODrawableScreen::translateX(float x)
{
	return drawareapadding + x * drawareawidth;
}

float LFODrawableScreen::translateY(float y)
{
	return (-y + 1) * drawareaheight + drawareapadding;
}

float LFODrawableScreen::Xtranslate(float x)
{
	return (x - drawareapadding) / drawareawidth;
}

float LFODrawableScreen::Ytranslate(float y)
{
	return -((y - drawareapadding) / drawareaheight - 1);
}

void LFODrawableScreen::initscreen()
{
	//midpoints = {};
	//for (int i = 1; i < points.size(); i++)
	//{
	//	midpoints.push_back({ (points[i][0] + points[i - 1][0]) / 2, (points[i][1] + points[i - 1][1]) / 2 });
	//}
	mousecloseindex = -1;
}

void LFODrawableScreen::mouseMove(const MouseEvent& event)
{
	mousecloseindex = -1;
	int  mouseposX = event.getPosition().getX();
	int  mouseposY = event.getPosition().getY();
	for (int i = 0; i < points->size(); i++)
	{
		float dis = (mouseposX - translateX((*points)[i][0])) * (mouseposX - translateX((*points)[i][0])) + (mouseposY - translateY((*points)[i][1])) * (mouseposY - translateY((*points)[i][1]));
		if (dis < 128)
		{
			mousecloseindex = i;
			break;
		}
	}
	if (mousecloseindex >= 0)repaint();
}

void LFODrawableScreen::mouseDrag(const MouseEvent& event)
{
	if (mousecloseindex > 0 && mousecloseindex < points->size() - 1)
	{
		float x = Xtranslate(event.getPosition().getX());
		float y = Ytranslate(event.getPosition().getY());
		x = x > (*points)[mousecloseindex - 1][0] ? x : (*points)[mousecloseindex - 1][0];
		x = x < (*points)[mousecloseindex + 1][0] ? x : (*points)[mousecloseindex + 1][0];
		y = y > 0 ? y : 0;
		y = y < 1 ? y : 1;
		(*points)[mousecloseindex][0] = x;
		(*points)[mousecloseindex][1] = y;
	}
	else if (mousecloseindex == 0 || mousecloseindex == points->size() - 1)
	{
		float y = Ytranslate(event.getPosition().getY());
		y = y > 0 ? y : 0;
		y = y < 1 ? y : 1;
		(*points)[0][1] = y;
		(*points)[points->size() - 1][1] = y;
	}
	//for (int i = 0; i < midpoints.size(); i++)
	//{
	//	midpoints[i][0] = (points[i][0] + points[i + 1][0]) / 2;
	//}
	repaint();
}
void LFODrawableScreen::mouseDoubleClick(const MouseEvent& event)
{
	int  mouseposX = event.getPosition().getX();
	int  mouseposY = event.getPosition().getY();
	for (int i = 1; i < points->size() - 1; i++)
	{
		float dis = (mouseposX - translateX((*points)[i][0])) * (mouseposX - translateX((*points)[i][0])) + (mouseposY - translateY((*points)[i][1])) * (mouseposY - translateY((*points)[i][1]));
		if (dis < 50)
		{
			points->erase(points->begin() + i);
			//midpoints.clear();
			//for (int i = 1; i < points.size(); i++)
			//{
			//	midpoints.push_back({ (points[i][0] + points[i - 1][0]) / 2, (points[i][1] + points[i - 1][1]) / 2 });
			//}
			//repaint();
			return;
		}
	}
	if (points->size() > 32)return;
	std::vector<float> v = { Xtranslate(event.getPosition().getX()),Ytranslate(event.getPosition().getY()) };	for (int i = 1; i < points->size(); i++)
		if (v[0] < (*points)[i][0])
		{
			points->insert(points->begin() + i, v);
			//midpoints.clear();
			//for (int i = 1; i < points.size(); i++)
			//{
			//	midpoints.push_back({ (points[i][0] + points[i - 1][0]) / 2, (points[i][1] + points[i - 1][1]) / 2 });
			//}
			return;
		}
}