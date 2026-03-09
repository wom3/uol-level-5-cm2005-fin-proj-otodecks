/*

  ==============================================================================



    PlaylistComponent.cpp

    Created: 9 Mar 2026 1:42:19pm

    Author:  Omar Mohammad Warraich



  ==============================================================================

*/



#include <JuceHeader.h>

#include "PlaylistComponent.h"



PlaylistComponent::PlaylistComponent()

{

    trackTitles.push_back("Track 1");

    trackTitles.push_back("Track 2 ");

    tableComponent.getHeader().addColumn("Artist", 1, 400);

    tableComponent.getHeader().addColumn("Track title", 2, 400);

    tableComponent.setModel(this);

    

    addAndMakeVisible(tableComponent);

}



PlaylistComponent::~PlaylistComponent()

{

}



void PlaylistComponent::paint (juce::Graphics& g)

{

//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId)); // clea

//    g.setColour (juce::Colours::grey);

//    g.drawRect (getLocalBounds(), 1); // draw an outline around the component

//    g.setColour (juce::Colours::greenyellow);

//    g.setFont (20.0f);

//    g.drawText ("Wave Form Display", getLocalBounds(),

//                juce::Justification::centred, true);

}



void PlaylistComponent::resized()

{

    tableComponent.setBounds(0, 0, getWidth(), getHeight());

}



void PlaylistComponent::paintRowBackground(juce::Graphics&,

                        int rowNumber,

                        int width,

                        int height,

                        bool rowIsSelected)

{

    

}



void PlaylistComponent::paintCell(juce::Graphics&,

                        int rowNumber,

                        int columnId,

                        int width,

                        int height,

                        bool rowIsSelected)

{

    

}



int PlaylistComponent::getNumRows()

{

    return static_cast<int>(trackTitles.size());

}



