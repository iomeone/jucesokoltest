#include "MainComponent.h"


using namespace juce::gl;

#define SOKOL_EXTERN
#define SOKOL_IMPL
#define SOKOL_GLCORE
#define SOKOL_EXTERNAL_GL_LOADER
#include "sokol_gfx.h"


static struct {
    sg_pass_action pass_action;
}state;



//==============================================================================
MainComponent::MainComponent()
{
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
void MainComponent::initialise()
{
    // Initialise GL objects for rendering here.

    {
        sg_desc desc{};
        sg_setup(&desc);
        state.pass_action = {};
        state.pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
        auto c = juce::Colours::lightblue;
        state.pass_action.colors[0].clear_value = { c.getFloatRed(), c.getFloatGreen(), c.getFloatBlue(), c.getFloatAlpha() };
    }

}

void MainComponent::shutdown()
{
    // Free any GL objects created for rendering here.
    sg_shutdown();
}

void MainComponent::render()
{
    // This clears the context with a black background.
    //juce::OpenGLHelpers::clear (juce::Colours::black);



    sg_begin_pass({ .action = state.pass_action , 
                    .swapchain = {.width = getWidth(), .height = getHeight()}
                  });

 
    sg_end_pass();
    sg_commit();

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
