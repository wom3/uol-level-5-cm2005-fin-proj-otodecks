/*
  ==============================================================================

    DJAudioPlayer.cpp
    Created: 8 Mar 2026 12:10:29pm
    Author:  Omar Mohammad Warraich

  ==============================================================================
*/

#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer()
{
    
}

DJAudioPlayer::~DJAudioPlayer()
{
    
}

void DJAudioPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    formatManager.registerBasicFormats();
    transportSource.prepareToPlay(samplesPerBlockExpected,sampleRate);
}

void DJAudioPlayer::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
}

void DJAudioPlayer::loadURL(juce::URL audioURL)
{
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false) );
    
    if (reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource
        (new juce:: AudioFormatReaderSource (reader, true));
        transportSource.setSource (newSource.get(),
        0, nullptr, reader->sampleRate);
        readerSource.reset (newSource.release());
    }
    else
    {
    std::cout << "Something went wrong loading the file " << std::endl;
    }
}

void DJAudioPlayer::setGain(double gain)
{
    
}

void DJAudioPlayer::setSpeed(double ratio)
{
    
}

void DJAudioPlayer::setPosition(double posInSecs)
{
    
}

void DJAudioPlayer::start()
{
    transportSource.start();
}

void DJAudioPlayer::stop()
{
    transportSource.stop();
}
