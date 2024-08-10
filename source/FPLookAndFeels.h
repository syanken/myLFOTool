/*
  ==============================================================================

	FPLookAndFeels.h
	Created: 22 Jul 2024 8:22:55pm
	Author:  pyc

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
using namespace juce;
class FPSlider :public Slider
{
public:
	FPSlider() {
		setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
		setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
		
		//setPopupDisplayEnabled(true, true, this->getParentComponent(), 2000);		
	};
	~FPSlider() { setLookAndFeel(nullptr); };
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FPSlider)
};

class MyButton :public TextButton
{

public:
	MyButton()
	{
		buttonstate = false;
	}
	~MyButton()
	{
		setLookAndFeel(nullptr);
	}
	void paint(Graphics& g)
	{
		if (buttonstate)
		{
			g.fillAll(Colours::black);
			g.setColour(Colours::white);
			g.drawText(getTitle(), getLocalBounds(), Justification::centred);
			
		}
		else
		{
			g.fillAll(Colours::white);
			g.setColour(Colours::black);
			g.drawText(getTitle(),getLocalBounds(),Justification::centred);
		}		
		g.setColour(Colours::black);
		g.drawRect(getLocalBounds());
		DBG (getTitle()+ "paint");
	}
	bool buttonstate;	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyButton);
};

class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
	OtherLookAndFeel()
	{
		setColour(juce::Slider::thumbColourId, juce::Colours::red);
	}
	void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& s) override
	{
		auto radius = (float)jmin(width / 2, height / 2) - 10.0f;
		auto centreX = (float)x + (float)width * 0.5f;
		auto centreY = (float)y + (float)height * 0.5f;
		auto rx = centreX - radius;
		auto ry = centreY - radius;
		auto rw = radius * 2.0f;
		auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

		g.setColour(juce::Colours::red);
		g.drawEllipse(rx, ry, rw, rw, 4.0f);
		g.drawText(String(s.getValue()),s.getHeight()/2-25, s.getWidth() / 2 - 25, 50, 50,Justification::centred);
		juce::Path p;
		auto pointerLength = radius * 0.5f;
		auto pointerThickness = 4.0f;
		p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
		p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
		g.fillPath(p);

	}
	void drawComboBox(Graphics& g,
		int 	width,
		int 	height,
		bool 	isButtonDown,
		int 	buttonX,
		int 	buttonY,
		int 	buttonW,
		int 	buttonH,
		ComboBox& combobox
	) {
		g.setColour(Colours::red);
		g.drawRect(combobox.getLocalBounds());
		g.drawText(combobox.getText(), combobox.getLocalBounds(), Justification::centred);

	}
};
