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
    addAndMakeVisible(loadButton);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);
    
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
    
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    player1.getNextAudioBlock(bufferToFill);
}

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
    double rowH = getHeight() / 6 ;
    double rowW = getWidth();
    playButton.setBounds(0, 0, rowW, rowH);
    stopButton.setBounds(0, rowH, rowW, rowH);
    gainSlider.setBounds(0, 2 * rowH, rowW, rowH);
    speedSlider.setBounds(0, 3 * rowH, rowW, rowH);
    posSlider.setBounds(0, 4 * rowH, rowW, rowH);
    loadButton.setBounds(0, 5 * rowH, rowW, rowH);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        player1.start();
    }
    if (button == &stopButton)
    {
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
        player1.setGain(slider-> getValue());
    }
    if (slider == &speedSlider)
    {
        player1.setSpeed(slider->getValue());  
    }
    if (slider == &posSlider)
    {
        player1.setPositionRelative(slider->getValue());
    }
}


