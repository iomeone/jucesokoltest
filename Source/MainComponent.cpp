#include "MainComponent.h"


//using namespace juce::gl;

//#define SOKOL_EXTERN
//#define SOKOL_IMPL
//#define SOKOL_GLCORE
//#define SOKOL_EXTERNAL_GL_LOADER
//#include "sokol_gfx.h"


//static struct {
//    sg_pass_action pass_action;
//}state;

#include "render.h"

//==============================================================================
MainComponent::MainComponent()
{

    this->openGLContext.setComponentPaintingEnabled(false);

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
}

MainComponent::~MainComponent()
{
    // This shuts down the GL system and stops the rendering calls.
    shutdownOpenGL();
}

//==============================================================================

/*
Windows:
set openGLContext.setSwapInterval(0) in void initialise() override; on windows , then fps will be as fast as it can


Mac:
So my simple fix would be comment out the code repaintEvent.wait (-1);

           #if JUCE_MAC
            if (cvDisplayLinkWrapper != nullptr)
            {
           //     repaintEvent.wait (-1);
                renderFrame();
            }
            else
           #endif
*/
void MainComponent::initialise()
{
    // Initialise GL objects for rendering here.

    openGLContext.setSwapInterval(0);
    _sg_initialize(getWidth(), getHeight(), fontMap);


     

}

void MainComponent::shutdown()
{
    // Free any GL objects created for rendering here.
    _sg_shutdown();
}

void MainComponent::render()
{
    // This clears the context with a black background.
    //juce::OpenGLHelpers::clear (juce::Colours::black);


    _sg_render(getWidth(), getHeight());

    if (_fps.incFrameCount())
    {
        printf("FPS: %d\n", _fps.fps);
    }
    // Add your rendering code here...
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // You can add your component specific drawing code here!
    // This will draw over the top of the openGL background.
}


 
void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.


}
