BSc Computer Science
CM2005 Object Oriented Programming
End-of-term coursework assignment
Introduction

In this course, we have developed a basic DJ application called Otodecks. For the end-of-
term assessment, you are tasked with developing the application further by adding
functionalities to support a DJ in preparing and doing live mixing of the tracks. The DJ
application should be able to load music files into the music library or directly to the decks
to play the music on the relevant deck and other specific requirements listed below. Update
the user interface and music decks in your own interesting way that is easy to use. The DJ
application should persist information between application loads, so it will need to store its
state and configurations (i.e., music library, track-based hot cues and equaliser) in one or
more data files with a suitable data structure.

Requirements/TODOS

We will assess your work based on the following requirements and criteria:

R1: The DJ application should contain the following core functionality, as shown in class
• R1A: Load audio files into audio players.
• R1B: Play two or more tracks simultaneously.
• R1C: Mix track by varying each of their volumes.
• R1D: Speed up and slow down the tracks.

R2: The music library component should allow the user to manage their music and persist
even after the application has closed and reopened.
• R2A: music library component should allow the user to load multiple files into the
deck and display the track details (i.e., file name, durations, …) in the playlist.
• R2B: load music from the library to different decks.
• R2C: music library state (i.e., playlist with track details from R2A) should persist after re-opening the application.

R3: Create hot cue buttons (up to 8) that you can trigger instantly and jump to a particular time point in the track (i.e., at the vocal introductions, drum breaks, or drops for easier live
remixing). These hot cue buttons are very useful for the DJ to do live mixing of tracks and creativity.
• R3A: Assign hot cues using the buttons while playing the track or when paused.
• R3B: Allow the user to modify the cues individually.
• R3C: Allow the user to clear all hot cues using a dedicated button or similar.
• R3D: Persist and load the hot cues for each track when re-opening the application.

R4: Develop a three-band equaliser (EQ) mixer (i.e., high, mid and low) panel for each deck
using the DSP module within the JUCE library.
• R4A: Create three knobs or sliders to allow the user to adjust the equaliser value of
a track to high, medium and low.
• R4B: Persist the track’s original and the last adjusted EQ setting information when
closing and re-opening the application.

R5: Investigate and discuss what and how Beats Per Minute (BPM) information for a track is
used within the DJ application context.
• R5A: Discuss what and how BPM is used within a DJ application. Provide at least
two examples of scenarios in which they will be used and how.
• R5B: Highlight alternative techniques (or features available in popular DJ
applications) that can be used for the purpose and example discussed in R5A.
• R5C: Conceptually discuss how you would calculate the BPM value for a given
track.
• R5D: Implement and visualise the BPM value for a track within the application. If
you do not implement and visualise it, at least technically discuss how you would
implement it.

Code style and technique
Your code should be written according to the following style and technique guidelines:

C1: Code is organised into header (.h or .hpp) files and implementation files (.cpp). Header files contain class interface  definitions, cpp files contain implementations of class function
members. THE header files should be .h extensions as per the  existing conventions. Use .hpp only if .h is not possible.

C2: Class interfaces in header files have comments for each public function describing purpose, inputs and outputs only for the files you are going to add.
C3: Code is laid out clearly with consistent indenting.
C4: Code is organised into functions with clear inputs and outputs and a clear, limited
purpose.
C5: Code is stateless wherever possible – functions make use of data passing in preference
to global or class scope data.
C6: Functions, classes and variables have meaningful names, with a consistent naming
style.
C7: Functions do not change the state of class or global scope variables unless that is the
explicit purpose of a function (e.g., a setter).
Report
A report in a PDF format is required to document how ALL the requirements are
implemented, with:
i. a brief discussion on the code logic (include fragments of code if needed).
ii. any assumptions, decisions, or unique features or behaviours of the application
included
iii. supporting screenshots of the final outcome (with suitable captions/annotations where relevant). do not take screenshots as i would do so myself.
