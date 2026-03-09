/*
  ==============================================================================
    PlaylistComponent.cpp
    Created: 9 Mar 2026 1:42:19pm
    Author:  Omar Mohammad Warraich
  ==============================================================================
*/

#include "PlaylistComponent.h"

#include <algorithm>
#include <cmath>

PlaylistComponent::PlaylistComponent(juce::AudioFormatManager& formatManagerToUse)
    : formatManager(formatManagerToUse)
{
    /** Configure table columns to show required R2A metadata. */
    tableComponent.getHeader().addColumn("Track title", 1, 420);
    tableComponent.getHeader().addColumn("Duration", 2, 120);
    tableComponent.setModel(this);

    /** Import button loads one or more tracks into the music library table. */
    importButton.addListener(this);
    addAndMakeVisible(importButton);
    addAndMakeVisible(tableComponent);
}

PlaylistComponent::~PlaylistComponent()
{
    importButton.removeListener(this);
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PlaylistComponent::resized()
{
    constexpr int buttonHeight = 34;
    constexpr int margin = 8;

    importButton.setBounds(margin, margin, 160, buttonHeight);
    tableComponent.setBounds(0, buttonHeight + (margin * 2), getWidth(), getHeight() - (buttonHeight + (margin * 2)));
}

void PlaylistComponent::paintRowBackground(juce::Graphics& g,
                                           int rowNumber,
                                           int width,
                                           int height,
                                           bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::orange);
    }
    else
    {
        g.fillAll(juce::Colours::darkgrey);
    }
}



void PlaylistComponent::paintCell(juce::Graphics& g,
                                  int rowNumber,
                                  int columnId,
                                  int width,
                                  int height,
                                  bool rowIsSelected)
{
    juce::ignoreUnused(rowIsSelected);

    if (rowNumber < 0 || rowNumber >= getNumRows())
    {
        return;
    }

    const auto& track = libraryTracks[static_cast<size_t>(rowNumber)];

    if (columnId == 1)
    {
        g.drawText(track.title, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }
    else if (columnId == 2)
    {
        g.drawText(formatDuration(track.durationSeconds),
                   2,
                   0,
                   width - 4,
                   height,
                   juce::Justification::centred,
                   true);
    }
}

int PlaylistComponent::getNumRows()
{
    return static_cast<int>(libraryTracks.size());
}

void PlaylistComponent::buttonClicked(juce::Button* button)
{
    if (button != &importButton)
    {
        return;
    }

    /** Open an async chooser that supports selecting multiple audio files. */
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select one or more audio files...",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        "*.wav;*.mp3;*.aiff;*.aif;*.flac");

    const auto flags = juce::FileBrowserComponent::openMode
                       | juce::FileBrowserComponent::canSelectFiles
                       | juce::FileBrowserComponent::canSelectMultipleItems;

    fileChooser->launchAsync(flags, [this](const juce::FileChooser& chooser)
    {
        /** Populate library rows from the selected files. */
        addTracksFromFiles(chooser.getResults());
    });
}

void PlaylistComponent::addTracksFromFiles(const juce::Array<juce::File>& files)
{
    /** Import each readable file once, then cache title and duration metadata for the table. */
    for (const auto& file : files)
    {
        if (!file.existsAsFile())
        {
            continue;
        }

        const bool alreadyTracked = std::any_of(libraryTracks.begin(),
                                                libraryTracks.end(),
                                                [&file](const LibraryTrack& existing)
                                                {
                                                    return existing.file == file;
                                                });

        if (alreadyTracked)
        {
            continue;
        }

        auto reader = std::unique_ptr<juce::AudioFormatReader>(formatManager.createReaderFor(file));
        if (reader == nullptr)
        {
            continue;
        }

        const double durationSeconds = reader->lengthInSamples / reader->sampleRate;
        libraryTracks.push_back(LibraryTrack{file, file.getFileName(), durationSeconds});
    }

    tableComponent.updateContent();
    repaint();
}

juce::String PlaylistComponent::formatDuration(double seconds) const
{
    /** Convert seconds to a human-friendly minutes:seconds display string. */
    const int totalSeconds = static_cast<int>(std::round(std::max(0.0, seconds)));
    const int minutes = totalSeconds / 60;
    const int remainingSeconds = totalSeconds % 60;
    return juce::String::formatted("%d:%02d", minutes, remainingSeconds);
}
