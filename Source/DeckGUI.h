/*
  ==============================================================================

    DeckGUI.h
    Created: 9 Mar 2026 1:58:36am
    Author:  Omar Mohammad Warraich

  ==============================================================================
*/

#pragma once
#include <array>
#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveFormDisplay.h"

class DeckGUI : public juce::Component,
public juce::Button::Listener,
public juce::Slider::Listener,
public juce::FileDragAndDropTarget,
public juce::Timer
{
    public:
        DeckGUI(DJAudioPlayer* player,
                juce::AudioFormatManager & formatManagerToUse ,
                juce::AudioThumbnailCache & cacheToUse);
        ~DeckGUI();
        void paint (juce::Graphics&) override;
        void resized() override;
        /** implement Button::Listener */
        void buttonClicked (juce::Button *) override;
        /** implement Slider::Listener */
        void sliderValueChanged (juce::Slider *slider) override;
        bool isInterestedInFileDrag (const juce::StringArray &files) override;
        void filesDropped (const juce::StringArray &files, int x, int y) override;
        void timerCallback() override;

        /** Loads a track file into this deck's player and waveform display. */
        void loadTrackFile(const juce::File& file);

        /** Assigns or triggers one of the 8 hot cues based on button index (0..7). */
        void handleHotCuePressed(int cueIndex);

        /** Clears all stored hot cues and resets related cue button labels. */
        void clearAllHotCues();

        /** Applies hot-cue button labels based on whether each cue slot is assigned. */
        void refreshHotCueButtonLabels();

        /** Returns the file path used to persist hot-cue state across app sessions. */
        juce::File getHotCueStateFile() const;

        /** Loads hot cues for the currently loaded track from persistent storage. */
        void loadHotCuesForCurrentTrack();

        /** Saves hot cues for the currently loaded track into persistent storage. */
        void saveHotCuesForCurrentTrack() const;
    private:
        juce::TextButton playButton{"PLAY"};
        juce::TextButton stopButton{"STOP"};
        juce::TextButton loadButton{"LOAD"};
        juce::Slider gainSlider;
        juce::Slider speedSlider;
        juce::Slider posSlider;
        juce::Slider lowEqSlider;
        juce::Slider midEqSlider;
        juce::Slider highEqSlider;
        juce::TextButton setCueModeButton{"Set Cue Mode"};
        juce::TextButton clearHotCuesButton{"Clear All Cues"};
        std::array<juce::TextButton, 8> hotCueButtons{
          juce::TextButton{"HC1"},
          juce::TextButton{"HC2"},
          juce::TextButton{"HC3"},
          juce::TextButton{"HC4"},
          juce::TextButton{"HC5"},
          juce::TextButton{"HC6"},
          juce::TextButton{"HC7"},
          juce::TextButton{"HC8"}
        };
        std::array<double, 8> hotCuePositions{
          -1.0, -1.0, -1.0, -1.0,
          -1.0, -1.0, -1.0, -1.0
        };
        juce::String currentTrackPath;
        std::unique_ptr<juce::FileChooser> fChooser;
        DJAudioPlayer* player;
        WaveFormDisplay waveFormDisplay;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
