# R5A: What BPM is and how it is used in a DJ application

## What BPM means
BPM (Beats Per Minute) is the tempo of a track, i.e., how many beats occur in one minute. In a DJ application, BPM is a practical control value used to align two tracks in time so transitions sound smooth and musical.

## How BPM is used in a DJ application
A DJ app uses BPM to support tempo-aware mixing decisions and controls. Typical uses include:
- estimating whether two tracks are naturally compatible for transition,
- deciding how much speed/tempo change is needed on one deck,
- helping keep beats synchronized over a blend section.

## Scenario 1: Beatmatching during a live transition
Suppose Deck A is playing at 128 BPM and Deck B is 124 BPM. Before bringing Deck B in, the DJ adjusts Deck B speed so its effective tempo approaches 128 BPM. This reduces phase drift between kick drums and makes the transition tighter.

In this scenario, BPM is used as a target reference:
- Deck A BPM is the anchor,
- Deck B BPM is adjusted toward the anchor,
- the DJ then fine-tunes by ear and phrasing.

## Scenario 2: Track selection and set flow planning
A DJ preparing a set can use BPM to choose the next track without a jarring energy change. For example, moving from 120 BPM to 121–123 BPM generally feels smoother than jumping directly to 138 BPM.

In this scenario, BPM is used as a library/filtering cue:
- identify nearby tempo candidates,
- maintain energy continuity,
- reduce corrective pitch/speed adjustments needed during playback.

## Why BPM matters in this project context
For OtoDecks, BPM is relevant because the application already supports dual-deck playback and speed control. BPM provides a measurable basis for tempo alignment between decks, making transitions more controlled than relying only on manual slider movement.

# R5B: Alternative techniques/features used in popular DJ applications

Popular DJ software does not rely on BPM value alone. It combines BPM with additional timing and musical tools to make transitions faster, tighter, and more reliable in live conditions.

## Alternative 1: Beatgrid + Sync
Many DJ apps (for example, Rekordbox, Serato DJ Pro, and Traktor) create a beatgrid: a timeline of where beats occur in the track. Once beatgrids are correct, a Sync feature can automatically align deck tempos and beat phase.

How this relates to Scenario 1 (live beatmatching):
- BPM gives the tempo target,
- beatgrid gives precise beat positions,
- Sync aligns both tracks quickly so drift is reduced.

This is an alternative to fully manual pitch matching and jog-wheel nudging.

## Alternative 2: Key-aware mixing (Harmonic Mixing)
Some DJ applications provide key detection and Camelot/Open Key display. The DJ can select tracks that are both tempo-compatible and harmonically compatible.

How this relates to Scenario 2 (track selection/set flow):
- BPM helps keep tempo transitions smooth,
- key compatibility helps avoid clashing melodies,
- together they improve overall transition quality.

This is an alternative to selecting tracks by BPM proximity only.

## Alternative 3: Quantize + Cue/Grid snapping
Popular DJ apps provide Quantize so cue triggers, loops, and jumps snap to nearest beat grid division (e.g., 1 beat, 1/2 beat).

How this supports both scenarios:
- during beatmatching, jumps stay rhythmically clean,
- during performance, cue triggering is more forgiving and musically tight.

This is an alternative to manual cue timing that can introduce slight off-beat errors.

## Alternative 4: Visual phase meters/waveform alignment
Applications often display stacked waveforms and phase meters showing whether beat peaks are leading/lagging.

How this supports Scenario 1:
- gives immediate visual feedback for tempo/phase mismatch,
- reduces dependence on only ear-based correction,
- helps maintain alignment over longer blends.

## Summary
BPM is still central, but modern DJ workflows usually combine it with beatgrid sync, quantization, key detection, and visual phase tools. These features can achieve the same goals as R5A scenarios (tight beat transitions and better track selection) with greater speed and consistency in live performance.

# R5C: Conceptual approach to calculate BPM for a track

To conceptually estimate BPM, a DJ application typically follows a beat-detection pipeline rather than reading BPM directly from metadata.

## Step 1: Pre-process audio
- Convert stereo to mono (or combine channels).
- Apply a band-pass emphasis where rhythmic transients are strong (often low-mid to percussion range).
- Optionally downsample for efficiency.

Purpose: reduce noise and focus on beat-relevant energy changes.

## Step 2: Build an onset (novelty) function
Compute a time-series that spikes when new rhythmic events occur (e.g., kicks/snares). Common conceptual methods:
- short-time energy difference,
- spectral flux (frame-to-frame spectral change),
- envelope derivative.

Result: a sequence where peaks represent likely beat onsets.

## Step 3: Estimate periodicity (tempo candidates)
Find repeating intervals in the onset function using one of these conceptual techniques:
- autocorrelation of onset signal,
- comb-filter bank,
- tempogram/Fourier analysis in tempo domain.

Choose the strongest periodic lag and convert lag to BPM.

## Step 4: Convert interval to BPM
If the dominant beat interval is $T$ seconds per beat, then:

$$
\mathrm{BPM} = \frac{60}{T}
$$

If using lag in samples $L$ at sample rate $f_s$:

$$
T = \frac{L}{f_s}, \quad \mathrm{BPM} = \frac{60 f_s}{L}
$$

## Step 5: Post-process and stabilize
- Constrain to realistic DJ range (for example 60–200 BPM).
- Resolve octave errors (e.g., 70 vs 140 BPM) using heuristics.
- Smooth over windows and take robust median/majority estimate.

## Practical notes
- Tracks with weak drums, variable tempo, or long ambient intros are harder.
- Some tracks may need section-based BPM (intro vs drop) rather than one global value.
- A hybrid approach is common: auto-estimate first, then allow user correction.

## Relevance to OtoDecks
For OtoDecks, this conceptual method can provide BPM values to support speed adjustment decisions and future sync-style features. Even without full auto-sync, displaying an estimated BPM per loaded track helps the DJ make better transition choices.

# R5D: BPM implementation and visualisation in OtoDecks

## What was implemented
In this project, BPM estimation and visualisation were implemented directly in the deck/player flow:

- **BPM estimation in audio engine (`DJAudioPlayer`)**
	- Added a per-track BPM analysis step when a file is loaded.
	- Implemented a lightweight onset-based method: downsample audio, compute energy envelope, build onset signal, then use autocorrelation to find dominant beat period.
	- Converted dominant lag to BPM and constrained result to a practical DJ tempo range.

- **BPM value access API**
	- Added a getter for the estimated BPM so GUI code can read the analysed value for the currently loaded track.

- **BPM visualisation in deck UI (`DeckGUI`)**
	- Added a dedicated BPM label/value display on each deck.
	- When a track is loaded (including restored tracks), the deck updates BPM text with the latest analysed value.
	- If BPM is unavailable, the UI shows a placeholder (`--`).

## Why this satisfies R5D
R5D asks to implement and visualise BPM within the application. The implementation computes BPM from audio content at load time, and the visualisation presents that value in each deck interface, making BPM directly available during mixing.
