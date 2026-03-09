/*
  ==============================================================================

    WaveFormDisplay.cpp
    Created: 9 Mar 2026 4:08:39am
    Author:  Omar Mohammad Warraich

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveFormDisplay.h"

WaveFormDisplay::WaveFormDisplay(juce::AudioFormatManager & formatManagerToUse,
                                 juce::AudioThumbnailCache & cacheToUse) :
                                 audioThumb(1000 , formatManagerToUse, cacheToUse),
                                 fileLoaded(false),
                                 position(0)
{
    audioThumb.addChangeListener(this);
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
    if (fileLoaded)
    {
        audioThumb.drawChannel(g, getLocalBounds(), 0, audioThumb.getTotalLength() , 0, 1.0f);
        g.setColour(juce::Colours::red);
        g.drawRect(position * getWidth(), 0, getWidth() / 20, getHeight()); 
    }
    else
    {
        g.setFont (20.0f);
        g.drawText ("File not loaded...", getLocalBounds(),
                    juce::Justification::centred, true);
    }
}

void WaveFormDisplay::resized()
{
    
}

void WaveFormDisplay::loadURL(juce::URL audioURL)
{
    audioThumb.clear();
    
    fileLoaded = audioThumb.setSource(new juce::URLInputSource(audioURL));
    
    if (fileLoaded)
    {
        std::cout << "wfd: loaded" << std::endl;
        repaint();
    }
    else
    {
        std::cout << "wfd: not loaded" << std::endl;
    }
}

void WaveFormDisplay::changeListenerCallback (juce::ChangeBroadcaster *source)
{
    std::cout << "wfd: change received! " << std::endl;
    repaint();
}

void WaveFormDisplay::setPositionRelative(double pos)
{
    if (pos != position)
    {
        position = pos;
        repaint();
    }
}
