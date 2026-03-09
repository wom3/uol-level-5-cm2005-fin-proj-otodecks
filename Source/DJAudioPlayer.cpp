/*
  ==============================================================================

    DJAudioPlayer.cpp
    Created: 8 Mar 2026 12:10:29pm
    Author:  Omar Mohammad Warraich

  ==============================================================================
*/

#include "DJAudioPlayer.h"

#include <cmath>
#include <vector>

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
        /** Estimate BPM before transferring reader ownership into transport source. */
        estimatedBpm = estimateBpmFromReader(*reader);

        std::unique_ptr<juce::AudioFormatReaderSource> newSource
        (new juce:: AudioFormatReaderSource (reader, true));
        transportSource.setSource (newSource.get(),
        0, nullptr, reader->sampleRate);
        readerSource.reset (newSource.release());
    }
    else
    {
    std::cout << "Something went wrong loading the file " << std::endl;
    estimatedBpm = 0.0;
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
    speedRatio = clamped;
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

double DJAudioPlayer::getEstimatedBpm() const
{
    return estimatedBpm;
}

double DJAudioPlayer::getSpeedRatio() const
{
    return speedRatio;
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

double DJAudioPlayer::estimateBpmFromReader(juce::AudioFormatReader& reader) const
{
    /** Build a downsampled mono signal to keep BPM analysis lightweight. */
    const double sourceRate = reader.sampleRate;
    if (sourceRate <= 0.0 || reader.lengthInSamples <= 0)
    {
        return 0.0;
    }

    constexpr double analysisRate = 4000.0;
    const int downsampleFactor = juce::jmax(1, static_cast<int>(std::round(sourceRate / analysisRate)));
    const juce::int64 maxSourceSamples = static_cast<juce::int64>(sourceRate * 120.0);
    const juce::int64 sourceSamplesToRead = juce::jmin(reader.lengthInSamples, maxSourceSamples);

    constexpr int blockSize = 8192;
    juce::AudioBuffer<float> readBuffer(juce::jmax(1, static_cast<int>(reader.numChannels)), blockSize);
    std::vector<float> mono;
    mono.reserve(static_cast<size_t>(sourceSamplesToRead / downsampleFactor) + 1);

    for (juce::int64 startSample = 0; startSample < sourceSamplesToRead; startSample += blockSize)
    {
        const int samplesThisBlock = static_cast<int>(juce::jmin<juce::int64>(blockSize, sourceSamplesToRead - startSample));
        reader.read(&readBuffer.getArrayOfWritePointers()[0],
                    readBuffer.getNumChannels(),
                    startSample,
                    samplesThisBlock);

        for (int sampleIndex = 0; sampleIndex < samplesThisBlock; sampleIndex += downsampleFactor)
        {
            float mixedSample = 0.0f;
            for (int channel = 0; channel < readBuffer.getNumChannels(); ++channel)
            {
                mixedSample += readBuffer.getSample(channel, sampleIndex);
            }
            mixedSample /= static_cast<float>(readBuffer.getNumChannels());
            mono.push_back(mixedSample);
        }
    }

    constexpr int frameSize = 1024;
    constexpr int hopSize = 256;
    if (static_cast<int>(mono.size()) < frameSize + hopSize)
    {
        return 0.0;
    }

    /** Compute RMS envelope then onset strength (positive envelope differences). */
    std::vector<double> envelope;
    for (int start = 0; start + frameSize < static_cast<int>(mono.size()); start += hopSize)
    {
        double sumSquares = 0.0;
        for (int i = 0; i < frameSize; ++i)
        {
            const double sampleValue = mono[static_cast<size_t>(start + i)];
            sumSquares += sampleValue * sampleValue;
        }
        envelope.push_back(std::sqrt(sumSquares / frameSize));
    }

    if (envelope.size() < 8)
    {
        return 0.0;
    }

    std::vector<double> onset(envelope.size(), 0.0);
    for (size_t index = 1; index < envelope.size(); ++index)
    {
        const double diff = envelope[index] - envelope[index - 1];
        onset[index] = diff > 0.0 ? diff : 0.0;
    }

    const double envelopeRate = (sourceRate / downsampleFactor) / hopSize;
    if (envelopeRate <= 0.0)
    {
        return 0.0;
    }

    const int lagMin = juce::jmax(1, static_cast<int>(std::round((60.0 * envelopeRate) / 200.0)));
    const int lagMax = juce::jmax(lagMin + 1, static_cast<int>(std::round((60.0 * envelopeRate) / 60.0)));

    /** Search periodic lag with maximum onset autocorrelation score. */
    double bestScore = 0.0;
    int bestLag = 0;
    for (int lag = lagMin; lag <= lagMax; ++lag)
    {
        double score = 0.0;
        for (size_t index = static_cast<size_t>(lag); index < onset.size(); ++index)
        {
            score += onset[index] * onset[index - static_cast<size_t>(lag)];
        }

        if (score > bestScore)
        {
            bestScore = score;
            bestLag = lag;
        }
    }

    if (bestLag <= 0 || bestScore <= 0.0)
    {
        return 0.0;
    }

    const double bpm = (60.0 * envelopeRate) / bestLag;
    return juce::jlimit(60.0, 200.0, bpm);
}
