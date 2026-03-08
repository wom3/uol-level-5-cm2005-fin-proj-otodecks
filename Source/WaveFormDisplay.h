/*
  ==============================================================================

    WaveFormDisplay.h
    Created: 9 Mar 2026 4:08:39am
    Author:  Omar Mohammad Warraich

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class WaveFormDisplay : public juce::Component
{
    public:
        WaveFormDisplay();
        ~WaveFormDisplay();
        
        void paint (juce::Graphics&) override;
        void resized() override;
        
    private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveFormDisplay)
};
