#pragma once
#include <string>

#ifdef _WIN32
    #define APIENTRY __stdcall
#endif

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>

#include <glfw/glfw3.h>

#ifdef _WINDOWS_
    #error windows.h was included!
#endif

#include <utils/shader.h>
#include <utils/framebuffer.h>
#include <utils/texture.h>
#include <utils/model.h>
#include <utils/camera.h>

#include <utils/modelobject.h>
#include <utils/planeobject.h>
#include <utils/screenquadobject.h>
#include <utils/shaderscene.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

using namespace std;

class CelShadingShaderScene : public ShaderScene
{
public:
    CelShadingShaderScene (std::string m_name, GLFWwindow* window, GLint width, GLint height) : ShaderScene(m_name), window(window), width(width), height(height)
    {}

    ~CelShadingShaderScene()
    {
        delete screen_quad;
        delete lut_cel_shading_texture;

        glDeleteRenderbuffers(1, &rbo);
        delete screen_fbo;

        delete cel_shading;
        delete back_face_shader;
        delete screen_shader;
    }

    void setup_scene() override
    {
        back_face_shader = new Shader(
            (SHADER_PATH + "cel_shading_shaders/backface_outline.vert").c_str(),
            (SHADER_PATH + "cel_shading_shaders/blackcolor.frag").c_str()
        );
        cel_shading = new Shader(
            (SHADER_PATH + "illumination_model.vert").c_str(),
            (SHADER_PATH + "cel_shading_shaders/cel_shading.frag").c_str()
        );
        screen_shader = new Shader(
            (SHADER_PATH + "screen.vert").c_str(),
            (SHADER_PATH + "screen.frag").c_str()
        );
        
        lut_cel_shading_texture = new Texture(
            ("../../textures/cel_shading_scene/" + string(lut_items[lut_current_index])).c_str(),
            GL_CLAMP_TO_EDGE,
            GL_NEAREST,
            GL_NEAREST
        );
            
        ModelObject* floorObject = new ModelObject("Floor", "../../models/plane.obj", *cel_shading, glm::vec3(0.0f, -1.0f, 0.0f));
        floorObject->setScale(glm::vec3(10.0f, 1.0f, 10.0f));
        floorObject->setColor(glm::vec3(0.0f, 0.5f, 0.0f));
        
        // we load the model(s) (code of Model class is in include/utils/model.h)
        ModelObject* bunnyObject = new ModelObject("Bunny", "../../models/bunny_lp.obj", *cel_shading, glm::vec3(-5.0f, 1.0f, -5.0f), 0.5f);
        
        // we load the model(s) (code of Model class is in include/utils/model.h)
        ModelObject* sphereObject = new ModelObject("Sphere", "../../models/sphere.obj", *cel_shading, glm::vec3(0.0f, 1.0f, -5.0f), 1.5f);
        
        ModelObject* duckObject = new ModelObject("Duck", "../../models/cel_shading_scene/rubber_duck_toy_lp.gltf", *cel_shading, glm::vec3(-1.0f, -1.0f, 4.0), 1.0f, glm::vec3(-90.0f, 0.0, 150.0f));
        Texture* duckTexture = new Texture("../../textures/cel_shading_scene/rubber_duck_toy_diff_1k.jpg");
        duckObject->setTexture(duckTexture);
        
        ModelObject* teddyObject = new ModelObject("Teddy", "../../models/cel_shading_scene/teddybear.fbx", *cel_shading, glm::vec3(0.0, 0.0, 4.0f), 1.0f, glm::vec3(-90.0f, 0.0, 180.0f));
        Texture* teddyTexture = new Texture("../../textures/cel_shading_scene/teddybear_base.png");
        teddyObject->setTexture(teddyTexture);
        
        ModelObject* teddyObject2 = new ModelObject("Teddy2", "../../models/cel_shading_scene/teddybear_2.gltf", *cel_shading, glm::vec3(0.0, 0.0, 0.0), 0.5f, glm::vec3(-90.0f, 0.0, 0.0));
        Texture* teddyTexture2 = new Texture("../../textures/cel_shading_scene/teddybear_2.png");
        teddyObject2->setTexture(teddyTexture2);
        
        ModelObject* spidermanObject = new ModelObject("Spiderman", "../../models/cel_shading_scene/spiderman_toy_lp.gltf", *cel_shading, glm::vec3(1.2f, -1.07f, 0.0), 0.15f, glm::vec3(-90.0f, 0.0, 0.0));
        Texture* spidermanTexture = new Texture("../../textures/cel_shading_scene/spiderman_toy.png");
        spidermanObject->setTexture(spidermanTexture);
        
        ModelObject* cribObject = new ModelObject("Crib", "../../models/cel_shading_scene/baby_crib.gltf", *cel_shading, glm::vec3(0.0, -1.0f, 0.0), 0.3f, glm::vec3(0.0f, 30.0f, 0.0));
        Texture* cribTexture = new Texture("../../textures/cel_shading_scene/baby_crib.jpeg");
        cribObject->setTexture(cribTexture);
        
        ModelObject* cubesToyObject = new ModelObject("Cubes toy", "../../models/cel_shading_scene/cubes_toy.gltf", *cel_shading, glm::vec3(-1.25f, -1.0f, 0.85), 0.085f, glm::vec3(0.0, 0.0, 0.0));
        Texture* cubesToyTexture = new Texture("../../textures/cel_shading_scene/cubes_toy.png");
        cubesToyObject->setTexture(cubesToyTexture);

        screen_quad = new ScreenQuadObject;
    
        add_internal_object(floorObject);
        add_internal_object(bunnyObject);
        add_internal_object(sphereObject);
        add_internal_object(duckObject);
        add_internal_object(teddyObject);
        add_internal_object(teddyObject2);
        add_internal_object(spidermanObject);
        add_internal_object(cribObject);
        add_internal_object(cubesToyObject);

        // RBO
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);  
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // SCREEN FBO
        screen_fbo = new FrameBuffer(width, height, rbo);
    }

    void update_scene(Camera* camera, glm::mat4& view, glm::mat4& projection, Shader* override_shader = nullptr) override
    {
        glEnable(GL_DEPTH_TEST);

        screen_fbo->bind();
        glClearColor(0.26f, 0.46f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        setup_cel_shading(*cel_shading);
        cel_shading->SetInt("LUT", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, lut_cel_shading_texture->name());
        
        back_face_shader->Use();
        back_face_shader->SetFloat("alongNormalsFactor", along_normals_factor);
		
        glEnable(GL_CULL_FACE);
        
        glCullFace(GL_FRONT);
        draw_objects(view, projection, back_face_shader);
        
        glCullFace(GL_BACK);
        draw_objects(view, projection);

        glDisable(GL_CULL_FACE);
        
        // RENDER ON DEFAULT FBO
        screen_fbo->bind_read();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        
        glBlitFramebuffer(
            0, 0, width, height,
            0, 0, width, height,
            GL_DEPTH_BUFFER_BIT,
            GL_NEAREST
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void draw(glm::mat4& view, glm::mat4& projection, Shader* override_shader = nullptr) override
    {
        screen_shader->Use();
        
        screen_shader->SetInt("screenTexture", 0);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screen_fbo->texture_name());
        
        glDisable(GL_DEPTH_TEST);
        
        screen_quad->draw();
        
        glEnable(GL_DEPTH_TEST);
    }

    void drawImGui() override
    {
        if (!ImGui::CollapsingHeader(m_name.c_str()))
        {
            return;
        }

        ImGui::SeparatorText("LUTs");
        if (ImGui::Combo("LUT", &lut_current_index, lut_items, IM_ARRAYSIZE(lut_items))) {
            if(lut_current_index >= 0) {
                delete(lut_cel_shading_texture);
                lut_cel_shading_texture = 
                    new Texture(("../../textures/cel_shading_scene/" + string(lut_items[lut_current_index])).c_str(),
                                    GL_CLAMP_TO_EDGE,
                                    GL_NEAREST,
                                    GL_NEAREST);
            }
        }
        ImGui::SeparatorText("Texture");
        ImGui::InputFloat("Offset Factor", (float*)&cel_shading_texture_offset_factor, 1.0f, 10000.0f);
        ImGui::SliderFloat("Outline Threshold Lower", (float*)&cel_shading_texture_outline_threshold_lower, 0.0f, 50.0f);
        ImGui::SliderFloat("Outline Threshold Upper", (float*)&cel_shading_texture_outline_threshold_upper, 0.0f, 50.0f);
        ImGui::SliderInt("Poster Factor for sobel", &cel_shading_poster_factor_sobel, 1.0f, 50.0f);
        ImGui::SliderInt("Poster Factor for final color", &cel_shading_poster_factor_final, 1.0f, 50.0f);
        ImGui::SeparatorText("Glossiness");
        ImGui::SliderFloat("Threshold lower", (float*)&gloss_threshold_lower, 0.0f, 1.0f);
        ImGui::SliderFloat("Threshold upper", (float*)&gloss_threshold_upper, 0.0f, 1.0f);
        ImGui::SliderFloat("Factor", (float*)&gloss_factor, 0.0f, 1.0f);
        ImGui::SeparatorText("Outline");
        ImGui::SliderFloat("Vectors along normals", (float*)&along_normals_factor, 0.0f, 2.0f, "%.5f");

        ShaderScene::drawImGui();
    }

private:
    GLFWwindow* window;
    GLint width;
    GLint height;
    
    glm::vec3 lightPos0 = glm::vec3(5.0f, 10.0f, 10.0f);
    
    Shader* cel_shading;
    Shader* back_face_shader;
    Shader* screen_shader;

    Texture* lut_cel_shading_texture;

    ScreenQuadObject* screen_quad;

    GLuint rbo;
    FrameBuffer* screen_fbo;

    int lut_current_index = 6;
    const char* lut_items[9] = {
        "LUT_cel_shading_1.png",
        "LUT_cel_shading_2.png",
        "LUT_cel_shading_3.png",
        "LUT_cel_shading_4.png",
        "LUT_cel_shading_5.png",
        "LUT_cel_shading_6.png",
        "LUT_cel_shading_7.png",
        "LUT_cel_shading_8.png",
        "LUT_cel_shading_9.png",
    };
  
    GLfloat cel_shading_texture_offset_factor = 750.0f;
    GLint cel_shading_poster_factor_sobel = 20;
    GLint cel_shading_poster_factor_final = 10;
    GLfloat cel_shading_texture_outline_threshold_lower = 2.0f;
    GLfloat cel_shading_texture_outline_threshold_upper = 3.0f;
    GLfloat along_normals_factor = 0.0035f;
    GLfloat gloss_threshold_lower = 0.97f;
    GLfloat gloss_threshold_upper = 0.98f;
    GLfloat gloss_factor = 0.25f;

    void setup_cel_shading(Shader& cel_shading)
    {
        cel_shading.Use();

        GLint pointLightLocation = glGetUniformLocation(cel_shading.Program, "pointLightPosition");
        GLint matSpecularLocation = glGetUniformLocation(cel_shading.Program, "specularColor");
        GLint matAmbientLocation = glGetUniformLocation(cel_shading.Program, "ambientColor");
        GLint thicknessLocation = glGetUniformLocation(cel_shading.Program, "thickness");
        
        glUniform3fv(pointLightLocation, 1, glm::value_ptr(lightPos0));

        cel_shading.SetFloat("offset_factor", cel_shading_texture_offset_factor);
        cel_shading.SetFloat("outline_threshold_lower", cel_shading_texture_outline_threshold_lower);
        cel_shading.SetFloat("outline_threshold_upper", cel_shading_texture_outline_threshold_upper);
        cel_shading.SetInt("poster_factor_sobel", cel_shading_poster_factor_sobel);
        cel_shading.SetInt("poster_factor_final", cel_shading_poster_factor_final);
        cel_shading.SetFloat("gloss_threshold_lower", gloss_threshold_lower);
        cel_shading.SetFloat("gloss_threshold_upper", gloss_threshold_upper);
        cel_shading.SetFloat("gloss_factor", gloss_factor);
    }
};