/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 8 Mar 2026 12:10:29pm
    Author:  Omar Mohammad Warraich

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class DJAudioPlayer : public juce::AudioSource
{
    public:
    
    DJAudioPlayer(juce::AudioFormatManager& _formatManager );
    ~DJAudioPlayer();
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    void loadURL(juce::URL audioURL);
    void setGain(double gain);
    void setSpeed(double ratio);
    void setPosition(double posInSecs);
    void setPositionRelative(double pos);

    /** Sets low-band EQ gain in dB (negative cuts, positive boosts). */
    void setLowEqGainDb(double gainDb);

    /** Sets mid-band EQ gain in dB (negative cuts, positive boosts). */
    void setMidEqGainDb(double gainDb);

    /** Sets high-band EQ gain in dB (negative cuts, positive boosts). */
    void setHighEqGainDb(double gainDb);
    
    /** get the relative position of the playhead */
    double getPositionRelative(); 
    
    void start();
    void stop();
    
    private:
    
    juce::AudioFormatManager& formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource>  readerSource;
    juce::AudioTransportSource transportSource;
    // Wrap transportSource with a resampler so speed changes affect playback
    juce::ResamplingAudioSource resampleSource{&transportSource, false, 2};

    /** Rebuilds all EQ filter coefficients from current sample rate and gain values. */
    void updateEqFilters();

    double currentSampleRate{44100.0};
    double lowEqGainDb{0.0};
    double midEqGainDb{0.0};
    double highEqGainDb{0.0};
    juce::SpinLock eqLock;
    juce::IIRFilter lowEqFilter;
    juce::IIRFilter midEqFilter;
    juce::IIRFilter highEqFilter;
};
