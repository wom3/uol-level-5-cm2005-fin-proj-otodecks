/*
  ==============================================================================



    PlaylistComponent.h

    Created: 9 Mar 2026 1:42:19pm

    Author:  Omar Mohammad Warraich



  ==============================================================================

*/



#pragma once



#include <JuceHeader.h>

#include <vector>

#include <string>



class PlaylistComponent : public juce::Component, public juce::TableListBoxModel, public juce::Button::Listener

{

    public:

        PlaylistComponent();

        ~PlaylistComponent();

        

    void paint (juce::Graphics&) override;

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
    
    juce::Component* refreshComponentForCell (int rowNumber,
                                               int columnId,
                                               bool isRowSelected,
                                               juce::Component *existingComponentToUpdate
                                               ) override;

    void buttonClicked(juce::Button* button) override;

    int getNumRows() override;

    private:

        juce::TableListBox tableComponent;

        std::vector<std::string> trackTitles;

    

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)

};



