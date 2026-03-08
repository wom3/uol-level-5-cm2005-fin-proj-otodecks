/*
  ==============================================================================

    DeckGUI.h
    Created: 9 Mar 2026 1:58:36am
    Author:  Omar Mohammad Warraich

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveFormDisplay.h"

class DeckGUI : public juce::Component,
public juce::Button::Listener,
public juce::Slider::Listener,
public juce::FileDragAndDropTarget
{
    public:
        DeckGUI(DJAudioPlayer* player);
        ~DeckGUI();
        void paint (juce::Graphics&) override;
        void resized() override;
        /** implement Button::Listener */
        void buttonClicked (juce::Button *) override;
        /** implement Slider::Listener */
        void sliderValueChanged (juce::Slider *slider) override;
        bool isInterestedInFileDrag (const juce::StringArray &files) override;
        void filesDropped (const juce::StringArray &files, int x, int y) override;
    private:
        juce::TextButton playButton{"PLAY"};
        juce::TextButton stopButton{"STOP"};
        juce::TextButton loadButton{"LOAD"};
        juce::Slider gainSlider;
        juce::Slider speedSlider;
        juce::Slider posSlider;
        std::unique_ptr<juce::FileChooser> fChooser;
        DJAudioPlayer* player;
        WaveFormDisplay waveFormDisplay;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
