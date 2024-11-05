
#include "render.h"


#include "sokol_gfx.h"

#include "flecs.h"
#include <cglm.h>

#include <functional>

#include <string>
#include <sstream>

#include <chrono>

#ifdef JUCE_WINDOWS

#include <Windows.h>
#endif // JUCE_WINDOWS





#define SOKOL_MAX_EFFECT_INPUTS (8)
#define SOKOL_MAX_EFFECT_PASS (8)
#define SOKOL_MAX_EFFECT_PARAMS (32)

typedef struct sokol_pass_input_t {
    const char* name;
    int id;
} sokol_pass_input_t;




typedef struct sokol_pass_desc_t {
    int32_t width;
    int32_t height;
    const char* shader;
    sg_pixel_format color_format;
    sokol_pass_input_t inputs[SOKOL_MAX_EFFECT_INPUTS];
} sokol_pass_desc_t;






typedef struct sokol_offscreen_pass_t {
    sg_pass_action pass_action;
    sg_attachments attachment;
    sg_pipeline pip;
    sg_image depth_target;
    sg_image color_target;

    sg_sampler smp;
} sokol_offscreen_pass_t;


typedef struct sokol_pass_t {
    sokol_offscreen_pass_t sub_offscreen_pass;
    int32_t input_count;
    int32_t param_count;
    int32_t inputs[SOKOL_MAX_EFFECT_INPUTS];
    float params[SOKOL_MAX_EFFECT_PARAMS];
    int32_t width;
    int32_t height;
} sokol_fx_pass_t;






typedef struct SokolEffect {
    sokol_pass_t pass[SOKOL_MAX_EFFECT_PASS];
    int32_t pass_count;
} SokolEffect;


static
const char* shd_threshold =
"float thresh = 1.0;\n"
"vec4 c = texture(tex, uv);\n"
"c.r = max(c.r - thresh, 0);\n"
"c.g = max(c.g - thresh, 0);\n"
"c.b = max(c.b - thresh, 0);\n"
"frag_color = c;\n";

static
const char* shd_h_blur =
"float kernel = 50.0;\n"
"vec4 sum = vec4(0.0);\n"
"float width = 800;\n"
"float height = 600;\n"
"float x = uv.x;\n"
"float y = uv.y;\n"
"float i, g;\n"

"kernel = kernel / width;\n"
"kernel = kernel / (width / height);\n"
"for(i=-kernel; i<=kernel; i+=1 / width) {\n"
"	g = i / kernel;\n"
"	g *= g;\n"
"	sum += texture(tex, vec2(x + i, y)) * exp(-(g) * 5);\n"
"}\n"
"frag_color = sum / 20;\n";

static
const char* shd_v_blur =
"float kernel = 50.0;\n"
"vec4 sum = vec4(0.0);\n"
"float width = 800;\n"
"float height = 600;\n"
"float x = uv.x;\n"
"float y = uv.y;\n"
"float i, g;\n"

"kernel = kernel / width;\n"
"for(i=-kernel; i<=kernel; i+=1 / width) {\n"
"	g = i / kernel;\n"
"	g *= g;\n"
"	sum += texture(tex, vec2(x, y + i)) * exp(-(g) * 5);\n"
"}\n"
"frag_color = sum / 20;\n";

static
const char* shd_blend =
"frag_color = texture(tex0, uv) + texture(tex1, uv);\n";






struct EcsRgb {
    float r;
    float g;
    float b;
    float a;
};
typedef EcsRgb ecs_rgb_t ;
typedef EcsRgb ecs_rgba_t;



struct EcsSpecular {
    float specular_power;
    float shininess;
};


struct EcsEmissive {
    float value;
};

struct MaterialTag {};


typedef struct SokolMaterial {
    uint16_t material_id;
} SokolMaterial;



struct EcsPosition3
{
    float x;
    float y;
    float z;
};



struct EcsBox{
    float width;
    float height;
    float depth;
};

struct EcsCamera
{
    vec3 position;
    vec3 lookat;
    vec3 up;
    float fov;
    float near_;
    float far_;
    bool ortho;

    EcsCamera()
    {
        this->set_position(0, 0, 0);
        this->set_lookat(0, 1, 1);
        this->set_up(0, -1, 0);
        this->set_fov(30);
        this->near_ = 0.1f;
        this->far_ = 100;
        this->ortho = false;
    }


    void set_position(float x, float y, float z) {
        this->position[0] = x;
        this->position[1] = y;
        this->position[2] = z;
    }

    void set_lookat(float x, float y, float z) {
        this->lookat[0] = x;
        this->lookat[1] = y;
        this->lookat[2] = z;
    }

    void set_up(float x, float y, float z) {
        this->up[0] = x;
        this->up[1] = y;
        this->up[2] = z;
    }

    void set_fov(float value) {
        this->fov = value;
    }
};


struct EcsDirectionalLight {
    vec3 position;
    vec3 direction;
    vec3 color;


    EcsDirectionalLight()
    {
        this->set_position(0, 0, 0);
        this->set_direction(0, 1, 1);
        this->set_color(1, 1, 1);
    }


    void set_position(float x, float y, float z) {
        this->position[0] = x;
        this->position[1] = y;
        this->position[2] = z;
    }

    void set_direction(float x, float y, float z) {
        this->direction[0] = x;
        this->direction[1] = y;
        this->direction[2] = z;
    }

    void set_color(float r, float g, float b) {
        this->color[0] = r;
        this->color[1] = g;
        this->color[2] = b;
    }
};

struct EcsRectangle {
    float width;
    float height;
};


 struct EcsTransform3 {
    mat4 value;
} ;







 typedef struct vs_uniforms_t {
     mat4 mat_vp;
 } vs_uniforms_t;

 typedef struct fs_uniforms_t {
     vec3 light_ambient;
     vec3 light_direction;
     vec3 light_color;
     vec3 eye_pos;
 } fs_uniforms_t;




#define FS_MAX_MATERIALS (255)

 struct vs_material_t {
     float specular_power;
     float shininess;
     float emissive;
 };

 struct vs_materials_t {
     vs_material_t array[FS_MAX_MATERIALS];
 };









 struct sokol_render_state_t {


     //const EcsDirectionalLight* light;
     //const EcsCamera* camera;
     sokol_render_state_t()
     {
         this->ambient_light.r = 0.4;
         this->ambient_light.g = 0.4;
         this->ambient_light.b = 1.0;
     }

     flecs::entity light;
     flecs::entity camera;

     ecs_rgb_t ambient_light;
     int32_t width;
     int32_t height;

     //sokol_resources_t* resources;
     //sokol_global_uniforms_t uniforms;
     sg_image shadow_map;
 };





struct SokolCanvas {
    sg_pass_action pass_action;
    sg_pipeline pip;

    EcsRgb  background_color;

    

    sg_image offscreen_tex;
    sg_image offscreen_depth_tex;
    sg_pass offscreen_pass;
    sg_pass_action tex_pass_action;
    sg_buffer offscreen_quad;

    SokolEffect fx_bloom;

    sg_pipeline tex_pip;

    sg_sampler smp;

};

typedef SokolCanvas EcsCanvas;




struct RectangleTag {};
struct BoxTag {};


struct SokolBuffer {
    // GPU buffers
    sg_buffer vertex_buffer;        // Geometry (static)
    sg_buffer normal_buffer;
    sg_buffer index_buffer;         // Indices (static)
    sg_buffer color_buffer;         // Color (per instance)
    sg_buffer transform_buffer;     // Transform (per instance)

    sg_buffer material_buffer;      // Material IDs (per instance)


    // Application-cached buffers
    EcsRgb* colors;
    mat4* transforms;
    float* materials;

    // Number of instances
    int32_t instance_count;


    int32_t instance_capacity;

    // Number of indices
    int32_t index_count;

    // Constructor
    SokolBuffer()
        : colors(nullptr), transforms(nullptr), materials(nullptr), instance_count(0), instance_capacity(0), index_count(0) {
        vertex_buffer = { 0 };
        index_buffer = { 0 };
        color_buffer = { 0 };
        transform_buffer = { 0 };
        material_buffer = { 0 };
    }

    void releaseBuffer()
    {

        if (color_buffer.id != SG_INVALID_ID) {
            sg_destroy_buffer(color_buffer);
        }
        if (transform_buffer.id != SG_INVALID_ID) {
            sg_destroy_buffer(transform_buffer);
        }

        if (material_buffer.id != SG_INVALID_ID) {
            sg_destroy_buffer(material_buffer);
        }

    }

    ~SokolBuffer() {
        if(colors)
        ecs_os_free(colors);
        if(transforms)
        ecs_os_free(transforms);
        if(materials)
        ecs_os_free(materials);
    }


};




std::string build_fs_shader(const sokol_pass_desc_t& pass) {
    std::ostringstream fs_shader;

    fs_shader << "#version 330\n"
        << "out vec4 frag_color;\n"
        << "in vec2 uv;\n";

    int i = 0;
    while (pass.inputs[i].name != nullptr) {
        fs_shader << "uniform sampler2D " << pass.inputs[i].name << ";\n";
        i++;
    }

    fs_shader << "void main() {\n";
    fs_shader << pass.shader << "\n";
    fs_shader << "}\n";

    return fs_shader.str();
}




const char* vs_fx_shader =
"#version 330\n"
"layout(location=0) in vec4 v_position;\n"
"layout(location=1) in vec2 v_uv;\n"
"out vec2 uv;\n"
"void main() {\n"
"  gl_Position = v_position;\n"
"  uv = v_uv;\n"
"}\n";


char fs_shader[4096] = { 0 };
int sokol_effect_add_pass(
    SokolEffect* fx,
    sokol_pass_desc_t pass_desc)
{
    sokol_pass_t* pass = &fx->pass[fx->pass_count++];

     auto _fs_shader = build_fs_shader(pass_desc);
    strcpy(fs_shader, _fs_shader.c_str());
    /* 创建着色器 */
    sg_shader_desc shd_desc = {};
    shd_desc.vs.source = vs_fx_shader;
    shd_desc.fs.source = fs_shader;

    sokol_pass_input_t* input;
    int i = 0;
    while (true) {
        input = &pass_desc.inputs[i];
        if (!input->name) {
            break;
        }
        //shd_desc.fs.images[i].name = input->name;
        shd_desc.fs.images[i].used = true;
        shd_desc.fs.images[i].image_type = SG_IMAGETYPE_2D;
        shd_desc.fs.images[i].sample_type = _SG_IMAGESAMPLETYPE_DEFAULT;


        shd_desc.fs.samplers[i].used = true;
        shd_desc.fs.samplers[i].sampler_type = SG_SAMPLERTYPE_FILTERING;


        shd_desc.fs.image_sampler_pairs[i].used = true;
        shd_desc.fs.image_sampler_pairs[i].image_slot = i;
        shd_desc.fs.image_sampler_pairs[i].sampler_slot = i;
        shd_desc.fs.image_sampler_pairs[i].glsl_name = input->name;



        pass->inputs[i] = input->id;
        i++;
    }
    sg_shader shd = sg_make_shader(&shd_desc);
    pass->input_count = i;

    /* 创建渲染管线 */
    sg_pipeline_desc pip_desc = {};
    pip_desc.shader = shd;
    pip_desc.layout.attrs[0].buffer_index = 0;
    pip_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
    pip_desc.layout.attrs[1].buffer_index = 0;
    pip_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2;
    pip_desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;
    pip_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    pip_desc.depth.write_enabled = true;
    pip_desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;

    pass->sub_offscreen_pass.pip = sg_make_pipeline(&pip_desc);

    /* 创建颜色和深度纹理 */
    sg_image_desc color_target_desc = {};
    color_target_desc.render_target = true;
    color_target_desc.width = pass_desc.width;
    color_target_desc.height = pass_desc.height;
    color_target_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    color_target_desc.label = "color-target";
    pass->sub_offscreen_pass.color_target = sg_make_image(&color_target_desc);

    sg_image_desc depth_target_desc = {};
    depth_target_desc.render_target = true;
    depth_target_desc.width = pass_desc.width;
    depth_target_desc.height = pass_desc.height;
    depth_target_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
    depth_target_desc.label = "depth-target";
    pass->sub_offscreen_pass.depth_target = sg_make_image(&depth_target_desc);

    /* 创建渲染通道 */
    sg_attachments_desc pass_sg_desc = {};
    pass_sg_desc.colors[0].image = pass->sub_offscreen_pass.color_target;
    pass_sg_desc.depth_stencil.image = pass->sub_offscreen_pass.depth_target;
    pass_sg_desc.label = "fx-pass";

    pass->sub_offscreen_pass.attachment = sg_make_attachments(&pass_sg_desc);



    sg_sampler_desc sampler_desc = {};
    sampler_desc.min_filter = SG_FILTER_LINEAR;
    sampler_desc.mag_filter = SG_FILTER_LINEAR;
 

    pass->sub_offscreen_pass.smp = sg_make_sampler(&sampler_desc);

    

    return fx->pass_count - 1;
}





/*


typedef struct sokol_pass_desc_t {
    const char* shader;
    int32_t width;
    int32_t height;
    sg_pixel_format color_format;
    sokol_pass_input_t inputs[SOKOL_MAX_EFFECT_INPUTS];
} sokol_pass_desc_t;
*/
 


SokolEffect sokol_init_bloom(int width, int height) {
    SokolEffect effect;
    int blur_w = width * 0.2;
    int blur_h = height * 0.2;
    sokol_pass_desc_t t;
    int threshold_pass = sokol_effect_add_pass(&effect, {
        blur_w,
        blur_h,
        shd_threshold,
        _SG_PIXELFORMAT_DEFAULT,
        { {"tex", 0} }
        });

    int blur_h_pass = sokol_effect_add_pass(&effect, {
        blur_w,
        blur_h,
        shd_h_blur,
        _SG_PIXELFORMAT_DEFAULT,
        { {"tex", threshold_pass} }
        });

    int blur_v_pass = sokol_effect_add_pass(&effect, {
        blur_w,
        blur_h,
        shd_v_blur,
        _SG_PIXELFORMAT_DEFAULT,
        { {"tex", blur_h_pass} }
        });

    sokol_effect_add_pass(&effect, {
        width,
        height,
        shd_blend,
        _SG_PIXELFORMAT_DEFAULT,
        { {"tex0", 0}, {"tex1", blur_v_pass} }
        });

    return effect;
}


/*
* 
sg_image sokol_effect_run(
    SokolCanvas* sk_canvas,
    SokolEffect* effect,
    sg_image input)


*/




static void effect_pass_draw(
    const SokolCanvas* sk_canvas,
    const SokolEffect* effect,
    const sokol_pass_t* fx_pass,
    sg_image input_0,
    sg_sampler sampler) // 添加 sampler 参数
{

    sg_pass pass = {};
    pass.action = fx_pass->sub_offscreen_pass.pass_action;
    pass.attachments = fx_pass->sub_offscreen_pass.attachment;


    sg_begin_pass(pass);
    sg_apply_pipeline(fx_pass->sub_offscreen_pass.pip);

    // 初始化绑定
    sg_bindings bind = {};
    bind.vertex_buffers[0] = sk_canvas->offscreen_quad;

    // 绑定片段着色器的纹理
    for (int i = 0; i < fx_pass->input_count; i++) {
        int input = fx_pass->inputs[i];
        if (input == 0) {
            bind.fs.images[i] = input_0;  // 使用 fs.images 绑定片段着色器纹理
        }
        else {
            bind.fs.images[i] = effect->pass[input - 1].sub_offscreen_pass.color_target;
        }
        bind.fs.samplers[i] = sampler;  // 使用 fs.samplers 绑定采样器
    }

    sg_apply_bindings(&bind);
    sg_draw(0, 6, 1);
    sg_end_pass();
}




sg_buffer init_quad() {
    float quad_data[] = {
        -1.0f, -1.0f,  0.0f,   0, 0,
         1.0f, -1.0f,  0.0f,   1, 0,
         1.0f,  1.0f,  0.0f,   1, 1,
        -1.0f, -1.0f,  0.0f,   0, 0,
         1.0f,  1.0f,  0.0f,   1, 1,
        -1.0f,  1.0f,  0.0f,   0, 1
    };

    sg_buffer_desc vbuf_desc = {};
    vbuf_desc.size = sizeof(quad_data);
    vbuf_desc.data.ptr = quad_data; // 使用 data.ptr 指定数据指针
    vbuf_desc.data.size = sizeof(quad_data); // 设置数据大小
    vbuf_desc.usage = SG_USAGE_IMMUTABLE;

    return sg_make_buffer(&vbuf_desc);
}


sg_image sokol_effect_run(
    const SokolCanvas* sk_canvas,
    const SokolEffect* effect,
    sg_image input)
{
    int i;
    for (i = 0; i < effect->pass_count; i++) {
        effect_pass_draw(sk_canvas, effect, &effect->pass[i], input, effect->pass[i].sub_offscreen_pass.smp);
    }

    return effect->pass[effect->pass_count - 1].sub_offscreen_pass.color_target;
}



static
sg_pass_action init_pass_action(const EcsCanvas* canvas) {
    ecs_rgb_t bg_color = canvas->background_color;

    sg_pass_action pass_action = {};
    pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;

    pass_action.colors[0].clear_value = { bg_color.r, bg_color.g, bg_color.b, 1.0f };

    return pass_action;
}

 
static
sg_pass_action init_tex_pass_action() {

    sg_pass_action pass_action = {};
    pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    pass_action.colors[0].clear_value = {0,0,0 };
    return pass_action;
}




static sg_pipeline init_tex_pipeline() {
    /* 创建着色器描述 */
    sg_shader_desc shd_desc = {};
    shd_desc.vs.source =
        "#version 330\n"
        "layout(location=0) in vec4 v_position;\n"
        "layout(location=1) in vec2 v_uv;\n"
        "out vec2 uv;\n"
        "void main() {\n"
        "  gl_Position = v_position;\n"
        "  uv = v_uv;\n"
        "}\n";
    shd_desc.fs.source =
        "#version 330\n"
        "uniform sampler2D tex;\n"
        "out vec4 frag_color;\n"
        "in vec2 uv;\n"
        "void main() {\n"
        "  frag_color = texture(tex, uv);\n"
        "}\n";

    // 设置片段着色器的纹理信息
    //shd_desc.fs.images[0].name = "tex";


    {
        shd_desc.fs.images[0].used = true;
        shd_desc.fs.images[0].image_type = SG_IMAGETYPE_2D;
        shd_desc.fs.images[0].sample_type = _SG_IMAGESAMPLETYPE_DEFAULT;


        shd_desc.fs.samplers[0].used = true;
        shd_desc.fs.samplers[0].sampler_type = SG_SAMPLERTYPE_FILTERING;


        shd_desc.fs.image_sampler_pairs[0].used = true;
        shd_desc.fs.image_sampler_pairs[0].image_slot = 0;
        shd_desc.fs.image_sampler_pairs[0].sampler_slot = 0;
        shd_desc.fs.image_sampler_pairs[0].glsl_name = "tex";
    }



    sg_shader shd = sg_make_shader(&shd_desc);

    /* 创建渲染管线描述 */
    sg_pipeline_desc pip_desc = {};
    pip_desc.shader = shd;

    // 配置顶点布局
    pip_desc.layout.attrs[0].buffer_index = 0;
    pip_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3; // 位置
    pip_desc.layout.attrs[1].buffer_index = 0;
    pip_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2; // UV 坐标

    // 深度和模板设置
    pip_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    pip_desc.depth.write_enabled = false;

    // 创建并返回渲染管线
    return sg_make_pipeline(&pip_desc);
}




static
sg_pipeline init_pipeline() {

    sg_shader_desc shader_desc = {};

    // Vertex shader uniform blocks
    shader_desc.vs.uniform_blocks[0].size = sizeof(vs_uniforms_t);
    shader_desc.vs.uniform_blocks[0].uniforms[0].name = "u_mat_vp";
    shader_desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_MAT4;

    // Add second uniform block for materials
    shader_desc.vs.uniform_blocks[1].size = sizeof(vs_materials_t);
    shader_desc.vs.uniform_blocks[1].uniforms[0].name = "u_materials";
    shader_desc.vs.uniform_blocks[1].uniforms[0].type = SG_UNIFORMTYPE_FLOAT3;
    shader_desc.vs.uniform_blocks[1].uniforms[0].array_count = FS_MAX_MATERIALS;

    // Fragment shader uniform blocks
    shader_desc.fs.uniform_blocks[0].size = sizeof(fs_uniforms_t);
    shader_desc.fs.uniform_blocks[0].uniforms[0].name = "u_light_ambient";
    shader_desc.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT3;
    shader_desc.fs.uniform_blocks[0].uniforms[1].name = "u_light_direction";
    shader_desc.fs.uniform_blocks[0].uniforms[1].type = SG_UNIFORMTYPE_FLOAT3;
    shader_desc.fs.uniform_blocks[0].uniforms[2].name = "u_light_color";
    shader_desc.fs.uniform_blocks[0].uniforms[2].type = SG_UNIFORMTYPE_FLOAT3;
    shader_desc.fs.uniform_blocks[0].uniforms[3].name = "u_eye_pos";
    shader_desc.fs.uniform_blocks[0].uniforms[3].type = SG_UNIFORMTYPE_FLOAT3;

    // Vertex shader code
    shader_desc.vs.source =
        "#version 330\n"
        "uniform mat4 u_mat_vp;\n"
        "uniform vec3 u_materials[255];\n"
        "layout(location=0) in vec3 v_position;\n"
        "layout(location=1) in vec3 v_normal;\n"
        "layout(location=2) in vec4 i_color;\n"
        "layout(location=3) in float i_material;\n"
        "layout(location=4) in mat4 i_mat_m;\n"
        "out vec4 position;\n"
        "out vec3 normal;\n"
        "out vec4 color;\n"
        "out vec3 material;\n"
        "out float f_material;\n"
        "void main() {\n"
        "  gl_Position = u_mat_vp * i_mat_m * vec4(v_position, 1.0);\n"
        "  position = i_mat_m * vec4(v_position, 1.0);\n"
        "  normal = mat3(i_mat_m) * v_normal;\n"
        "  color = i_color;\n"
       
        "   uint material_id = uint(i_material);\n"
        "   material = u_materials[material_id];\n"
        "   f_material = i_material;\n"
        "}\n";

    // Fragment shader code
    shader_desc.fs.source =
        "#version 330\n"
        "uniform vec3 u_light_ambient;\n"
        "uniform vec3 u_light_direction;\n"
        "uniform vec3 u_light_color;\n"
        "uniform vec3 u_eye_pos;\n"
        "in vec4 position;\n"
        "in vec3 normal;\n"
        "in vec4 color;\n"
        "in vec3 material;\n"
        "in float f_material;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "  float specular_power = material.x;\n"
        "  float shininess = material.y;\n"
        "  float emissive = material.z;\n"
        "  vec3 l = normalize(u_light_direction);\n"
        "  vec3 n = normalize(normal);\n"
        "  float dot_n_l = max(dot(n, l), 0.0);\n"
        "  vec3 v = normalize(u_eye_pos - position.xyz);\n"
        "  vec3 r = reflect(-l, n);\n"
        "  float r_dot_v = max(dot(r, v), 0.0);\n"
        "  vec4 ambient = vec4(u_light_ambient, 1.0) * color;\n"
        "  vec4 diffuse = vec4(u_light_color, 1.0) * color * dot_n_l;\n"
        "  vec4 specular = vec4(specular_power * pow(r_dot_v, shininess) * dot_n_l * u_light_color, 1.0);\n"
        "  specular = clamp(specular, 0.0, 1.0);\n"
        "  frag_color =  emissive + ambient + diffuse + specular;\n"
        "}\n";

    sg_shader shd = sg_make_shader(&shader_desc);

    // Configure rendering pipeline
    sg_pipeline_desc pipeline_desc = {};
    pipeline_desc.shader = shd;
    pipeline_desc.index_type = SG_INDEXTYPE_UINT16;

    // Configure buffer layouts
    pipeline_desc.layout.buffers[0].stride = sizeof(float) * 3; // Vertex positions
    pipeline_desc.layout.buffers[0].step_func = SG_VERTEXSTEP_PER_VERTEX;

    pipeline_desc.layout.buffers[1].stride = sizeof(float) * 3; // Normals
    pipeline_desc.layout.buffers[1].step_func = SG_VERTEXSTEP_PER_VERTEX;

    pipeline_desc.layout.buffers[2].stride = sizeof(EcsRgb); // Colors
    pipeline_desc.layout.buffers[2].step_func = SG_VERTEXSTEP_PER_INSTANCE;

    pipeline_desc.layout.buffers[3].stride = sizeof(float); // Material IDs
    pipeline_desc.layout.buffers[3].step_func = SG_VERTEXSTEP_PER_INSTANCE;

    pipeline_desc.layout.buffers[4].stride = sizeof(EcsTransform3); // Transforms
    pipeline_desc.layout.buffers[4].step_func = SG_VERTEXSTEP_PER_INSTANCE;

    // Configure vertex attributes
    pipeline_desc.layout.attrs[0].buffer_index = 0; // Position
    pipeline_desc.layout.attrs[0].offset = 0;
    pipeline_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;

    pipeline_desc.layout.attrs[1].buffer_index = 1; // Normal
    pipeline_desc.layout.attrs[1].offset = 0;
    pipeline_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT3;

    pipeline_desc.layout.attrs[2].buffer_index = 2; // Color
    pipeline_desc.layout.attrs[2].offset = 0;
    pipeline_desc.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT4;

    pipeline_desc.layout.attrs[3].buffer_index = 3; // Material ID
    pipeline_desc.layout.attrs[3].offset = 0;
    pipeline_desc.layout.attrs[3].format = SG_VERTEXFORMAT_FLOAT;

    // Transform matrix attributes
    pipeline_desc.layout.attrs[4].buffer_index = 4;
    pipeline_desc.layout.attrs[4].offset = 0;
    pipeline_desc.layout.attrs[4].format = SG_VERTEXFORMAT_FLOAT4;

    pipeline_desc.layout.attrs[5].buffer_index = 4;
    pipeline_desc.layout.attrs[5].offset = 16;
    pipeline_desc.layout.attrs[5].format = SG_VERTEXFORMAT_FLOAT4;

    pipeline_desc.layout.attrs[6].buffer_index = 4;
    pipeline_desc.layout.attrs[6].offset = 32;
    pipeline_desc.layout.attrs[6].format = SG_VERTEXFORMAT_FLOAT4;

    pipeline_desc.layout.attrs[7].buffer_index = 4;
    pipeline_desc.layout.attrs[7].offset = 48;
    pipeline_desc.layout.attrs[7].format = SG_VERTEXFORMAT_FLOAT4;


    pipeline_desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA16F;
    pipeline_desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH;

    // Depth test and backface culling
    pipeline_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    pipeline_desc.depth.write_enabled = true;
    pipeline_desc.cull_mode = SG_CULLMODE_BACK;

    return sg_make_pipeline(&pipeline_desc);
}



   /*

return sg_make_pipeline(&(sg_pipeline_desc) {
    .shader = shd,
        .index_type = SG_INDEXTYPE_UINT16,
        .layout = {
            .buffers = {
                [2] = {.stride = 16, .step_func = SG_VERTEXSTEP_PER_INSTANCE },
                [3] = {.stride = 64, .step_func = SG_VERTEXSTEP_PER_INSTANCE }
            },

            .attrs = {
             
                [0] = {.buffer_index = 0, .offset = 0,  .format = SG_VERTEXFORMAT_FLOAT3 },
                [1] = {.buffer_index = 1, .offset = 0,  .format = SG_VERTEXFORMAT_FLOAT3 },

             
                [2] = {.buffer_index = 2, .offset = 0, .format = SG_VERTEXFORMAT_FLOAT4 },

            
                [3] = {.buffer_index = 3, .offset = 0,  .format = SG_VERTEXFORMAT_FLOAT4 },
                [4] = {.buffer_index = 3, .offset = 16, .format = SG_VERTEXFORMAT_FLOAT4 },
                [5] = {.buffer_index = 3, .offset = 32, .format = SG_VERTEXFORMAT_FLOAT4 },
                [6] = {.buffer_index = 3, .offset = 48, .format = SG_VERTEXFORMAT_FLOAT4 }
            }
    },
        .depth_stencil = {
            .depth_compare_func = SG_COMPAREFUNC_LESS_EQUAL,
            .depth_write_enabled = true
    },
        .rasterizer.cull_mode = SG_CULLMODE_BACK
});
    */

static
void compute_flat_normals(
    vec3* vertices,
    uint16_t* indices,
    int32_t count,
    vec3* normals_out)
{
    int32_t v;
    for (v = 0; v < count; v += 3) {
        vec3 vec1, vec2, normal;
        glm_vec3_sub(vertices[indices[v + 0]], vertices[indices[v + 1]], vec1);
        glm_vec3_sub(vertices[indices[v + 0]], vertices[indices[v + 2]], vec2);
        glm_vec3_crossn(vec2, vec1, normal);

        glm_vec3_copy(normal, normals_out[indices[v + 0]]);
        glm_vec3_copy(normal, normals_out[indices[v + 1]]);
        glm_vec3_copy(normal, normals_out[indices[v + 2]]);
    }
}



static
void init_rect_buffers(flecs::world& ecs) {
    auto rect_buf = ecs.lookup("SokolRectangleBuffer");
    ecs_assert(rect_buf.is_alive(), ECS_INTERNAL_ERROR, NULL);

    auto b = rect_buf.get_mut<SokolBuffer>();
    ecs_assert(b != nullptr, ECS_INTERNAL_ERROR, NULL);

    vec3 vertices[] = {
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { 0.5f,  0.5f, 0.0f},
        {-0.5f,  0.5f, 0.0f}
    };

    uint16_t indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    vec3 normals[6];
    compute_flat_normals(vertices, indices, 6, normals);



    {
        sg_buffer_desc vbuf_desc = {};
        vbuf_desc.size = sizeof(vertices);
        vbuf_desc.data = SG_RANGE(vertices);
        vbuf_desc.usage = SG_USAGE_IMMUTABLE;

        b->vertex_buffer = sg_make_buffer(&vbuf_desc);
    }



    {
        sg_buffer_desc nbuf_desc = {};
        nbuf_desc.size = sizeof(normals);
        nbuf_desc.data = SG_RANGE(normals);
        nbuf_desc.type = SG_BUFFERTYPE_VERTEXBUFFER;
        nbuf_desc.usage = SG_USAGE_IMMUTABLE;

        b->normal_buffer = sg_make_buffer(&nbuf_desc);
    }


    {
        sg_buffer_desc ibuf_desc = {};
        ibuf_desc.size = sizeof(indices);
        ibuf_desc.data = SG_RANGE(indices);
        ibuf_desc.type = SG_BUFFERTYPE_INDEXBUFFER;
        ibuf_desc.usage = SG_USAGE_IMMUTABLE;

        b->index_buffer = sg_make_buffer(&ibuf_desc);
    }


    b->index_count = 6;
}








static
void init_box_buffers(flecs::world& ecs) {
    auto box_buf = ecs.lookup("SokolBoxBuffer");
    ecs_assert(box_buf.is_alive(), ECS_INTERNAL_ERROR, NULL);

    auto b = box_buf.get_mut<SokolBuffer>();
    ecs_assert(b != nullptr, ECS_INTERNAL_ERROR, NULL);


    vec3 vertices[] = {
       {-0.5f, -0.5f, -0.5f}, // Back   
       { 0.5f, -0.5f, -0.5f},
       { 0.5f,  0.5f, -0.5f},
       {-0.5f,  0.5f, -0.5f},

       {-0.5f, -0.5f,  0.5f}, // Front  
       { 0.5f, -0.5f,  0.5f},
       { 0.5f,  0.5f,  0.5f},
       {-0.5f,  0.5f,  0.5f},

       {-0.5f, -0.5f, -0.5f}, // Left   
       {-0.5f,  0.5f, -0.5f},
       {-0.5f,  0.5f,  0.5f},
       {-0.5f, -0.5f,  0.5f},

       { 0.5f, -0.5f, -0.5f}, // Right   
       { 0.5f,  0.5f, -0.5f},
       { 0.5f,  0.5f,  0.5f},
       { 0.5f, -0.5f,  0.5f},

       {-0.5f, -0.5f, -0.5f}, // Bottom   
       {-0.5f, -0.5f,  0.5f},
       { 0.5f, -0.5f,  0.5f},
       { 0.5f, -0.5f, -0.5f},

       {-0.5f,  0.5f, -0.5f}, // Top   
       {-0.5f,  0.5f,  0.5f},
       { 0.5f,  0.5f,  0.5f},
       { 0.5f,  0.5f, -0.5f},
    };


    {
        sg_buffer_desc vbuf_desc = {};
        vbuf_desc.size = sizeof(vertices);
        vbuf_desc.data = SG_RANGE(vertices);
        vbuf_desc.usage = SG_USAGE_IMMUTABLE;
        vbuf_desc.type = SG_BUFFERTYPE_VERTEXBUFFER;
        b->vertex_buffer = sg_make_buffer(&vbuf_desc);
    }




    uint16_t indices[] = {
        0,  1,  2,   0,  2,  3,
        6,  5,  4,   7,  6,  4,
        8,  9,  10,  8,  10, 11,
        14, 13, 12,  15, 14, 12,
        16, 17, 18,  16, 18, 19,
        22, 21, 20,  23, 22, 20,
    };


    {
        sg_buffer_desc ibuf_desc = {};
        ibuf_desc.size = sizeof(indices);
        ibuf_desc.data = SG_RANGE(indices);
        ibuf_desc.type = SG_BUFFERTYPE_INDEXBUFFER;
        ibuf_desc.usage = SG_USAGE_IMMUTABLE;

        b->index_buffer = sg_make_buffer(&ibuf_desc);
    }
 



    {
        vec3 normals[24];
        compute_flat_normals(vertices, indices, 36, normals);

        sg_buffer_desc nbuf_desc = {};
        nbuf_desc.size = sizeof(normals);
        nbuf_desc.data = SG_RANGE(normals);
        nbuf_desc.usage = SG_USAGE_IMMUTABLE;
        nbuf_desc.type = SG_BUFFERTYPE_VERTEXBUFFER;
        b->normal_buffer = sg_make_buffer(&nbuf_desc);

    }






    b->index_count = 36;
}


static
void init_buffers(
    flecs::world& world)
{
    init_rect_buffers(world);
    init_box_buffers(world);
}

//#include <sokol_log.h>  // Sokol的日志支持


// 自定义日志函数
void my_log(const char* tag, uint32_t log_level, uint32_t log_item_id,
    const char* message_or_null, uint32_t line_nr,
    const char* filename_or_null, void* user_data) {

    // 根据日志级别设置日志级别描述
    const char* log_level_str = "INFO";
    if (log_level == 0) log_level_str = "PANIC";
    else if (log_level == 1) log_level_str = "ERROR";
    else if (log_level == 2) log_level_str = "WARNING";

    // 打印日志信息到控制台
    printf("[%s] Level: %s, Item ID: %d, Message: %s, Line: %d, File: %s\n",
        tag,
        log_level_str,
        log_item_id,
        message_or_null ? message_or_null : "No message",
        line_nr,
        filename_or_null ? filename_or_null : "No file");
}


int global_width = 0;
int global_height = 0;


flecs::world world;



//flecs::query<const EcsPosition3, const EcsRectangle, const EcsRgb, const EcsTransform3> rectangle_query;
//
//flecs::query<const EcsPosition3, const EcsBox, const EcsRgb, const EcsTransform3> box_query;


flecs::query<> rectangle_query;

flecs::query<> box_query;


//flecs::query<const SokolMaterial> instance_of_material_query;

flecs::query<> pure_material_query;

flecs::query<> instance_of_material_query;



//auto box_query = world.query_builder<const EcsPosition3, const EcsBox, const EcsRgb, const EcsTransform3>()
//.cached()
//.build();
// 在初始化阶段创建查询
void init_queries(flecs::world& world) {
    rectangle_query = world.query_builder<>()

        .with< EcsPosition3>()
        .with< EcsRectangle>()
        .with< EcsRgb>()
        .with< EcsTransform3>()

        .cached()
        .build();



    box_query = world.query_builder<>()
        .with< EcsPosition3>()
        .with< EcsBox>()
        .with< EcsRgb>()
        .with< EcsTransform3>()
        .cached()
        .build();


    auto HasMaterialentity = world.lookup("HasMaterial");

    instance_of_material_query = world.query_builder()
        .cached()
        .with(HasMaterialentity, flecs::Wildcard)
        //.without<MaterialTag>()
        .build();


    pure_material_query = world.query_builder()
        .cached()
        .with<EcsSpecular>().oper(flecs::Or)
        .with<EcsEmissive>()

        .with<SokolMaterial>().oper(flecs::Not)
        .with< MaterialTag>()
        .build();

    //instance_of_material_query = world.query_builder()
    //    .cached()
    //    .with<EcsSpecular>().up(flecs::ChildOf).oper(flecs::Or)
    //    .with<EcsEmissive>().up(flecs::ChildOf)
    //    .with<SokolMaterial>().oper(flecs::Not)
    //    .build();
}

template<typename GeometryComponent>
void attachGeometry(SokolBuffer& b, flecs::query<>& query, std::function<void(const GeometryComponent*, mat4&)> applyScaling) {

    if (!query.changed()) {
        return;
    }

    int32_t count = 0;
    query.each([&](flecs::entity) {
        count++;
        });

    if (count == 0) {
        b.instance_count = 0;
        return;
    }

    // Reallocate application-level buffers if needed
    if (b.instance_capacity < count) {
        b.instance_capacity = count * 2; // Increase capacity to avoid frequent reallocations
        b.colors = (ecs_rgba_t*)ecs_os_realloc(b.colors, b.instance_capacity * sizeof(ecs_rgba_t));
        b.transforms = (mat4*)ecs_os_realloc(b.transforms, b.instance_capacity * sizeof(mat4));
        b.materials = (float*)ecs_os_realloc(b.materials, b.instance_capacity * sizeof(float));

        // Recreate GPU buffers
        {
            if (b.color_buffer.id != SG_INVALID_ID) {
                sg_destroy_buffer(b.color_buffer);
            }
            sg_buffer_desc color_buf_desc = {};
            color_buf_desc.size = b.instance_capacity * sizeof(ecs_rgba_t);
            color_buf_desc.usage = SG_USAGE_DYNAMIC;
            b.color_buffer = sg_make_buffer(&color_buf_desc);
        }

        {
            if (b.transform_buffer.id != SG_INVALID_ID) {
                sg_destroy_buffer(b.transform_buffer);
            }
            sg_buffer_desc transform_buf_desc = {};
            transform_buf_desc.size = b.instance_capacity * sizeof(mat4);
            transform_buf_desc.usage = SG_USAGE_DYNAMIC;
            b.transform_buffer = sg_make_buffer(&transform_buf_desc);
        }

        {
            if (b.material_buffer.id != SG_INVALID_ID) {
                sg_destroy_buffer(b.material_buffer);
            }
            sg_buffer_desc material_buf_desc = {};
            material_buf_desc.size = b.instance_capacity * sizeof(uint32_t);
            material_buf_desc.usage = SG_USAGE_DYNAMIC;
            b.material_buffer = sg_make_buffer(&material_buf_desc);
        }
    }

    size_t colors_size = count * sizeof(ecs_rgba_t);
    size_t transforms_size = count * sizeof(mat4);
    size_t materials_size = count * sizeof(float);

    int32_t cursor = 0;

    query.each([&](flecs::entity ent) {
     
 
        const GeometryComponent* geometry = ent.get< GeometryComponent>();

        const EcsRgb* color = ent.get< EcsRgb>();

        b.colors[cursor] = *color;

        const EcsTransform3* transform = ent.get< EcsTransform3>();


        // Copy the transform matrix
        glm_mat4_copy(const_cast<mat4&>(transform->value), b.transforms[cursor]);

        // Apply scaling using the provided lambda function
        applyScaling(geometry, b.transforms[cursor]);

        // Get material
        auto HasMaterialentity = world.lookup("HasMaterial");
        if (HasMaterialentity) {
            auto material = ent.target(HasMaterialentity);
            if (material) {
                const SokolMaterial* sm = material.get<SokolMaterial>();
                uint32_t material_id = sm ? sm->material_id : 0;
                b.materials[cursor] = material_id;
            }
        }

        cursor++;
        });

    b.instance_count = count;

    // Update GPU buffers
    sg_update_buffer(b.color_buffer, { .ptr = b.colors, .size = colors_size });
    sg_update_buffer(b.transform_buffer, { .ptr = b.transforms, .size = transforms_size });
    sg_update_buffer(b.material_buffer, { .ptr = b.materials, .size = materials_size });
}



void SokolAttachRect(flecs::entity e, SokolBuffer& b) {

    attachGeometry<EcsRectangle>(b, rectangle_query, [](const EcsRectangle* rect, mat4& transform) {
        vec3 scale = { rect->width, rect->height, 1.0f };
        glm_scale(transform, scale);
        });

    return;
  
}




void SokolAttachBox(flecs::entity e, SokolBuffer& b) {

    attachGeometry<EcsBox>(b, box_query, [](const EcsBox* box, mat4& transform) {
        vec3 scale = { box->width, box->height, box->depth };
        glm_scale(transform, scale);
        });

    return;
}

void SokolAttachBuffer(flecs::entity e, SokolBuffer& b) {

    if (e.has<RectangleTag >())
    {
        SokolAttachRect(e, b);
    }


    if (e.has<BoxTag  >())
    {
        SokolAttachBox(e, b);
    }

}



static
void init_uniforms(const SokolCanvas& canvas, vs_uniforms_t& vs_out, fs_uniforms_t& fs_out, const sokol_render_state_t* state)
{
    // 定义矩阵
    mat4 mat_p, mat_v;

    // 获取相机
    const EcsCamera* cam = nullptr;
    if (state->camera.is_alive()) {
        cam = state->camera.get<EcsCamera>();
    }




    // 计算视图和投影矩阵
    float aspect = (float)global_width / (float)global_height;
    if (cam) {
        // 使用相机参数
        if (cam->ortho) {
            // 正交投影
            glm_ortho(-aspect, aspect, -1.0f, 1.0f, cam->near_, cam->far_, mat_p);
        }
        else {
            // 透视投影
            glm_perspective(cam->fov, aspect, cam->near_, cam->far_, mat_p);
        }
        // 视图矩阵
        glm_lookat((float*)cam->position, (float*)cam->lookat, (float*)cam->up, mat_v);
    }
    else {
        // 默认相机参数
        glm_perspective(glm_rad(30.0f), aspect, 0.1f, 100.0f, mat_p);
        vec3 eye = { 0.0f, -4.0f, 0.0f };
        vec3 center = { 0.0f, 0.0f, 5.0f };
        vec3 up = { 0.0f, 1.0f, 0.0f };
        glm_lookat(eye, center, up, mat_v);
    }

    // 计算视图投影矩阵

    glm_mat4_mul(mat_p, mat_v, vs_out.mat_vp);

    //glm_mat4_identity(vs_out.mat_vp);
   

    const EcsDirectionalLight* light = nullptr;

    ecs_assert(state->light.is_alive(), ECS_INVALID_PARAMETER, NULL);

    light = state->light.get<EcsDirectionalLight>();
    
    ecs_assert(light != NULL, ECS_INVALID_PARAMETER, NULL);
    if (light)
    {
        glm_vec3_copy(light->direction, fs_out.light_direction);
        glm_vec3_copy(light->color, fs_out.light_color);
    }
    else
    {
        glm_vec3_zero(fs_out.light_direction);
        glm_vec3_zero(fs_out.light_color);
    }

 
    glm_vec3_copy((float*)&state->ambient_light, fs_out.light_ambient);
    glm_vec3_copy(cam->position, fs_out.eye_pos);


 
}
 
 
void populate_materials(flecs::world& world, vs_materials_t& mat_u) {
    instance_of_material_query.each([&](flecs::entity e) {
        //uint16_t id = mat.material_id;
        //::MessageBoxA(0, e.name().c_str(), "populate_materials", 0);
       

        auto HasMaterialentity = world.lookup("HasMaterial");

        if (HasMaterialentity)
        {
            auto material = e.target(HasMaterialentity);
            if (material)
            {
               
                const SokolMaterial *sm = material.get<SokolMaterial>();

                if (sm)
                {
                    //::MessageBoxA(0, material.name().c_str(), "material", 0);
                    uint16_t id = sm->material_id;
               

                    const EcsSpecular* spec = material.get<EcsSpecular>();
                    if (spec) {
                        mat_u.array[id].specular_power = spec->specular_power;
                        mat_u.array[id].shininess = spec->shininess;
                    }
                    else {
                        mat_u.array[id].specular_power = 0;
                        mat_u.array[id].shininess = 0;
                    }


                    const EcsEmissive* em = e.get<EcsEmissive>();
                    if (em) {
                        mat_u.array[id].emissive = em->value;
                    }
                    else {
                        mat_u.array[id].emissive = 0;
                    }
                }
            }



        }
      
    });
}




static sg_image init_render_target(int32_t width, int32_t height) {
    sg_image_desc img_desc = {};
    img_desc.render_target = true;
    img_desc.width = width;
    img_desc.height = height;
    img_desc.pixel_format = SG_PIXELFORMAT_RGBA16F;
    img_desc.sample_count = 1;
    img_desc.label = "color-image";

    return sg_make_image(&img_desc);
}



static sg_image init_render_depth_target(int32_t width, int32_t height) {
    sg_image_desc img_desc = {};
    img_desc.render_target = true;
    img_desc.width = width;
    img_desc.height = height;
    img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
    img_desc.sample_count = 1;
    img_desc.label = "depth-image";

    return sg_make_image(&img_desc);
}


static
sg_pass init_offscreen_pass(sg_image img, sg_image depth_img) {
    sg_pass pass = {};

    sg_attachments_desc attachments_desc = {};
    attachments_desc.colors[0].image = img;
    attachments_desc.depth_stencil.image = depth_img;
    attachments_desc.label = "offscreen-attachments";
    sg_attachments attachments = sg_make_attachments(&attachments_desc);

    // 配置渲染通道的清除操作
    sg_pass_action pass_action = {};
    pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    pass_action.colors[0].clear_value = { 0.25f, 0.25f, 0.25f, 1.0f }; // 使用列表初始化
    pass_action.depth.load_action = SG_LOADACTION_CLEAR;
    pass_action.depth.clear_value = 1.0f;

    // 将附件和清除操作应用到离屏渲染通道
    pass.attachments = attachments;
    pass.action = pass_action;
    pass.label = "offscreen-pass";




    return pass;
}



void _sg_initialize(int w, int h) 
{
    global_width = w;
    global_height = h;
    ecs_log_set_level(1);



    world.component<EcsPosition3>();
    world.component<EcsRectangle>();
    world.component<EcsRgb>();
    world.component<EcsTransform3>();
    world.component<SokolCanvas>();
    world.component<SokolBuffer>();
    world.component<EcsCamera>();
    world.component<EcsSpecular>();// .add(flecs::OnInstantiate, flecs::Inherit);
    world.component<EcsEmissive>();// .add(flecs::OnInstantiate, flecs::Inherit);
    world.component<SokolMaterial>();




    // 创建高光材质实体
    auto shiny_material = world.entity("SpecularMaterial")
        .set<EcsSpecular>({ 1.0f, 32.0f }).add< MaterialTag>();
      

    // 创建自发光材质实体
    auto emissive_material = world.entity("EmissiveMaterial")
        .set<EcsEmissive>({ 1.0f }).add< MaterialTag>();
       

    auto HasMaterial = world.entity("HasMaterial");

    auto SokolRectangleBuffer = world.entity("SokolRectangleBuffer")
        .add<SokolBuffer>()
        .add<RectangleTag>();

    auto SokolBoxBuffer = world.entity("SokolBoxBuffer")
        .add<SokolBuffer>()
        .add<BoxTag>();

  
    sg_logger logger = {
          .func = my_log,  // 设置自定义日志函数
          .user_data = NULL  // 传递自定义用户数据（这里为 NULL）
    };


    // **直接在初始化函数中调用 sg_setup**
    sg_desc desc = {
     .logger = logger 
    };
    sg_setup(&desc);
    assert(sg_isvalid());  // 确保 Sokol 已经初始化



    sokol_render_state_t state;
    {

        // 创建相机实体
        EcsCamera camera = {};
        vec3 position = { 0.0f, 0.0f, -50.0f };
        vec3 lookat = { 0.0f, 0.0f, 0.0f };
        vec3 up = { 0.0f, 1.0f, 0.0f };

        glm_vec3_copy(position, camera.position);
        glm_vec3_copy(lookat, camera.lookat);
        glm_vec3_copy(up, camera.up);




        camera.fov = glm_rad(60.0f);
        camera.near_ = .1f;
        camera.far_ = 100.0f;
        camera.ortho = false;

        auto camera_entity = world.entity()
            .set<EcsCamera>(camera);

       
        state.camera = camera_entity;

    }


    {

        // 创建一个 EcsDirectionalLight 实例并初始化
        EcsDirectionalLight light = {};

        // 设置光源的位置（如果需要）
        light.set_position(0.0f, 10.0f, 0.0f);  // 例如，将光源放在 y 轴上方

        // 设置光源的方向
        light.set_direction(0.0f, -1.0f, 0.0f);  // 向下照射

        // 设置光源的颜色
        light.set_color(1.0f, 1.0f, 1.0f);  // 白色光

        // 创建一个带有 EcsDirectionalLight 组件的实体
        auto light_entity = world.entity()
            .set<EcsDirectionalLight>(light);

        // 将光源实体存储在 state 中
        state.light = light_entity;

    }



    sg_image offscreen_tex = init_render_target(w, h);

    sg_image offscreen_depth_tex = init_render_depth_target(w, h);

    // 初始化 SokolCanvas
    SokolCanvas sokol_canvas;
    // 设置背景颜色，您可以根据需要修改
    sokol_canvas.background_color = { 0.2f, 0.1f, 0.1f }; // 灰色背景
    sokol_canvas.pass_action = init_pass_action(&sokol_canvas);
    sokol_canvas.pip = init_pipeline();
    sokol_canvas.offscreen_quad = init_quad();

    sokol_canvas.fx_bloom = sokol_init_bloom(w, h);


    sokol_canvas.tex_pass_action = init_tex_pass_action();
    
    sokol_canvas.tex_pip = init_tex_pipeline();
    sokol_canvas.offscreen_tex = offscreen_tex;
    sokol_canvas.offscreen_depth_tex = offscreen_depth_tex;

    sokol_canvas.offscreen_pass = init_offscreen_pass(offscreen_tex, offscreen_depth_tex);


    sg_sampler_desc sampler_desc = {};
    sampler_desc.min_filter = SG_FILTER_LINEAR;
    sampler_desc.mag_filter = SG_FILTER_LINEAR;
    sokol_canvas.smp = sg_make_sampler(&sampler_desc);


    // 创建一个带有 EcsCanvas 组件的实体
    world.entity()
        .set<EcsCanvas>(sokol_canvas);


    //world.entity()
    //    .set<sokol_render_state_t>(state);


    world.set<sokol_render_state_t>(state); // 单例模式

    // 初始化缓冲区
    init_buffers(world);

    init_queries(world);

    world.system<SokolBuffer>()
        .kind(flecs::PostLoad)
        .each([](flecs::entity e, SokolBuffer& b) {
              SokolAttachBuffer(e, b);
            });





 
        world.system<EcsCamera>()
            .kind(flecs::OnUpdate)
            .each([](flecs::entity e, EcsCamera& camera) {
            // 每秒 60 度，换算成弧度每秒
            const float rotation_speed = glm_rad(60.0f);

            // 获取时间增量 (delta time)
            using clock = std::chrono::high_resolution_clock;
            static auto last_time = clock::now();
            auto now = clock::now();
            float delta_time = std::chrono::duration<float>(now - last_time).count();
            last_time = now;

            // 累积旋转角度
            static float accumulated_angle = 0.0f;
            accumulated_angle += rotation_speed * delta_time;

            // 设置旋转半径 (相机到旋转中心的距离)
            float radius = glm_vec3_norm(camera.position);

            // 根据累积角度计算新的相机位置
            camera.position[0] = 50 * cos(accumulated_angle);


                });
 

  



    world.system<const SokolCanvas>()
        .kind(flecs::OnStore)
        .each([](flecs::entity e, const SokolCanvas& canvas) {
   
        const sokol_render_state_t* state = world.get<sokol_render_state_t>();

        vs_uniforms_t vs_u;
        fs_uniforms_t fs_u;


        init_uniforms(canvas, vs_u, fs_u, state);

        static vs_materials_t mat_u = {};


        if (pure_material_query.changed())
        {
            printf("\n pure_material_query changed \n");
            pure_material_query.each([](flecs::entity e) {
                static uint16_t next_material = 1;
                //::MessageBoxA(0, e.name().c_str(), "set SokolMaterial", 0);
                e.set(SokolMaterial{ next_material++ });
                /*                const SokolMaterial* sm = e.get_mut<SokolMaterial>();
                                auto x = sm->material_id;*/
                                //::MessageBoxA(0, e.name().c_str(), "set id", 0);

                });


        }

        else if (instance_of_material_query.changed())
        {
            printf("\n instance_of_material_query changed \n");
            populate_materials(world, mat_u);
        }




        sg_begin_pass(canvas.offscreen_pass);


        sg_apply_pipeline(canvas.pip);


        world.each([&](flecs::entity e, SokolBuffer& buffer) {
                if (buffer.instance_count == 0) {
                    return;
                }

                sg_bindings bind = {};
                bind.vertex_buffers[0] = buffer.vertex_buffer;
                bind.vertex_buffers[1] = buffer.normal_buffer;
                bind.vertex_buffers[2] = buffer.color_buffer;
                bind.vertex_buffers[3] = buffer.material_buffer;
                bind.vertex_buffers[4] = buffer.transform_buffer;
                bind.index_buffer = buffer.index_buffer;

                sg_apply_bindings(&bind);


                {
                    sg_range uniform_data = { .ptr = &vs_u, .size = sizeof(vs_uniforms_t) };
                    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, uniform_data);
                }
                //if (materials_changed) 
                {
                    sg_range uniform_data = { .ptr = &mat_u, .size = sizeof(vs_materials_t) };
                    sg_apply_uniforms(SG_SHADERSTAGE_VS, 1, uniform_data);
                }

                {
                    sg_range uniform_data = { .ptr = &fs_u, .size = sizeof(fs_u) };
                    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, uniform_data);
                }




                sg_draw(0, buffer.index_count, buffer.instance_count);
            });

        sg_end_pass();



        //run passes
        {

            sg_image tex_fx = sokol_effect_run(&canvas, &canvas.fx_bloom, canvas.offscreen_tex);

            sg_pass pass = {0};


            sg_swapchain swapchain = {};
            swapchain.width = global_width;
            swapchain.height = global_height;


            pass.action = canvas.tex_pass_action;
            pass.swapchain = swapchain;

            sg_begin_pass(pass);
            sg_apply_pipeline(canvas.tex_pip);


            sg_bindings bind = {};
            bind.vertex_buffers[0] = canvas.offscreen_quad;
            bind.fs.images[0] = tex_fx;// canvas.offscreen_tex;
            bind.fs.samplers[0] = canvas.smp;


            sg_apply_bindings(&bind);

            sg_draw(0, 6, 1);
            sg_end_pass();

        }



        sg_commit();
 
            });






    auto init_transform = [](EcsTransform3& transform, const EcsPosition3& position) {
        glm_mat4_identity(transform.value); // 初始化为单位矩阵


        vec3 translation = { position.x, position.y, position.z };
        glm_translate(transform.value, translation);

        };

    if(1)
    {

   
        // Rectangle
        EcsPosition3 pos1 = { -0.5f, 0.f, 0.0f };
        EcsRectangle rect1 = { 20.f, 20.f };
        EcsRgb color1 = { .5f, 0.0f, 0.0f, 1.0f };
        EcsTransform3 transform1;
        init_transform(transform1, pos1);

        world.entity()
            .set<EcsPosition3>(pos1)
            .set<EcsRectangle>(rect1)
            .set<EcsRgb>(color1)
            .set<EcsTransform3>(transform1);
    }

    if(0)
    {
        // Rectangle
        EcsPosition3 pos2 = { 1.5f, .0f, .0f }; // 位于x轴正方向2.0的位置
        EcsRectangle rect2 = { .5f, .5f }; // 宽度和高度为1.0
        EcsRgb color2 = { 0.0f, .0f, 1.f, 1.0f }; // 绿色
        EcsTransform3 transform2;
        init_transform(transform2, pos2);

        world.entity("arect")
            .set<EcsPosition3>(pos2)
            .set<EcsRectangle>(rect2)
            .set<EcsRgb>(color2)
            .set<EcsTransform3>(transform2)
            .add(HasMaterial, shiny_material);
         

    }

    if(1)
    {
        // 创建第二个矩形实体
        EcsPosition3 pos2 = { .5f, .2f, .0f }; // 位于x轴正方向2.0的位置
        EcsBox box = { .5f, .5f , .5}; // 宽度和高度为1.0
        EcsRgb color2 = { 1.f, 1.1f, 0.0f, 1.0f }; // 绿色
        EcsTransform3 transform2;
        init_transform(transform2, pos2);

        world.entity("abox")
            .set<EcsPosition3>(pos2)
            .set<EcsBox>(box)
            .set<EcsRgb>(color2)
            .set<EcsTransform3>(transform2)
            .add(HasMaterial, shiny_material);

    }

   

    const float box_size = 0.5f;
    const float spacing = 2.0f;
    const int grid_half_size = 1;  // 100 个 box 对称排列的半径

    for (int x = -grid_half_size; x < grid_half_size; x++) {
        for (int y = -grid_half_size; y < grid_half_size; y++) {
            EcsPosition3 pos = { x * spacing, y * spacing, 0.0f };
            EcsBox box = { box_size, box_size, box_size };
            EcsRgb color = { 1.f, 1.1f, 0.0f, 1.0f };  // 设置 box 的颜色
            EcsTransform3 transform;
            glm_mat4_identity(transform.value);
            vec3 translation = { pos.x, pos.y, pos.z };
            glm_translate(transform.value, translation);

            world.entity()
                .set<EcsPosition3>(pos)
                .set<EcsBox>(box)
                .set<EcsRgb>(color)
                .set<EcsTransform3>(transform)
                .add(HasMaterial, shiny_material);
        }
    }

}



void _sg_shutdown() 
{
    world.system<SokolBuffer>()
        .kind(flecs::PostLoad)
        .each([](flecs::entity e, SokolBuffer& b) {
                     b.releaseBuffer();
       });
}






float calculateOscillatingY(float amplitude, float speed) {
    // 获取当前时间
    using clock = std::chrono::high_resolution_clock;
    static auto start_time = clock::now();
    auto now = clock::now();
    float time = std::chrono::duration<float>(now - start_time).count();  // 以秒为单位的时间

    // 使用正弦函数计算 y 轴的震动
    return sin(time * speed) * amplitude;
}




void _sg_render(int w, int h)
{
    global_width = w;
    global_height = h;




    // 设置震动参数
    float oscillation_amplitude = 360.f; 
    float oscillation_speed = .1f;     


    //float oscillation_amplitude = .5f;
    //float oscillation_speed = 1.f;

    float new_y = calculateOscillatingY(oscillation_amplitude, oscillation_speed);

  
    //world.each([&](EcsCamera& camera) {
    //    camera.position[0] = new_y;  
    //    });
    int xx = 0;

    //world.each([&](flecs::entity e, EcsTransform3& transform) {
    //  
    //    

    //    if(0)
    //    {
    //        // 将矩阵重置为单位矩阵
    //        glm_mat4_identity(transform.value);
    //        // 计算新的位移
    //        if (xx % 2 == 0)
    //        {
    //            vec3 translation = { new_y, 0.0f, 0.0f };
    //            glm_translate(transform.value, translation);
    //        }
    //        if (xx % 2 == 1)
    //        {
    //            vec3 translation = {  0.0f,new_y, 0.0f };
    //            glm_translate(transform.value, translation);
    //        }
    //    }


    //    if(0)
    //    {

    //        glm_mat4_identity(transform.value);

    //        // 旋转角度（以弧度为单位），例如 45 度的旋转
    //        float angle = glm_rad(new_y); // new_y 可以控制旋转速度或方向

    //        // 根据实体的编号来控制不同的旋转轴
    //        if (xx % 2 == 0) {
    //            vec3 axis = { 1.0f, 0.0f, 0.0f };  // 绕 X 轴旋转
    //            glm_rotate(transform.value, angle, axis);
    //        }
    //        if (xx % 2 == 1) {
    //            vec3 axis = { 0.0f, 1.0f, 0.0f };  // 绕 Y 轴旋转
    //            glm_rotate(transform.value, angle, axis);
    //        }

    //        xx++;
    //    }
    //    
    //    
    //    });


    world.progress();

}