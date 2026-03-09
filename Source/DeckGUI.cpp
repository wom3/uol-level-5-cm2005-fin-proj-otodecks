/*
  ==============================================================================

    DeckGUI.cpp
    Created: 9 Mar 2026 1:58:36am
    Author:  Omar Mohammad Warraich

  ==============================================================================
*/

#include "DeckGUI.h"
#include <JuceHeader.h>

#include <cmath>

namespace
{
/** Builds an empty hot-cue JSON state object with a tracks array. */
juce::var createEmptyHotCueState()
{
    auto stateObject = std::make_unique<juce::DynamicObject>();
    stateObject->setProperty("tracks", juce::var(juce::Array<juce::var>{}));
    return juce::var(stateObject.release());
}

/** Builds an empty deck-track JSON state object with deck-specific entries. */
juce::var createEmptyDeckTrackState()
{
    auto stateObject = std::make_unique<juce::DynamicObject>();
    stateObject->setProperty("deckTracks", juce::var(juce::Array<juce::var>{}));
    return juce::var(stateObject.release());
}
}

DeckGUI::DeckGUI(DJAudioPlayer* _player,
                 const juce::String& _deckStateId,
                 juce::AudioFormatManager & formatManagerToUse,
                 juce::AudioThumbnailCache & cacheToUse
                 ) : deckStateId(_deckStateId),
                     player(_player),
                     waveFormDisplay(formatManagerToUse, cacheToUse)
{
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);
    addAndMakeVisible(gainLabel);
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(positionLabel);
    addAndMakeVisible(lowEqSlider);
    addAndMakeVisible(midEqSlider);
    addAndMakeVisible(highEqSlider);
    addAndMakeVisible(lowEqLabel);
    addAndMakeVisible(midEqLabel);
    addAndMakeVisible(highEqLabel);
    addAndMakeVisible(setCueModeButton);
    addAndMakeVisible(clearHotCuesButton);
    addAndMakeVisible(waveFormDisplay);
    /** Create and register 8 hot-cue buttons for assign/jump interactions. */
    for (int index = 0; index < static_cast<int>(hotCueButtons.size()); ++index)
    {
        hotCueButtons[static_cast<size_t>(index)].setComponentID(juce::String(index));
        hotCueButtons[static_cast<size_t>(index)].addListener(this);
        addAndMakeVisible(hotCueButtons[static_cast<size_t>(index)]);
    }

    gainSlider.setRange(0, 1);
    gainSlider.setValue(0.2);
    gainSlider.setTextValueSuffix(" ");

    // Configure speed slider: 1.0 = normal speed
    speedSlider.setRange(0.25, 4.0, 0.0);
    speedSlider.setSkewFactorFromMidPoint(1.0);
    speedSlider.setValue(1.0);
    speedSlider.setTextValueSuffix(" x");

    posSlider.setRange(0, 1);

    /** Label primary deck controls for quicker live-use identification. */
    gainLabel.setText("VOL", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centredLeft);
    speedLabel.setText("SPEED", juce::dontSendNotification);
    speedLabel.setJustificationType(juce::Justification::centredLeft);
    positionLabel.setText("POSITION", juce::dontSendNotification);
    positionLabel.setJustificationType(juce::Justification::centredLeft);

    /** Three-band EQ sliders: 0 dB is neutral, negatives cut and positives boost. */
    lowEqSlider.setRange(-24.0, 24.0, 0.1);
    lowEqSlider.setValue(0.0);
    lowEqSlider.setTextValueSuffix(" dB");
    midEqSlider.setRange(-24.0, 24.0, 0.1);
    midEqSlider.setValue(0.0);
    midEqSlider.setTextValueSuffix(" dB");
    highEqSlider.setRange(-24.0, 24.0, 0.1);
    highEqSlider.setValue(0.0);
    highEqSlider.setTextValueSuffix(" dB");

    /** Label EQ bands so the controls are easier to identify while mixing. */
    lowEqLabel.setText("LOW (Bass)", juce::dontSendNotification);
    lowEqLabel.setJustificationType(juce::Justification::centredLeft);
    midEqLabel.setText("MID", juce::dontSendNotification);
    midEqLabel.setJustificationType(juce::Justification::centredLeft);
    highEqLabel.setText("HIGH (Treble)", juce::dontSendNotification);
    highEqLabel.setJustificationType(juce::Justification::centredLeft);

    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    setCueModeButton.addListener(this);
    clearHotCuesButton.addListener(this);
    gainSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);
    lowEqSlider.addListener(this);
    midEqSlider.addListener(this);
    highEqSlider.addListener(this);

    /** Toggle mode allows individual hot-cue overwrite without affecting other cues. */
    setCueModeButton.setClickingTogglesState(true);
    setCueModeButton.setToggleState(false, juce::dontSendNotification);
    
    startTimer(500);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::initialisePersistentState()
{
    /** Restore this deck's last loaded track after app restart. */
    loadCurrentTrackState();
}

void DeckGUI::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId)); // clea
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1); // draw an outline around the component
    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("DeckGUI", getLocalBounds(),
                juce::Justification::centred, true); // draw some placeholder text
}

void DeckGUI::resized()
{
    /** Reserve two rows for 8 hot cues while preserving existing deck controls. */
    const int rowH = getHeight() / 16;
    const int cueButtonWidth = getWidth() / 4;
    const int labelWidth = 110;

    playButton.setBounds(0, 0, getWidth(), rowH);
    stopButton.setBounds(0, rowH, getWidth(), rowH);
    gainLabel.setBounds(0, rowH * 2, labelWidth, rowH);
    gainSlider.setBounds(labelWidth, rowH * 2, getWidth() - labelWidth, rowH);
    speedLabel.setBounds(0, rowH * 3, labelWidth, rowH);
    speedSlider.setBounds(labelWidth, rowH * 3, getWidth() - labelWidth, rowH);
    positionLabel.setBounds(0, rowH * 4, labelWidth, rowH);
    posSlider.setBounds(labelWidth, rowH * 4, getWidth() - labelWidth, rowH);
    /** Place each EQ label on the left and its slider on the right. */
    lowEqLabel.setBounds(0, rowH * 5, labelWidth, rowH);
    lowEqSlider.setBounds(labelWidth, rowH * 5, getWidth() - labelWidth, rowH);
    midEqLabel.setBounds(0, rowH * 6, labelWidth, rowH);
    midEqSlider.setBounds(labelWidth, rowH * 6, getWidth() - labelWidth, rowH);
    highEqLabel.setBounds(0, rowH * 7, labelWidth, rowH);
    highEqSlider.setBounds(labelWidth, rowH * 7, getWidth() - labelWidth, rowH);
    waveFormDisplay.setBounds(0, rowH * 8, getWidth(), rowH * 3);

    /** Lay out hot cues as a 2x4 grid beneath the waveform display. */
    for (int cueIndex = 0; cueIndex < static_cast<int>(hotCueButtons.size()); ++cueIndex)
    {
        const int cueRow = cueIndex / 4;
        const int cueColumn = cueIndex % 4;
        hotCueButtons[static_cast<size_t>(cueIndex)].setBounds(cueColumn * cueButtonWidth,
                                                               (rowH * 11) + (cueRow * rowH),
                                                               cueButtonWidth,
                                                               rowH);
    }

    setCueModeButton.setBounds(0, rowH * 13, getWidth(), rowH);
    clearHotCuesButton.setBounds(0, rowH * 14, getWidth(), rowH);
    loadButton.setBounds(0, rowH * 15, getWidth(), rowH);
}

void DeckGUI::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        player->start();
    }
    if (button == &stopButton)
    {
        player->stop();
    }
    if (button == &loadButton)
    {
        // Use JUCE 8 async FileChooser API
        fChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file to play...",
            juce::File::getSpecialLocation(juce::File::userHomeDirectory),
            "*.wav;*.mp3;*.aiff;*.aif");

        auto flags = juce::FileBrowserComponent::openMode
                   | juce::FileBrowserComponent::canSelectFiles;

        fChooser->launchAsync(flags, [this](const juce::FileChooser& fc)
        {
            auto result = fc.getResult();
            if (result.existsAsFile())
            {
                loadTrackFile(result);
            }
        });
    }

    if (button == &setCueModeButton)
    {
        /** Visual feedback indicates whether cue presses will overwrite stored cue points. */
        const bool isEditMode = setCueModeButton.getToggleState();
        setCueModeButton.setButtonText(isEditMode ? "Set Cue Mode: ON" : "Set Cue Mode");
        return;
    }

    if (button == &clearHotCuesButton)
    {
        /** R3C: dedicated action to clear all assigned hot cues for this deck. */
        clearAllHotCues();
        return;
    }

    for (int cueIndex = 0; cueIndex < static_cast<int>(hotCueButtons.size()); ++cueIndex)
    {
        if (button == &hotCueButtons[static_cast<size_t>(cueIndex)])
        {
            /** Hot-cue button routes to assign-or-jump behavior for that cue index. */
            handleHotCuePressed(cueIndex);
            return;
        }
    }
}

void DeckGUI::sliderValueChanged (juce::Slider *slider)
{
    if (slider == &gainSlider)
    {
    player->setGain(slider->getValue());
    }
    if (slider == &speedSlider)
    {
    player->setSpeed(slider->getValue());
    }
    if (slider == &posSlider)
    {
    player->setPositionRelative(slider->getValue());
    }
    if (slider == &lowEqSlider)
    {
    /** Route low-band EQ control to player DSP processing. */
    player->setLowEqGainDb(slider->getValue());
    }
    if (slider == &midEqSlider)
    {
    /** Route mid-band EQ control to player DSP processing. */
    player->setMidEqGainDb(slider->getValue());
    }
    if (slider == &highEqSlider)
    {
    /** Route high-band EQ control to player DSP processing. */
    player->setHighEqGainDb(slider->getValue());
    }
}

bool DeckGUI::isInterestedInFileDrag (const juce::StringArray &files)
{
    std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl;

    for (const auto &filePath : files)
    {
        juce::File file{filePath};
        if (file.existsAsFile() && file.hasFileExtension("wav;mp3;aiff;aif"))
        {
            return true;
        }
    }

    return false;
}

void DeckGUI::filesDropped (const juce::StringArray &files, int x, int y)
{
    std::cout << "DeckGUI::filesDropped" << std::endl;
    juce::ignoreUnused(x, y);
    if (files.size() == 1)
    {
        loadTrackFile(juce::File{files[0]});
    }
}

void DeckGUI::loadTrackFile(const juce::File& file)
{
    /** Single entry point for loading audio into both transport and waveform UI. */
    if (!file.existsAsFile())
    {
        return;
    }

    const juce::URL audioURL{file};
    player->loadURL(audioURL);
    waveFormDisplay.loadURL(audioURL);

    /** Track identity is used as the key for saving/loading per-track hot cues. */
    currentTrackPath = file.getFullPathName();
    loadHotCuesForCurrentTrack();

    /** Persist last loaded track path so this deck can restore it on next launch. */
    saveCurrentTrackState();
}

void DeckGUI::handleHotCuePressed(int cueIndex)
{
    /** Cue press either overwrites one cue in edit mode or triggers existing cue in normal mode. */
    if (cueIndex < 0 || cueIndex >= static_cast<int>(hotCuePositions.size()))
    {
        return;
    }

    const double currentPosition = player->getPositionRelative();
    const bool hasValidCurrentPosition = std::isfinite(currentPosition) && currentPosition >= 0.0 && currentPosition <= 1.0;
    const bool isEditMode = setCueModeButton.getToggleState();

    if (isEditMode && hasValidCurrentPosition)
    {
        /** R3B: overwrite only the selected cue with the current playhead position. */
        hotCuePositions[static_cast<size_t>(cueIndex)] = currentPosition;
        refreshHotCueButtonLabels();
        /** Persist modified cue state for the active track (R3D). */
        saveHotCuesForCurrentTrack();
        return;
    }

    const double cuePosition = hotCuePositions[static_cast<size_t>(cueIndex)];
    if (cuePosition >= 0.0 && cuePosition <= 1.0)
    {
        player->setPositionRelative(cuePosition);
        return;
    }

    if (hasValidCurrentPosition)
    {
        hotCuePositions[static_cast<size_t>(cueIndex)] = currentPosition;
        refreshHotCueButtonLabels();
        /** Persist modified cue state for the active track (R3D). */
        saveHotCuesForCurrentTrack();
    }
}

void DeckGUI::clearAllHotCues()
{
    /** Reset all cue slots and restore default hot-cue button labels. */
    for (int cueIndex = 0; cueIndex < static_cast<int>(hotCuePositions.size()); ++cueIndex)
    {
        hotCuePositions[static_cast<size_t>(cueIndex)] = -1.0;
    }

    refreshHotCueButtonLabels();
    /** Persist cleared cue state for the active track (R3D). */
    saveHotCuesForCurrentTrack();
}

void DeckGUI::refreshHotCueButtonLabels()
{
    /** Show '*' suffix for cues that are currently assigned on this track. */
    for (int cueIndex = 0; cueIndex < static_cast<int>(hotCuePositions.size()); ++cueIndex)
    {
        const bool isAssigned = hotCuePositions[static_cast<size_t>(cueIndex)] >= 0.0;
        const auto baseLabel = "HC" + juce::String(cueIndex + 1);
        hotCueButtons[static_cast<size_t>(cueIndex)].setButtonText(isAssigned ? baseLabel + "*" : baseLabel);
    }
}

juce::File DeckGUI::getHotCueStateFile() const
{
    /** Store hot-cue data in app-data directory so cues survive app restarts. */
    const auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                                .getChildFile("OtoDecks");
    appDataDir.createDirectory();
    return appDataDir.getChildFile("hot_cues.json");
}

void DeckGUI::loadHotCuesForCurrentTrack()
{
    /** Default to no cues when there is no saved state for the current track. */
    hotCuePositions.fill(-1.0);

    if (currentTrackPath.isEmpty())
    {
        refreshHotCueButtonLabels();
        return;
    }

    const auto stateFile = getHotCueStateFile();
    if (!stateFile.existsAsFile())
    {
        refreshHotCueButtonLabels();
        return;
    }

    const auto parsedState = juce::JSON::parse(stateFile.loadFileAsString());
    const auto* stateObject = parsedState.getDynamicObject();
    if (stateObject == nullptr)
    {
        refreshHotCueButtonLabels();
        return;
    }

    const auto tracksVar = stateObject->getProperty("tracks");
    const auto* tracks = tracksVar.getArray();
    if (tracks == nullptr)
    {
        refreshHotCueButtonLabels();
        return;
    }

    for (const auto& trackEntry : *tracks)
    {
        const auto* trackObject = trackEntry.getDynamicObject();
        if (trackObject == nullptr)
        {
            continue;
        }

        if (trackObject->getProperty("path").toString() != currentTrackPath)
        {
            continue;
        }

        const auto cuesVar = trackObject->getProperty("cues");
        const auto* cueArray = cuesVar.getArray();
        if (cueArray == nullptr)
        {
            break;
        }

        for (int cueIndex = 0; cueIndex < static_cast<int>(hotCuePositions.size()) && cueIndex < cueArray->size(); ++cueIndex)
        {
            const double cueValue = cueArray->getReference(cueIndex);
            hotCuePositions[static_cast<size_t>(cueIndex)] = (cueValue >= 0.0 && cueValue <= 1.0) ? cueValue : -1.0;
        }
        break;
    }

    refreshHotCueButtonLabels();
}

void DeckGUI::saveHotCuesForCurrentTrack() const
{
    /** Skip persistence if no track has been loaded onto this deck yet. */
    if (currentTrackPath.isEmpty())
    {
        return;
    }

    const auto stateFile = getHotCueStateFile();
    juce::var state = createEmptyHotCueState();

    if (stateFile.existsAsFile())
    {
        const auto parsed = juce::JSON::parse(stateFile.loadFileAsString());
        if (parsed.getDynamicObject() != nullptr)
        {
            state = parsed;
        }
    }

    auto* stateObject = state.getDynamicObject();
    if (stateObject == nullptr)
    {
        state = createEmptyHotCueState();
        stateObject = state.getDynamicObject();
    }

    auto tracksVar = stateObject->getProperty("tracks");
    auto* tracks = tracksVar.getArray();
    if (tracks == nullptr)
    {
        stateObject->setProperty("tracks", juce::var(juce::Array<juce::var>{}));
        tracksVar = stateObject->getProperty("tracks");
        tracks = tracksVar.getArray();
    }

    if (tracks == nullptr)
    {
        return;
    }

    juce::Array<juce::var> cueArray;
    for (const auto cue : hotCuePositions)
    {
        cueArray.add((cue >= 0.0 && cue <= 1.0) ? cue : -1.0);
    }

    auto updatedTrackObject = std::make_unique<juce::DynamicObject>();
    updatedTrackObject->setProperty("path", currentTrackPath);
    updatedTrackObject->setProperty("cues", juce::var(cueArray));
    const juce::var updatedTrackVar(updatedTrackObject.release());

    bool replacedExisting = false;
    for (int index = 0; index < tracks->size(); ++index)
    {
        const auto* existingObject = tracks->getReference(index).getDynamicObject();
        if (existingObject != nullptr && existingObject->getProperty("path").toString() == currentTrackPath)
        {
            tracks->set(index, updatedTrackVar);
            replacedExisting = true;
            break;
        }
    }

    if (!replacedExisting)
    {
        tracks->add(updatedTrackVar);
    }

    stateFile.replaceWithText(juce::JSON::toString(state, true));
}

juce::File DeckGUI::getDeckTrackStateFile() const
{
    /** Store per-deck loaded track paths in app-data so decks restore on reopen. */
    const auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                                .getChildFile("OtoDecks");
    appDataDir.createDirectory();
    return appDataDir.getChildFile("deck_tracks.json");
}

void DeckGUI::loadCurrentTrackState()
{
    /** Skip restore if this deck does not have a stable state id. */
    if (deckStateId.isEmpty())
    {
        return;
    }

    const auto stateFile = getDeckTrackStateFile();
    if (!stateFile.existsAsFile())
    {
        return;
    }

    const auto parsedState = juce::JSON::parse(stateFile.loadFileAsString());
    const auto* stateObject = parsedState.getDynamicObject();
    if (stateObject == nullptr)
    {
        return;
    }

    const auto* deckTracks = stateObject->getProperty("deckTracks").getArray();
    if (deckTracks == nullptr)
    {
        return;
    }

    for (const auto& deckEntry : *deckTracks)
    {
        const auto* deckObject = deckEntry.getDynamicObject();
        if (deckObject == nullptr)
        {
            continue;
        }

        if (deckObject->getProperty("deckId").toString() != deckStateId)
        {
            continue;
        }

        const auto savedPath = deckObject->getProperty("path").toString();
        if (savedPath.isEmpty())
        {
            return;
        }

        const juce::File savedFile{savedPath};
        if (savedFile.existsAsFile())
        {
            loadTrackFile(savedFile);
        }
        return;
    }
}

void DeckGUI::saveCurrentTrackState() const
{
    /** Persist current deck track only when deck id and track path are valid. */
    if (deckStateId.isEmpty() || currentTrackPath.isEmpty())
    {
        return;
    }

    const auto stateFile = getDeckTrackStateFile();
    juce::var state = createEmptyDeckTrackState();

    if (stateFile.existsAsFile())
    {
        const auto parsed = juce::JSON::parse(stateFile.loadFileAsString());
        if (parsed.getDynamicObject() != nullptr)
        {
            state = parsed;
        }
    }

    auto* stateObject = state.getDynamicObject();
    if (stateObject == nullptr)
    {
        state = createEmptyDeckTrackState();
        stateObject = state.getDynamicObject();
    }

    auto deckTracksVar = stateObject->getProperty("deckTracks");
    auto* deckTracks = deckTracksVar.getArray();
    if (deckTracks == nullptr)
    {
        stateObject->setProperty("deckTracks", juce::var(juce::Array<juce::var>{}));
        deckTracksVar = stateObject->getProperty("deckTracks");
        deckTracks = deckTracksVar.getArray();
    }

    if (deckTracks == nullptr)
    {
        return;
    }

    auto updatedDeckObject = std::make_unique<juce::DynamicObject>();
    updatedDeckObject->setProperty("deckId", deckStateId);
    updatedDeckObject->setProperty("path", currentTrackPath);
    const juce::var updatedDeckVar(updatedDeckObject.release());

    bool replacedExisting = false;
    for (int index = 0; index < deckTracks->size(); ++index)
    {
        const auto* existingObject = deckTracks->getReference(index).getDynamicObject();
        if (existingObject != nullptr && existingObject->getProperty("deckId").toString() == deckStateId)
        {
            deckTracks->set(index, updatedDeckVar);
            replacedExisting = true;
            break;
        }
    }

    if (!replacedExisting)
    {
        deckTracks->add(updatedDeckVar);
    }

    stateFile.replaceWithText(juce::JSON::toString(state, true));
}

void DeckGUI::timerCallback()
{
//    std::cout << "DeckGUI::timerCallback" << std::endl;
    waveFormDisplay.setPositionRelative(player->getPositionRelative());
}
