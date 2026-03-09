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
    
    void start();
    void stop();
    
    private:
    
    juce::AudioFormatManager& formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource>  readerSource;
    juce::AudioTransportSource transportSource;
    // Wrap transportSource with a resampler so speed changes affect playback
    juce::ResamplingAudioSource resampleSource{&transportSource, false, 2};
};
