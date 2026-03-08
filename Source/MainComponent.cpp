#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(volSlider);
    volSlider.setRange(0, 1);
    
    playButton.addListener(this);
    stopButton.addListener(this);
    volSlider.addListener(this);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    playing = false;
    gain = 0.5;
    phase = 0;
    dphase = 0;
    
    formatManager.registerBasicFormats();
    juce::URL audioURL{"file:///Users/owa/code/projuce/OtoDecks/tracks/aon_inspired.mp3"};
    
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false) );
    
    if (reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource
        (new juce:: AudioFormatReaderSource (reader, true));
        transportSource.setSource (newSource.get(),
        0, nullptr, reader->sampleRate);
        readerSource.reset (newSource.release());
        transportSource.start();
    }
    else
    {
    std::cout << "Something went wrong loading the file " << std::endl;
    }
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}


//void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
//{
//    // Your audio-processing code goes here!
//    if (!playing)
//    {
//        bufferToFill.clearActiveBufferRegion();
//        return;
//    }
//    
//    auto* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
//    auto* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
//    
//    for (auto i = 0; i < bufferToFill.numSamples; ++i)
//    {
//        auto sample = fmod(phase, 1.0f);
//        phase += fmod(dphase, 0.01f);
//        dphase += 0.0000005f;
//        leftChannel[i] = sample * 0.125f * gain;
//        rightChannel[i] = sample * 0.125f * gain;
//    }
//    
//    
//
//    // For more details, see the help for AudioProcessor::getNextAudioBlock()
//
//    // Right now we are not producing any data, in which case we need to clear the buffer
//    // (to prevent the output of random noise)
////    bufferToFill.clearActiveBufferRegion();
//}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    transportSource.releaseResources(); 
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    double rowH = getHeight() / 5;
    double rowW = getWidth();
    playButton.setBounds(0, 0, rowW, rowH);
    stopButton.setBounds(0, rowH, rowW, rowH);
    volSlider.setBounds(0, 2 * rowH, rowW, rowH);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        playing = true;
        dphase = 0;
    }
    if (button == &stopButton)
    {
        playing = false;
    }
    
}

void MainComponent::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &volSlider)
    {
        std::cout << "Vol slider moved" << volSlider.getValue() << std::endl;
        gain = volSlider.getValue();
    }
}
