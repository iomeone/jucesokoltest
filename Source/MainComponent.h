#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/



class FPS
{
public:
    int fps = 0;


    FPS()
    {
        lastTime = juce::Time::getMillisecondCounter();
    }
    bool incFrameCount()
    {
        frameCount++;
        juce::uint32 currentTime = juce::Time::getMillisecondCounter();
        if (currentTime - lastTime >= 1000)
        {
            fps = frameCount;
            frameCount = 0;
            lastTime = currentTime;
            return true;
        }
        return false;
    }

private:
    juce::uint32 lastTime; // ms
    int frameCount{ 0 };
};






class MainComponent  : public juce::OpenGLAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void initialise() override;
    void shutdown() override;
    void render() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    FPS _fps;

    std::map<std::string, std::pair<size_t, std::vector<unsigned char>>> fontMap;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
