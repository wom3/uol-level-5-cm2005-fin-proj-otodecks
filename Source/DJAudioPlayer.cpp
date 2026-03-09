/*
  ==============================================================================

    DJAudioPlayer.cpp
    Created: 8 Mar 2026 12:10:29pm
    Author:  Omar Mohammad Warraich

  ==============================================================================
*/

#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(juce::AudioFormatManager& _formatManager) : formatManager(_formatManager)
{
}

DJAudioPlayer::~DJAudioPlayer()
{ 
    
}

void DJAudioPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected,sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    /** Initialise EQ filters for the current device sample rate before processing audio. */
    currentSampleRate = sampleRate;
    lowEqFilter.reset();
    midEqFilter.reset();
    highEqFilter.reset();
    updateEqFilters();
}

void DJAudioPlayer::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    resampleSource.getNextAudioBlock(bufferToFill);

    /** Apply low/mid/high EQ filters in sequence to each output channel. */
    const juce::SpinLock::ScopedLockType filterLock(eqLock);
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        float* channelData = bufferToFill.buffer->getWritePointer(channel,
                                                                  bufferToFill.startSample);
        lowEqFilter.processSamples(channelData, bufferToFill.numSamples);
        midEqFilter.processSamples(channelData, bufferToFill.numSamples);
        highEqFilter.processSamples(channelData, bufferToFill.numSamples);
    }
}

void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
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
    if (gain < 0 || gain > 1.0)
    {
        std::cout << "DJAudioPlayer::setGain gain should be between 0 and 1." << std::endl;
    }
    else
    {
        transportSource.setGain(gain);
    }
}

void DJAudioPlayer::setSpeed(double ratio)
{
    // ResamplingAudioSource expects 1.0 = normal speed, >1.0 faster, <1.0 slower.
    // Clamp to a safe, audible range to avoid zero/negative ratios.
    const double clamped = juce::jlimit(0.25, 4.0, ratio);
    resampleSource.setResamplingRatio(clamped);
}
 
void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

void DJAudioPlayer::setPositionRelative(double pos)
{
    if (pos < 0 || pos > 1.0)
    {
        std::cout << "DJAudioPlayer::setPositionRelative pos should be between 0 and 1." << std::endl;
    }
    else
    {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}

void DJAudioPlayer::setLowEqGainDb(double gainDb)
{
    /** Clamp UI-provided EQ range to a practical live-mixing band. */
    lowEqGainDb = juce::jlimit(-24.0, 24.0, gainDb);
    updateEqFilters();
}

void DJAudioPlayer::setMidEqGainDb(double gainDb)
{
    /** Clamp UI-provided EQ range to a practical live-mixing band. */
    midEqGainDb = juce::jlimit(-24.0, 24.0, gainDb);
    updateEqFilters();
}

void DJAudioPlayer::setHighEqGainDb(double gainDb)
{
    /** Clamp UI-provided EQ range to a practical live-mixing band. */
    highEqGainDb = juce::jlimit(-24.0, 24.0, gainDb);
    updateEqFilters();
}

void DJAudioPlayer::start()
{
    transportSource.start();
}

void DJAudioPlayer::stop()
{
    transportSource.stop();
}

double DJAudioPlayer::getPositionRelative()
{
    const double lengthInSeconds = transportSource.getLengthInSeconds();
    /** Guard against empty/unloaded transport length to avoid invalid division results. */
    if (lengthInSeconds <= 0.0)
    {
        return 0.0;
    }

    return transportSource.getCurrentPosition() / lengthInSeconds;
}

void DJAudioPlayer::updateEqFilters()
{
    /** Build three-band EQ filters: low shelf, mid peak, and high shelf. */
    const juce::SpinLock::ScopedLockType filterLock(eqLock);

    const float lowGain = static_cast<float>(juce::Decibels::decibelsToGain(lowEqGainDb));
    const float midGain = static_cast<float>(juce::Decibels::decibelsToGain(midEqGainDb));
    const float highGain = static_cast<float>(juce::Decibels::decibelsToGain(highEqGainDb));

    lowEqFilter.setCoefficients(juce::IIRCoefficients::makeLowShelf(currentSampleRate,
                                                                    200.0,
                                                                    0.707,
                                                                    lowGain));
    midEqFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(currentSampleRate,
                                                                      1000.0,
                                                                      1.0,
                                                                      midGain));
    highEqFilter.setCoefficients(juce::IIRCoefficients::makeHighShelf(currentSampleRate,
                                                                      4000.0,
                                                                      0.707,
                                                                      highGain));
}
