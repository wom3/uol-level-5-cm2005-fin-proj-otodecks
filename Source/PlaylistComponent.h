/*
  ==============================================================================



    PlaylistComponent.h

    Created: 9 Mar 2026 1:42:19pm

    Author:  Omar Mohammad Warraich



  ==============================================================================

*/



#pragma once

#pragma once

#include <JuceHeader.h>
#include <functional>
#include <vector>

class PlaylistComponent : public juce::Component,
              public juce::TableListBoxModel,
              public juce::Button::Listener
{
public:
  /** Creates the playlist UI using a shared format manager for metadata extraction. */
  PlaylistComponent(juce::AudioFormatManager& formatManagerToUse);
  ~PlaylistComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;

  void paintRowBackground(juce::Graphics&,
              int rowNumber,
              int width,
              int height,
              bool rowIsSelected) override;

  void paintCell(juce::Graphics&,
           int rowNumber,
           int columnId,
           int width,
           int height,
           bool rowIsSelected) override;

  juce::Component* refreshComponentForCell(int rowNumber,
                       int columnId,
                       bool isRowSelected,
                       juce::Component* existingComponentToUpdate) override;

  int getNumRows() override;
  void buttonClicked(juce::Button* button) override;

  /** Sets callback used to load a selected library track into deck 1 or deck 2. */
  void setTrackLoadRequestHandler(std::function<void(const juce::File&, int)> handler);

private:
  /** Stores one imported library track with metadata shown in the playlist table. */
  struct LibraryTrack
  {
    juce::File file;
    juce::String title;
    double durationSeconds{0.0};
  };

  /** Adds selected files to the library and extracts display metadata (title + duration). */
  void addTracksFromFiles(const juce::Array<juce::File>& files);
  /** Formats seconds as mm:ss for the Duration column. */
  juce::String formatDuration(double seconds) const;

  juce::AudioFormatManager& formatManager;
  juce::TextButton importButton{"Import tracks"};
  juce::TableListBox tableComponent;
  std::vector<LibraryTrack> libraryTracks;
  std::unique_ptr<juce::FileChooser> fileChooser;
  std::function<void(const juce::File&, int)> trackLoadRequestHandler;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)
};



