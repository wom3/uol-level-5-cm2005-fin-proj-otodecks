# Copilot instructions for OtoDecks

## Project snapshot
- OtoDecks is a JUCE desktop GUI app (macOS Xcode exporter committed) with two DJ decks mixed into a single output.
- Primary code lives in `Source/`; `JuceLibraryCode/` is generated and should not be hand-edited unless explicitly required.
- App entry is `Source/Main.cpp`, which hosts `MainComponent` in a `DocumentWindow`.

## Architecture and data flow
- `MainComponent` owns all core runtime objects:
  - shared `juce::AudioFormatManager` + `juce::AudioThumbnailCache`
  - two `DJAudioPlayer` instances (`player1`, `player2`)
  - two `DeckGUI` views bound to those players
  - one `juce::MixerAudioSource` that mixes both players to device output
  - one `PlaylistComponent` (currently UI-only)
- Audio callback flow:
  - `MainComponent::prepareToPlay` prepares players + mixer and adds each player as mixer input.
  - `MainComponent::getNextAudioBlock` delegates to mixer only.
- Deck control flow:
  - `DeckGUI` UI events call `DJAudioPlayer` (`start/stop/setGain/setSpeed/setPositionRelative`).
  - File load in `DeckGUI` updates both `DJAudioPlayer::loadURL` and `WaveFormDisplay::loadURL`.
  - `DeckGUI` timer (500 ms) polls `player->getPositionRelative()` and pushes to waveform playhead.

## File-specific patterns to follow
- Keep audio engine logic in `DJAudioPlayer` (`Source/DJAudioPlayer.*`), not in GUI classes.
- Keep view/layout + user interaction in `DeckGUI` (`Source/DeckGUI.*`) and `PlaylistComponent` (`Source/PlaylistComponent.*`).
- Keep cross-component wiring in `MainComponent` (`Source/MainComponent.*`).
- Use JUCE async file chooser style used in `DeckGUI::buttonClicked` (`launchAsync` with captured `this`).
- Current slider conventions in `DeckGUI`:
  - gain: `0..1`
  - speed: `0.25..4.0` (`1.0` normal)
  - position: relative `0..1`

## Build and run workflow (macOS)
- Open Xcode project: `Builds/MacOSX/OtoDecks.xcodeproj`.
- Build app target/scheme `OtoDecks` in Debug or Release.
- Command-line build (when full Xcode is installed and selected):
  - `xcodebuild -project Builds/MacOSX/OtoDecks.xcodeproj -scheme OtoDecks -configuration Debug build`
- If `.jucer` settings change, re-save `OtoDecks.jucer` in Projucer to regenerate exporter files.

## Integration boundaries and caveats
- JUCE modules are referenced via global JUCE path in `OtoDecks.jucer` (`../../../JUCE/modules`); environment setup must provide this.
- `PlaylistComponent` currently prints selected row button actions; it is not yet connected to deck playback.
- `tracks/` exists for project assets; no automatic ingest pipeline is implemented in current code.

## Editing guidelines for agents in this repo
- Prefer focused edits in `Source/*.cpp` and `Source/*.h`; avoid broad refactors.
- Preserve existing ownership style (stack-owned components in `MainComponent`, raw player pointer in `DeckGUI` constructor usage).
- When adding playback controls, route changes through `DJAudioPlayer` public methods, then call from GUI.
- Keep UI timer-driven waveform updates lightweight; avoid heavy work in `timerCallback`.
