/*
  ==============================================================================

    DeckGUI.cpp
    Created: 9 Mar 2026 1:58:36am
    Author:  Omar Mohammad Warraich

  ==============================================================================
*/

#include "DeckGUI.h"
#include <JuceHeader.h>

DeckGUI::DeckGUI(DJAudioPlayer* _player,
                 juce::AudioFormatManager & formatManagerToUse,
                 juce::AudioThumbnailCache & cacheToUse
                 ) : player(_player),
                     waveFormDisplay(formatManagerToUse, cacheToUse)
{
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);
    addAndMakeVisible(waveFormDisplay);

    gainSlider.setRange(0, 1);
    gainSlider.setValue(0.2);

    // Configure speed slider: 1.0 = normal speed
    speedSlider.setRange(0.25, 4.0, 0.0);
    speedSlider.setSkewFactorFromMidPoint(1.0);
    speedSlider.setValue(1.0);

    posSlider.setRange(0, 1);

    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    gainSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);
    
    startTimer(500);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
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
    double rowH = getHeight() / 8;
    playButton.setBounds(0, 0, getWidth(), rowH);
    stopButton.setBounds(0, rowH, getWidth(), rowH);
    gainSlider.setBounds(0, rowH * 2, getWidth(), rowH);
    speedSlider.setBounds(0, rowH * 3, getWidth(), rowH);
    posSlider.setBounds(0, rowH * 4, getWidth(), rowH);
    waveFormDisplay.setBounds(0, rowH * 5, getWidth(), rowH * 2);
    loadButton.setBounds(0, rowH * 7, getWidth(), rowH);
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
                juce::URL audioURL{ result };
                player->loadURL(audioURL);
                waveFormDisplay.loadURL(audioURL);
            }
        });
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
    if (files.size() == 1)
    {
        player->loadURL(juce::URL{juce::File{files[0]}});
    }
}

void DeckGUI::timerCallback()
{
//    std::cout << "DeckGUI::timerCallback" << std::endl;
    waveFormDisplay.setPositionRelative(player->getPositionRelative());
}
