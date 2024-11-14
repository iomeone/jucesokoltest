#pragma once

#include <JuceHeader.h>
#include "Event.h"
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



    bool keyPressed(const juce::KeyPress& key) override;

    bool keyStateChanged(bool isKeyDown) override;


    void mouseMove(const juce::MouseEvent& event)override;

    void mouseDown(const juce::MouseEvent& event)override;
    void mouseUp(const juce::MouseEvent& event)override;

    void mouseDrag(const juce::MouseEvent& event)override;

    void mouseWheelMove(const juce::MouseEvent& event,
        const juce::MouseWheelDetails& wheel) override;



private:
    //==============================================================================
    // Your private member variables go here...
    FPS _fps;

    std::array<TKeyEvent, 256> keyArray;

    std::map<std::string, std::pair<size_t, std::vector<unsigned char>>> fontMap;


    juce::TextButton button{ "Click Me" };


    juce::Point<float> previousMousePosition;
    bool isFirstMouseMove = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
