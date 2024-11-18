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





#include "gizmo_pipeline.h"
#include "gizmo_bindings.h"





#include "simple_lemniscate.h"

#include "RoseCurve.h"

#include "CircleShape.h"

#include "GridShape.h"


#include "camera.h"
#include "camera_quaternion.h"

#ifdef JUCE_WINDOWS

#include <Windows.h>
#endif // JUCE_WINDOWS


minalg::float2 lastCursor;

float g_w, g_h;

bool b_left_mouse_down = false;





SimplePipeline* _quard_pipeline = nullptr;


SimplePipeline* _quard_pipeline_line = nullptr;

SimplePipeline* _quard_pipeline_line_strip = nullptr;




GizmoPipeline* gizmo_pipeline = nullptr;
GizmoBindings* gizmo_bindings = nullptr;





//batteries::Camera camera;
//batteries::CameraController cameracontroller;


camera_quaternion camera_quaternion;



tinygizmo::gizmo_context gizmo_ctx;
tinygizmo::gizmo_application_state gizmo_state;

tinygizmo::rigid_transform gizmo_transform;






 
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

    g_w = w;
    g_h = h;
    //cameracontroller.SetCamera(&camera);
    //cameracontroller.Configure({
    //    .mode = (int)batteries::CameraController::Mode::Orbit,
    //    .pitch = 30.0f,
    //    .yaw = 90.0f,
    //    .distance = 10.0f,
    //    });



    gizmo_transform.position = { 1.0f, 1.0f, 0.0f };
    gizmo_transform.orientation = { 0.0f, 0.0f, 0.0f, 1.0f };
    gizmo_transform.scale = { 1.0f, 1.0f, 1.0f };

    camera_quaternion.perspective();

    gizmo_ctx.render = [&](const tinygizmo::geometry_mesh& mesh) {

        if (mesh.vertices.size() > 0 && mesh.triangles.size() > 0)
        {
            // 更新绑定
            gizmo_bindings->Update(mesh.vertices, mesh.triangles);

            // 应用管线
            sg_apply_pipeline(gizmo_pipeline->pipeline);

            // 设置 Uniform
            gizmo_vs_params_t vs_params;
            vs_params.view_proj = camera_quaternion.projectionViewMatrix();// camera.Projection(g_w, g_h)* camera.View(); // 使用您的 Camera 类

            sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));

            // 应用绑定
            sg_apply_bindings(gizmo_bindings->bindings);

            // 绘制
            sg_draw(0, gizmo_bindings->GetNumElements(), 1);
        }

        };










    sg_logger logger = {
          .func = my_log,  
          .user_data = NULL
    };


 
    sg_desc desc = {
     .logger = logger 
    };
    sg_setup(&desc);
    assert(sg_isvalid());  // 确保 Sokol 已经初始化



    {
        _quard_pipeline = new SimplePipeline();

        _quard_pipeline_line_strip = new SimplePipeline(sg_primitive_type::SG_PRIMITIVETYPE_LINE_STRIP);

        _quard_pipeline_line = new SimplePipeline(sg_primitive_type::SG_PRIMITIVETYPE_LINES);
    }


    {
        gizmo_pipeline = new GizmoPipeline();
        gizmo_bindings = new GizmoBindings();
    }





}



void _sg_shutdown() 
{
 
    SimpleLemniscate::Instance().release();
    GridShape::Instance().release();


    gizmo_bindings->release();
    gizmo_pipeline->release();



    delete _quard_pipeline;
    delete _quard_pipeline_line_strip;



    delete gizmo_bindings;
    delete gizmo_pipeline;
}


struct ray { linalg::aliases::float3 origin; linalg::aliases::float3 direction; };

struct rect
{
    float x0, y0, x1, y1;
    int width() const { return x1 - x0; }
    int height() const { return y1 - y0; }
    linalg::aliases::int2 dims() const { return{ width(), height() }; }
    float aspect_ratio() const { return (float)width() / height(); }
};




ray get_ray_from_pixel(const linalg::aliases::float2& pixel, const rect& viewport, class camera_quaternion& camera_quaternion)
{
    const float x = 2 * (pixel.x - viewport.x0) / viewport.width() - 1, y = 1 - 2 * (pixel.y - viewport.y0) / viewport.height();


    glm::mat4 pv = camera_quaternion.projectionMatrix() * camera_quaternion.viewMatrix();

    linalg::aliases::float4x4 pv_linalg(
        linalg::aliases::float4(pv[0][0], pv[0][1], pv[0][2], pv[0][3]),
        linalg::aliases::float4(pv[1][0], pv[1][1], pv[1][2], pv[1][3]),
        linalg::aliases::float4(pv[2][0], pv[2][1], pv[2][2], pv[2][3]),
        linalg::aliases::float4(pv[3][0], pv[3][1], pv[3][2], pv[3][3])
    );


    const linalg::aliases::float4x4 inv_view_proj = linalg::inverse(pv_linalg);
    const linalg::aliases::float4 p0 = mul(inv_view_proj, linalg::aliases::float4(x, y, -1, 1)), p1 = mul(inv_view_proj, linalg::aliases::float4(x, y, +1, 1));
    return{ {camera_quaternion.position.x, camera_quaternion.position.y, camera_quaternion.position.z}, p1.xyz() * p0.w - p0.xyz() * p1.w};
}





void _sg_render(int w, int h)
{

    g_w = w;
    g_h = h;


 /*   camera_quaternion.perspective();*/
    const auto view_proj = camera_quaternion.projectionMatrix() * camera_quaternion.viewMatrix(); //camera.Projection(g_w, g_h) * camera.View();
    //const auto view_proj = camera.Projection(g_w, g_h)* camera.View();

    const vs_params_t vs_blinnphong_params = {
        .view_proj = view_proj,

    };


    {
        // 更新 gizmo_state
 // 1. 确保视口大小正确
        gizmo_state.viewport_size = { (float)w, (float)h };

        // 2. 更新相机参数
        gizmo_state.cam.yfov = camera_quaternion.fovy; // 确保 yfov 以弧度为单位
        gizmo_state.cam.near_clip = camera_quaternion.zNear;
        gizmo_state.cam.far_clip = camera_quaternion.zFar;
        gizmo_state.cam.position = { camera_quaternion.position.x, camera_quaternion.position.y, camera_quaternion.position.z };


        //glm::mat4 view_matrix = camera.View();


        auto cameraOrientation = camera_quaternion.orient(0, 0);

       

        gizmo_state.cam.orientation = minalg::float4(cameraOrientation.x, cameraOrientation.y, cameraOrientation.z, cameraOrientation.w);


        gizmo_state.ray_origin = { camera_quaternion.position.x, camera_quaternion.position.y, camera_quaternion.position.z };


        const auto rayDir = get_ray_from_pixel({ lastCursor.x, lastCursor.y }, { 0.f, 0.f, g_w, g_h }, camera_quaternion).direction;

        gizmo_state.ray_direction = minalg::float3(rayDir.x, rayDir.y, rayDir.z);

  
 
        gizmo_state.mouse_left = b_left_mouse_down;
 

        static auto itransform_mode = tinygizmo::transform_mode::none;

        if (gizmo_ctx.get_mode() == tinygizmo::transform_mode::translate && itransform_mode != tinygizmo::transform_mode::translate)
        {
            itransform_mode = tinygizmo::transform_mode::translate;
            printf("\n translate \n");
        }
        if (gizmo_ctx.get_mode() == tinygizmo::transform_mode::rotate && itransform_mode != tinygizmo::transform_mode::rotate)
        {
            itransform_mode = tinygizmo::transform_mode::rotate;
            printf("\n rotate \n");
        }
        if (gizmo_ctx.get_mode() == tinygizmo::transform_mode::scale && itransform_mode != tinygizmo::transform_mode::scale)
        {
            itransform_mode = tinygizmo::transform_mode::scale;
            printf("\n scale \n");
        }


        gizmo_ctx.update(gizmo_state);
 




    }


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

            
            if(1)
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

            if (1)
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

            if(1)
            {
                sg_apply_pipeline(_quard_pipeline_line_strip->pipeline);
                sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_blinnphong_params));
                {

                    std::vector<glm::vec3> translations = {
                        glm::vec3(-5.0f, 0.0f, 0.0f), // Translation: x = -5, y = 0, z = 0
                        glm::vec3(5.0f, 0.0f, 0.0f), // Translation: x =  5, y = 0, z = 0
                        glm::vec3(5.0f, 5.0f, 0.0f)  // Translation: x =  5, y = 5, z = 0
                    };

                    std::vector<glm::vec3> scales = {
                        glm::vec3(1.0f, 1.0f, 1.0f), // Scale: no scaling
                        glm::vec3(2.0f, 2.0f, 1.0f), // Scale: 2x in x and y
                        glm::vec3(0.5f, 0.5f, 1.0f)  // Scale: 0.5x in x and y                    
                    };

                    CircleShape::Instance().SetTranslations(translations, scales);
                }
                sg_apply_bindings(CircleShape::Instance().GetBindings());

                sg_draw(0, CircleShape::Instance().GetNumElements(), CircleShape::Instance().GetNumInstances());
            }


            {


                if (tinygizmo::transform_gizmo("Gizmo", gizmo_ctx, gizmo_transform)) {
                    // 当 Gizmo 被交互时，可以在这里更新您的对象变换
                    // 例如，打印新的位置
                    printf("Gizmo moved to position: %f, %f, %f\n",
                        gizmo_transform.position.x,
                        gizmo_transform.position.y,
                        gizmo_transform.position.z);
                }

                gizmo_ctx.draw();

            }
   

            
            sg_end_pass();
        }


        sg_commit();

}


// sokol-shdc --input loadpng-sapp.glsl --output loadpng-sapp.glsl.h --slang glsl330