#include "render.h"


#include "sokol_gfx.h"

#include "flecs.h"
#include <cglm.h>

#include <functional>

#include <string>
#include <sstream>

#include <chrono>

#include "quad.h"

#include "pos_color_pipeline.h"

#include "simple_lemniscate.h"

#include "RoseCurve.h"

#include "CircleShape.h"

#include "GridShape.h"


#include "camera.h"


#ifdef JUCE_WINDOWS

#include <Windows.h>
#endif // JUCE_WINDOWS





SimplePipeline* _quard_pipeline = nullptr;


SimplePipeline* _quard_pipeline_line = nullptr;

SimplePipeline* _quard_pipeline_line_strip = nullptr;








batteries::Camera camera;
batteries::CameraController cameracontroller;






 
void my_log(const char* tag, uint32_t log_level, uint32_t log_item_id,
    const char* message_or_null, uint32_t line_nr,
    const char* filename_or_null, void* user_data) {


    const char* log_level_str = "INFO";
    if (log_level == 0) log_level_str = "PANIC";
    else if (log_level == 1) log_level_str = "ERROR";
    else if (log_level == 2) log_level_str = "WARNING";

    printf("[%s] Level: %s, Item ID: %d, Message: %s, Line: %d, File: %s\n",
        tag,
        log_level_str,
        log_item_id,
        message_or_null ? message_or_null : "No message",
        line_nr,
        filename_or_null ? filename_or_null : "No file");
}

void _sg_initialize(int w, int h, const std::map<std::string, std::pair<size_t, std::vector<unsigned char>>>& fontMap)
{

    cameracontroller.SetCamera(&camera);
    //cameracontroller.Configure({
    //    .mode = (int)batteries::CameraController::Mode::Orbit,
    //    .pitch = 30.0f,
    //    .yaw = 90.0f,
    //    .distance = 10.0f,
    //    });





    sg_logger logger = {
          .func = my_log,  
          .user_data = NULL
    };


 
    sg_desc desc = {
     .logger = logger 
    };
    sg_setup(&desc);
    assert(sg_isvalid());  // 确保 Sokol 已经初始化


    _quard_pipeline = new SimplePipeline();

    _quard_pipeline_line_strip = new SimplePipeline(sg_primitive_type::SG_PRIMITIVETYPE_LINE_STRIP);


    _quard_pipeline_line = new SimplePipeline(sg_primitive_type::SG_PRIMITIVETYPE_LINES);

}



void _sg_shutdown() 
{
 
    SimpleLemniscate::Instance().release();
    GridShape::Instance().release();
    delete _quard_pipeline;
    delete _quard_pipeline_line_strip;
}




void _sg_render(int w, int h)
{



    const auto view_proj = camera.Projection() * camera.View();


    const vs_params_t vs_blinnphong_params = {
        .view_proj = view_proj,

    };



    sg_reset_state_cache();
        {
            sg_pass pass = {
                            .action = {
                                   .colors = { {.load_action = SG_LOADACTION_CLEAR ,
                                                .clear_value = {0.0f, .0f, .0f, 1.0f }} }
                            },
                            .swapchain = {.width = w, .height = h }
            };

            sg_begin_pass(pass);

            

            {
                sg_apply_pipeline(_quard_pipeline->pipeline);
                sg_apply_uniforms(SG_SHADERSTAGE_VS,0, SG_RANGE(vs_blinnphong_params));
                sg_apply_bindings(SimpleQuad::Instance().GetBindings());
                sg_draw(0, SimpleQuad::Instance().GetNumElements(), 3);
            }



            //{
            //    sg_apply_pipeline(_quard_pipeline_line_strip->pipeline_use_index);
            //    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_blinnphong_params));
            //    sg_apply_bindings(SimpleQuad::Instance().GetBindings_use_index());
            //    sg_draw(0, SimpleQuad::Instance().GetNumElements(), 1);
            //}

            if (0)
            {
                sg_apply_pipeline(_quard_pipeline->pipeline_use_index);
                sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_blinnphong_params));
                sg_apply_bindings(SimpleLemniscate::Instance().GetBindings());
                sg_draw(0, SimpleLemniscate::Instance().GetNumElements(), 1);
            }

            if (0)
            {
                sg_apply_pipeline(_quard_pipeline_line->pipeline);
                sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_blinnphong_params));
                sg_apply_bindings(GridShape::Instance().GetBindings());
                sg_draw(0, GridShape::Instance().GetNumElements(), 1);
            }


            //{
            //    sg_apply_pipeline(_quard_pipeline_line_strip->pipeline_use_index);
            //    sg_apply_bindings(SimpleLemniscate::Instance().GetBindings());
            //    sg_draw(0, SimpleLemniscate::Instance().GetNumElements(), 1);
            //}

            if(0)
            {
                sg_apply_pipeline(_quard_pipeline_line_strip->pipeline);
                sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_blinnphong_params));
                sg_apply_bindings(RoseCurve::Instance().GetBindings());
                sg_draw(0, RoseCurve::Instance().GetNumElements(), 1);
            }


            //{
            //    sg_apply_pipeline(_quard_pipeline_line_strip->pipeline);
            //    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_blinnphong_params));
            //    sg_apply_bindings(RoseCurve::Instance().GetBindings());
            //    sg_draw(0, RoseCurve::Instance().GetNumElements(), 1);
            //}

            if(0)
            {
                sg_apply_pipeline(_quard_pipeline_line_strip->pipeline);
                sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_blinnphong_params));
                sg_apply_bindings(CircleShape::Instance().GetBindings());
                sg_draw(0, CircleShape::Instance().GetNumElements(), 1);
            }
   

            
            sg_end_pass();
        }


        sg_commit();

}


// sokol-shdc --input loadpng-sapp.glsl --output loadpng-sapp.glsl.h --slang glsl330