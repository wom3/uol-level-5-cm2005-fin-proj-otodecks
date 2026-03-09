#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : audioThumb(1000, formatManager, thumbCache)

//DeckGUI::DeckGUI(DJAudioPlayer* _player) : player(_player)
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
    addAndMakeVisible(deck1);
    addAndMakeVisible(deck2);
    
    formatManager.registerBasicFormats();
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
    
//    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
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
    deck1.setBounds(0, 0, getWidth()/2, getHeight());
    deck2.setBounds(getWidth()/2, 0, getWidth()/2, getHeight());
}

void MainComponent::buttonClicked(juce::Button* button)
{
//    if (button == &playButton)
//    {
//        player1.start();
//    }
//    if (button == &stopButton)
//    {
//        player1.stop();
//    }
//    if (button == &loadButton)
//    {
//        // Use JUCE 8 async FileChooser API
//        fChooser = std::make_unique<juce::FileChooser>(
//            "Select an audio file to play...",
//            juce::File::getSpecialLocation(juce::File::userHomeDirectory),
//            "*.wav;*.mp3;*.aiff;*.aif");
//
//        auto flags = juce::FileBrowserComponent::openMode
//                   | juce::FileBrowserComponent::canSelectFiles;
//
//        fChooser->launchAsync(flags, [this](const juce::FileChooser& fc)
//        {
//            auto result = fc.getResult();
//            if (result.existsAsFile())
//            {
//                juce::URL audioURL{ result };
//                player1.loadURL(audioURL);
//            }
//        });
//    }
}

void MainComponent::sliderValueChanged (juce::Slider* slider)
{
//    if (slider == &gainSlider)
//    {
//        player1.setGain(slider-> getValue());
//    }
//    if (slider == &speedSlider)
//    {
//        player1.setSpeed(slider->getValue());  
//    }
//    if (slider == &posSlider)
//    {
//        player1.setPositionRelative(slider->getValue());
//    }
}


