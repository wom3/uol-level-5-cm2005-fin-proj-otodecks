#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent,
public juce::Button::Listener,
public juce::Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    void buttonClicked (juce::Button *) override;
    void sliderValueChanged (juce::Slider *slider) override;

private:
    //==============================================================================
    // Your private member variables go here...
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbCache{100};
    
    DJAudioPlayer player1{formatManager};
    DeckGUI deck1{&player1, formatManager, thumbCache};
    
    DJAudioPlayer player2{formatManager};
    DeckGUI deck2{&player2, formatManager, thumbCache };
    
    juce::MixerAudioSource mixerSource;
    
    std::unique_ptr<juce::FileChooser> fChooser;
    juce::AudioThumbnail audioThumb;
    PlaylistComponent playlistComponent; 
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
