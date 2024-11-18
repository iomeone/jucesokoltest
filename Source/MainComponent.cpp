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

#include "camera.h"
#include "tiny-gizmo.hpp"
extern batteries::Camera camera;
extern batteries::CameraController cameracontroller;
extern minalg::float2 lastCursor;
extern bool b_left_mouse_down;

//==============================================================================
MainComponent::MainComponent()
{

    this->openGLContext.setComponentPaintingEnabled(true);

    addAndMakeVisible(button);
    button.setButtonText("Click Me");

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (1280, 800);
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

    openGLContext.setSwapInterval(1);
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

    //openGLContext.bind



    //openGLContext.extensions.glBindFramebuffer(juce::gl::GL_FRAMEBUFFER, openGLContext.getFrameBufferID());

    //juce::gl::glClear(juce::gl::GL_COLOR_BUFFER_BIT | juce::gl::GL_DEPTH_BUFFER_BIT | juce::gl::GL_STENCIL_BUFFER_BIT);


    cameracontroller.Update(0.0166666675);
    _sg_render(getWidth(), getHeight());

    //openGLContext.extensions.glBindFramebuffer(juce::gl::GL_FRAMEBUFFER, 0);



    //juce::gl::glEnable(juce::gl::GL_DEPTH_TEST);
    //juce::gl::glDisable(juce::gl::GL_BLEND);
  


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

    button.setBounds(10, 10, 100, 40);
}



bool MainComponent::keyPressed(const juce::KeyPress& key)
{
   
    TKeyEvent e;
    e.type = e_key_down;
    e.key_code = key.getKeyCode();
        
    keyArray[e.key_code] = e;

    cameracontroller.Event(e);
    return false;

}

bool MainComponent::keyStateChanged(bool isKeyDown)
{
    if (!isKeyDown)
    {
        for (auto& keyEvent : keyArray)
        {

            if (keyEvent.type == e_key_down)
            {
                if (!juce::KeyPress::isKeyCurrentlyDown(keyEvent.key_code))
                {
                    keyEvent.type = e_key_up;
                    cameracontroller.Event(keyEvent);
                }
            }


        }
    }


    return false;
}

void MainComponent::mouseMove(const juce::MouseEvent& event)
{
    TKeyEvent e;
    e.type = e_mouse_move;

    if (isFirstMouseMove)
    {
        previousMousePosition = event.position;
        isFirstMouseMove = false;
    }

    e.mouse_dx = event.position.getX() - previousMousePosition.getX();
    e.mouse_dy = event.position.getY() - previousMousePosition.getY();

    previousMousePosition = event.position;


    lastCursor.x = event.position.x;
    lastCursor.y = event.position.y;

    //cameracontroller.Event(e);
}

void MainComponent::mouseDown(const juce::MouseEvent& event)
{
   
    TKeyEvent e;
    e.type = e_mouse_down;


    if (event.mods.isLeftButtonDown())
    {
        b_left_mouse_down = true;
        e.mouse_button = e_mouse_left;
    }
    else if (event.mods.isMiddleButtonDown())
        e.mouse_button = e_mouse_middle;
    else if (event.mods.isRightButtonDown())
        e.mouse_button = e_mouse_right;


    previousMousePosition = event.position;

    lastCursor.x = event.position.x;
    lastCursor.y = event.position.y;

    isFirstMouseMove = false;

    cameracontroller.Event(e);
}

void MainComponent::mouseUp(const juce::MouseEvent& event)
{
    TKeyEvent e;
    e.type = e_mouse_up;

    
    if (event.mods.isLeftButtonDown())
    {
        e.mouse_button = e_mouse_left;
        b_left_mouse_down = false;
    }
    else if (event.mods.isMiddleButtonDown())
        e.mouse_button = e_mouse_middle;
    else if (event.mods.isRightButtonDown())
        e.mouse_button = e_mouse_right;
    else
        e.mouse_button = e_mouse_none; 


    isFirstMouseMove = true;

    cameracontroller.Event(e);
}

void MainComponent::mouseDrag(const juce::MouseEvent& event)
{
    TKeyEvent e;
    e.type = e_mouse_move;

    if (isFirstMouseMove)
    {
        previousMousePosition = event.position;
        isFirstMouseMove = false;
    }

    e.mouse_dx = event.position.getX() - previousMousePosition.getX();
    e.mouse_dy = event.position.getY() - previousMousePosition.getY();

    previousMousePosition = event.position;

    //cameracontroller.Event(e);
}

void MainComponent::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    TKeyEvent e;
    e.type = e_mouse_scroll;
    e.scroll_y = wheel.deltaY; 

    cameracontroller.Event(e);
}
