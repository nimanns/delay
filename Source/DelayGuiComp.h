/*
  ==============================================================================

	DelayGuiComp.h
	Created: 3 Oct 2021 12:05:07pm
	Author:  Nimanns

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//==============================================================================
/*
*/


class Visualizer : public juce::Component, public juce::Timer
{
public:

	Visualizer() :mainBuffer(nullptr), sample(0)
	{
		setSize(200, 50);
		setBounds(20, 390, 170, 50);
	}

	~Visualizer()
	{

	}

	void timerCallback()
	{
		perform();
	}

	void paint(juce::Graphics& g)
	{
		g.fillAll(juce::Colour::fromRGB(186, 186, 186));
		g.setColour(juce::Colour::fromRGB(46, 46, 46));
		g.strokePath(p, juce::PathStrokeType(1.5f));
		g.setColour(juce::Colour::fromRGB(46, 46, 46));
		g.drawRect(0.f,0.f,170.f,50.f,2.5f);
	}

	void perform()
	{
		if (x == 0) p.startNewSubPath(0, 25);

		float thisPoint = juce::jmap(((mainBuffer->getReadPointer(0)[sample->at(0)] + mainBuffer->getReadPointer(1)[sample->at(1)]) / 2.f) * 10.f, -1.0f, 1.0f, 0.f, 50.f);
		thisPoint = juce::jlimit<float>(5, 45, thisPoint);
		p.lineTo(float(x), thisPoint);
		repaint();
		x++;
		if (x == 170)
		{
			x = 0;
			p.clear();
		}

	}

	void writeFromBuffer(juce::AudioBuffer<float>* audioBuffer, std::vector<int>* indeces)
	{
		mainBuffer = audioBuffer;
		sample = indeces;
		startTimer(70);
	}
	juce::AudioBuffer<float>* mainBuffer;
	std::vector<int>* sample;
	float prevPoint{ 25 };
	int j = 0;
	juce::Path p;
	int x = 0;

private:

};

static const float mix(const float& a, const float& b, const float& p)
{
	return a + p * (b - a);
}

class CustomLF1 : public juce::LookAndFeel_V4
{
public:
	juce::String valType;

	void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override {
		auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
		auto centreX = (float)x + (float)width * 0.5f;
		auto centreY = (float)y + (float)height * 0.5f;
		auto rx = centreX - radius;
		auto ry = centreY - radius;
		auto rw = radius * 2.0f;
		auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
		if (slider.getName() == "FSyncTimes" || slider.getName() == "SSyncTimes")
		{
			valType = syncTimeLabels[int(slider.getValue())];
		}
		else if (slider.getName() == "FLP" || slider.getName() == "SLP" || slider.getName() == "FHP" || slider.getName() == "SHP")
		{
			if (slider.getValue() >= 1000.f)
			{
				valType = juce::String(int(slider.getValue() / 1000))+ 'k';
			}
			else
			{
				valType = juce::String(slider.getValue());
			}
		}
		else
		{
			valType = juce::String(slider.getValue());
		}

		g.setColour(juce::Colours::ghostwhite);
		g.drawText(valType, x, y, width, height, juce::Justification::centred, false);
		g.drawText(slider.getTitle(), x, height/3, width, height, juce::Justification::centred, false);
		juce::Path initialArc;
		initialArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
		g.setColour(juce::Colours::black);
		g.strokePath(initialArc, juce::PathStrokeType(6.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
		g.setColour(juce::Colours::ghostwhite);
		g.strokePath(initialArc, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

		juce::Path p;
		auto pointerLength = radius * 0.53f;
		auto pointerThickness = 2.0f;
		p.addRectangle(-pointerThickness * 1.5f, -radius, pointerThickness, pointerLength);
		p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
		g.setColour(juce::Colours::black);
		g.fillPath(p);

		juce::Path backgroundArc;
		backgroundArc.addCentredArc(centreX,
			centreY,
			radius,
			radius,
			0.0f,
			rotaryStartAngle,
			angle,
			true);

		g.setColour(juce::Colours::black);
		g.strokePath(backgroundArc, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

	};

	void drawToggleButton(juce::Graphics& g, juce::ToggleButton& btn,
		bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
	{

		if (btn.getState() == juce::ToggleButton::ButtonState::buttonNormal)
		{
			g.drawImage(toggle, 0, 0, 40, 40, 0, 0, 40, 40);
		}
		if (btn.getState() == juce::ToggleButton::ButtonState::buttonOver)
		{
			g.drawImage(toggle, 0, 0, 40, 40, 0, 0, 40, 40);

		}
		if (btn.getState() == juce::ToggleButton::ButtonState::buttonDown)
		{
			g.drawImage(toggle, 0, 0, 40, 40, 0, 40, 40, 40);

		}

		if (btn.getToggleState())
		{
			g.drawImage(toggle, 0, 0, 40, 40, 0, 80, 40, 40);

		}
	}

private:
	juce::StringArray syncTimeLabels{ "1/4","1/8","1/8d","1/8t","1/16","1/16d","1/16t" };
	juce::Image toggle = juce::ImageCache::getFromMemory(BinaryData::toggle_png, BinaryData::toggle_pngSize);

};

class CustomLF3 : public juce::LookAndFeel_V4
{
public:
	juce::String label;

	void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override {
		auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
		auto centreX = (float)x + (float)width * 0.5f;
		auto centreY = (float)y + (float)height * 0.5f;
		auto rx = centreX - radius;
		auto ry = centreY - radius;
		auto rw = radius * 2.0f;
		auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
		auto middle = mix(rotaryStartAngle, rotaryEndAngle, .5f);
		auto rotation = mix(rotaryStartAngle, rotaryEndAngle, sliderPos);
		auto val = slider.getValue();
		if (val < 0)
		{
			label = juce::String(int(slider.getValue() * 100)) + "% L";
		}
		else if (val > 0)
		{
			label = juce::String(int(slider.getValue() * 100)) + "% R";
		}
		else if (val == 0)
		{
			label = "Centered";
		}

		g.setColour(juce::Colours::ghostwhite);
		g.drawText(label, x, y, width, height, juce::Justification::centred, false);
		g.setColour(juce::Colours::black);
		g.drawEllipse(rx, ry, rw, rw, 6.0f);
		g.setColour(juce::Colours::ghostwhite);
		g.drawEllipse(rx, ry, rw, rw, 2.5f);
		juce::Path p;
		auto pointerLength = radius * 0.53f;
		auto pointerThickness = 2.0f;
		p.addRectangle(-pointerThickness * 1.5f, -radius, pointerThickness, pointerLength);
		p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
		g.setColour(juce::Colours::black);
		g.fillPath(p);

		juce::Path backgroundArc;
		backgroundArc.addCentredArc(centreX,
			centreY,
			radius,
			radius,
			0.0f,
			middle,
			rotation,
			true);

		g.setColour(juce::Colours::black);
		g.strokePath(backgroundArc, juce::PathStrokeType(2.5, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

	};

};

class DelayGuiComp : public juce::Component
{
public:
	DelayGuiComp(const juce::String& id)
	{
		setSize(210, 620);
		basicParams.push_back(std::make_unique<juce::Slider>());
		basicParams.push_back(std::make_unique<juce::Slider>());
		basicParams.push_back(std::make_unique<juce::Slider>());
		basicParams.push_back(std::make_unique<juce::Slider>());

		for (int i = 0; i < basicParams.size(); ++i) {
			basicParams[i]->setSliderStyle(juce::Slider::RotaryVerticalDrag);
			basicParams[i]->setLookAndFeel(&customLF1);
			basicParams[i]->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
			basicParams[i]->setName(id + paramNames[i]);
			basicParams[i]->setTitle(titles[i]);
			addAndMakeVisible(*basicParams[i]);
		}

		panSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
		panSlider.setLookAndFeel(&customLF3);
		panSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		panSlider.setName(juce::String(id + "Pan"));
		revBtn.setLookAndFeel(&customLF1);
		syncBtn.setLookAndFeel(&customLF1);

		addAndMakeVisible(revBtn);
		addAndMakeVisible(visualizer);
		addAndMakeVisible(panSlider);
		addAndMakeVisible(syncBtn);

		lowPassSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
		lowPassSlider.setLookAndFeel(&customLF1);
		lowPassSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		lowPassSlider.setName(juce::String(id + "LP"));
		lowPassSlider.setTitle("LP");
		addAndMakeVisible(lowPassSlider);

		highPassSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
		highPassSlider.setLookAndFeel(&customLF1);
		highPassSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		highPassSlider.setName(juce::String(id + "HP"));
		highPassSlider.setTitle("HP");
		addAndMakeVisible(highPassSlider);

	}

	~DelayGuiComp() override
	{
		for (int i = 0; i < basicParams.size(); ++i) {
			basicParams[i]->setLookAndFeel(nullptr);
		}
		panSlider.setLookAndFeel(nullptr);
		revBtn.setLookAndFeel(nullptr);
		syncBtn.setLookAndFeel(nullptr);
		lowPassSlider.setLookAndFeel(nullptr);
		highPassSlider.setLookAndFeel(nullptr);
	}

	void paint(juce::Graphics& g) override
	{

		juce::Rectangle<int> normalRect1 = juce::Rectangle<int>(210, 620).reduced(10, 30).removeFromTop(getHeight() / 4);
		juce::Rectangle<int> normalRect2 = juce::Rectangle<int>(210, 620).reduced(10, 30).removeFromTop(getHeight() / 4);
		basicParams[0]->setBounds(normalRect1.removeFromTop(normalRect1.getHeight() / 2).removeFromLeft(normalRect1.getWidth() / 2));
		basicParams[2]->setBounds(normalRect2.removeFromTop(normalRect2.getHeight() / 2).removeFromRight(normalRect2.getWidth() / 2));
		basicParams[3]->setBounds(normalRect1);
		juce::Rectangle<int> syncTimesRect = juce::Rectangle<int>(210, 620).reduced(10, 30).removeFromTop(getHeight() / 4);
		basicParams[1]->setBounds(syncTimesRect.removeFromTop(syncTimesRect.getHeight() / 2).removeFromLeft(syncTimesRect.getWidth() / 2));
		if (sync)
		{
			basicParams[0]->setVisible(false);
			basicParams[1]->setVisible(true);
		}

		if (!sync)
		{
			basicParams[0]->setVisible(true);
			basicParams[1]->setVisible(false);
		}
		juce::Rectangle<int> syncBtnBounds = juce::Rectangle<int>(210, 620).reduced(20, 0).removeFromBottom(getHeight() / 4);
		syncBtn.setBounds(syncBtnBounds.removeFromTop(syncBtnBounds.getHeight() / 2).removeFromLeft(syncBtnBounds.getWidth() / 2));

		revBtn.setBounds(syncBtnBounds);
		g.setColour(juce::Colours::ghostwhite);
		g.setFont(22.f);
		g.drawText("Sync",90,430,100,100,juce::Justification::left);
		g.drawText("Reverse", 80, 510, 100, 100, juce::Justification::left);
		panSlider.setBounds(60, 250, 90, 90);

		lowPassSlider.setBounds(120, 200, 50, 50);
		highPassSlider.setBounds(40, 200, 50, 50);


	}

	void resized() override
	{

	}

	Visualizer visualizer;
	bool sync{ false };
	std::vector<std::unique_ptr<juce::Slider>> basicParams;
	juce::ToggleButton revBtn;
	juce::ToggleButton syncBtn;
	juce::Slider panSlider;
	juce::Slider lowPassSlider;
	juce::Slider highPassSlider;

	const std::vector<juce::String> titles{ "Time", "Time", "Fdbk", "Wet" };

private:
	CustomLF1 customLF1;
	CustomLF3 customLF3;
	std::vector<juce::String> labels = { "ms","","%","dB" };
	std::vector<juce::String> paramNames = { "Time","SyncTimes","Feedback","Wet" };

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayGuiComp)
};
