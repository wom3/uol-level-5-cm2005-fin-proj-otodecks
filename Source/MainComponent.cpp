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
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(loadButton);
    gainSlider.setRange(0, 1);
    
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    gainSlider.addListener(this);
    speedSlider.addListener(this);
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
    
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
//    transportSource.getNextAudioBlock(bufferToFill);
//    resampleSource.getNextAudioBlock(bufferToFill);
    player1.getNextAudioBlock(bufferToFill);
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
    player1.releaseResources(); 
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
    gainSlider.setBounds(0, 2 * rowH, rowW, rowH);
    speedSlider.setBounds(0, 3 * rowH, rowW, rowH);
    loadButton.setBounds(0, 4  * rowH, rowW, rowH);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        playing = true;
        dphase = 0;
        player1.start();
    }
    if (button == &stopButton)
    {
        playing = false;
        player1.stop();
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
                player1.loadURL(audioURL);
            }
        });
    }
}

void MainComponent::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &gainSlider)
    {
        std::cout << gainSlider.getValue() << std::endl;
        gain = gainSlider.getValue();
        transportSource.setGain(gain);
    }
    if (slider == &speedSlider)
    {
        resampleSource.setResamplingRatio(slider->getValue());  
    }
}

void MainComponent::loadURL(juce::URL audioURL)
{
    // Use the legacy/deprecated overload that your JUCE version supports
    std::unique_ptr<juce::InputStream> stream = audioURL.createInputStream(false);
    if (stream != nullptr)
    {
        if (auto* reader = formatManager.createReaderFor(std::move(stream)))
        {
            std::unique_ptr<juce::AudioFormatReaderSource> newSource(
                new juce::AudioFormatReaderSource(reader, true));
            transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
            readerSource.reset(newSource.release());
            return;
        }
    }

    std::cout << "Something went wrong loading the file" << std::endl;
}
  
