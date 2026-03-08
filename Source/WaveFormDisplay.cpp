/*
  ==============================================================================

    WaveFormDisplay.cpp
    Created: 9 Mar 2026 4:08:39am
    Author:  Omar Mohammad Warraich

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveFormDisplay.h"

WaveFormDisplay::WaveFormDisplay()
{
}

WaveFormDisplay::~WaveFormDisplay()
{
}

void WaveFormDisplay::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId)); // clea
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1); // draw an outline around the component
    g.setColour (juce::Colours::greenyellow);
    g.setFont (20.0f);
    g.drawText ("Wave Form Display", getLocalBounds(),
                juce::Justification::centred, true);
}

void WaveFormDisplay::resized()
{
    
}
