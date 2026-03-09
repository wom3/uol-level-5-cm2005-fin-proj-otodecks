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
    trackTitles.push_back("Track 2");
    trackTitles.push_back("Track 3");
    trackTitles.push_back("Track 4");
    trackTitles.push_back("Track 5");
    trackTitles.push_back("Track 6");
    
    
    tableComponent.getHeader().addColumn("Track title", 1, 500);
    tableComponent.getHeader().addColumn("", 2, 300);
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
};



void PlaylistComponent::resized()
{
    tableComponent.setBounds(0, 0, getWidth(), getHeight());
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
    g.drawText(trackTitles[rowNumber], 2, 0, width - 4, height, juce::Justification::centredLeft, true);
}

int PlaylistComponent::getNumRows()
{
    return static_cast<int>(trackTitles.size());
}



juce::Component* PlaylistComponent::refreshComponentForCell (int rowNumber,
                                           int columnId,
                                           bool isRowSelected,
                                           juce::Component *existingComponentToUpdate
                                           )
{
    if (columnId == 2)
    {
        if (existingComponentToUpdate == nullptr)
        {
            juce::TextButton* btn = new juce::TextButton{"play"};
            juce::String id{std::to_string(rowNumber)};
            btn->setComponentID(id);
            
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(juce::Button* button)
{
    int id = std::stoi(button->getComponentID().toStdString());
    std::cout << "PlaylistComponent::buttonClicked " << trackTitles[id] <<std::endl;
}
