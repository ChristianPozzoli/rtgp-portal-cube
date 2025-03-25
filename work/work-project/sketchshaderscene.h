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
#include <utils/texturecubemap.h>
#include <utils/model.h>
#include <utils/camera.h>

#include <utils/modelobject.h>
#include <utils/planeobject.h>
#include <utils/screenquadobject.h>
#include <utils/shaderscene.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

class SketchShaderScene : public ShaderScene
{
public:
    SketchShaderScene (std::string m_name, GLint width, GLint height) : ShaderScene(m_name, width, height)
    {}

    ~SketchShaderScene()
    {
        delete screen_quad;
        delete spheremapObject;
        delete skyboxCubeObject;

        delete hatch_texture;
        delete skyCubeMap;

        delete normal_fbo;
        delete lambert_depth_fbo;
        delete spheremap_fbo;
        delete color_fbo;

        delete lambert_depth_shader;
        delete color_shader;
        delete normal_shader;
        delete spheremap_shader;
        delete skybox_shader;
        delete screen_shader;

        glDeleteRenderbuffers(1, &rbo);
        rbo = 0;
    }

    void setup_scene() override
    {
        lambert_depth_shader = new Shader(
            (SHADER_PATH + "illumination_model.vert").c_str(),
            (SHADER_PATH + "sketch_shaders/lambert-depth.frag").c_str()
        );
        color_shader = new Shader(
            (SHADER_PATH + "basic.vert").c_str(),
            (SHADER_PATH + "fullcolor.frag").c_str()
        );
        normal_shader = new Shader(
            (SHADER_PATH + "illumination_model.vert").c_str(),
            (SHADER_PATH + "sketch_shaders/normal.frag").c_str()
        );
        spheremap_shader = new Shader(
            (SHADER_PATH + "sketch_shaders/spheremap.vert").c_str(),
            (SHADER_PATH + "sketch_shaders/spheremap.frag").c_str()
        );
        skybox_shader = new Shader(
            (SHADER_PATH + "skybox.vert").c_str(),
            (SHADER_PATH + "skybox.frag").c_str()
        );
        screen_shader = new Shader(
            (SHADER_PATH + "screen.vert").c_str(),
            (SHADER_PATH + "sketch_shaders/screen.frag").c_str()
        );

        hatch_texture = new Texture("../../textures/hatch_rgb.png");
        hatch_texture->setWrapS(GL_REPEAT);
        hatch_texture->setWrapT(GL_REPEAT);

        skyCubeMap = new TextureCubeMap("../../textures/cube/plainsky/", ".png");
        
        ModelObject* bunnyObject = new ModelObject("Bunny", "../../models/bunny_lp.obj", *lambert_depth_shader, glm::vec3(- 6.0f, - 0.7f, 9.0f), 0.1f);
        bunnyObject->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
        bunnyObject->setColor(glm::vec3(1.0f, 0.441f, 0.0f));

        ModelObject* sphereObject = new ModelObject("Sphere", "../../models/sphere.obj", *lambert_depth_shader, glm::vec3(- 3.5f, - 0.55f, 16.0f), 0.35f);
        sphereObject->setColor(glm::vec3(1.0f, 0.0f, 0.0f));

        ModelObject* cubeObject = new ModelObject("Cube", "../../models/cube.obj", *lambert_depth_shader, glm::vec3(-5.0f, 1.0f, -5.0f), 1.5f);
        cubeObject->setColor(glm::vec3(0.0f, 0.0f, 1.0f));

        spheremapObject = new ModelObject("sphereMap", "../../models/sphere.obj", *spheremap_shader);
        spheremapObject->setTexture(hatch_texture);

        skyboxCubeObject = new ModelObject("Cube", "../../models/cube.obj", *skybox_shader);

        ModelObject* floorObject = new ModelObject("Floor", "../../models/plane.obj", *lambert_depth_shader, glm::vec3(0.0f, -1.0f, 0.0f));
        floorObject->setScale(glm::vec3(10.0f, 1.0f, 10.0f));
        floorObject->setColor(glm::vec3(0.0f, 0.5f, 0.0f));
        
        ModelObject* treeObject = new ModelObject("Stylized tree", "../../models/sketch_scene/stylized_tree_trunk.fbx", *lambert_depth_shader);
        treeObject->setPosition(glm::vec3(3.0f, - 1.0f, -2.0f));
        treeObject->setScale(0.25f);
        treeObject->setColor(glm::vec3(0.343f, 0.212f, 0.0f));
        ModelObject* leavesObject = new ModelObject("Leaves", "../../models/sketch_scene/stylized_tree_leaves.fbx", *lambert_depth_shader);
        leavesObject->setColor(glm::vec3(1.0f, 0.681f, 0.991f));
        treeObject->addChild(leavesObject);
        
        ModelObject* oldTreeObject = new ModelObject("Old tree", "../../models/sketch_scene/old_tree.fbx", *lambert_depth_shader);
        oldTreeObject->setTexture("../../textures/sketch_scene/old_tree.png");
        oldTreeObject->setPosition(glm::vec3(-2.0f, - 0.6f, 16.0f));
        oldTreeObject->setRotation(glm::vec3(180.0f, 220.0f, 0.0f));
        oldTreeObject->setScale(0.75f);
        oldTreeObject->setColor(glm::vec3(1.0f, 0.559f, 0.0f));
        
        ModelObject* benchObject = new ModelObject("Bench", "../../models/sketch_scene/bench.fbx", *lambert_depth_shader);
        benchObject->setTexture("../../textures/sketch_scene/bench.png");
        benchObject->setPosition(glm::vec3(2.0f, - 1.0f, -2.0f));
        benchObject->setRotation(glm::vec3(0.0f, 120.0f, 0.0f));
        
        ModelObject* swingObject = new ModelObject("Swing", "../../models/sketch_scene/swing.fbx", *lambert_depth_shader);
        swingObject->setColor(glm::vec3(0.0f, 0.412f, 1.0f));
        swingObject->setPosition(glm::vec3(0.2f, -0.5f, 15.5f));
        swingObject->setRotation(glm::vec3(0.0f, 5.0f, 0.0f));
        swingObject->setScale(0.6f);
        
        ModelObject* sliderObject = new ModelObject("Slider", "../../models/sketch_scene/slider.fbx", *lambert_depth_shader);
        sliderObject->setColor(glm::vec3(1.0f, 1.0f, 0.0f));
        sliderObject->setPosition(glm::vec3(18.0f, 1.8f, 10.0f));
        sliderObject->setRotation(glm::vec3(0.0f, - 120.0f, 0.0f));
        sliderObject->setScale(0.8f);

        screen_quad = new ScreenQuadObject;

        add_internal_object(floorObject);
        add_internal_object(bunnyObject);
        add_internal_object(cubeObject);
        add_internal_object(sphereObject);

        add_internal_object(treeObject);
        add_internal_object(leavesObject);
        add_internal_object(oldTreeObject);
        add_internal_object(benchObject);
        add_internal_object(swingObject);
        add_internal_object(sliderObject);
        
        glm::mat4 view = glm::mat4(1.0f);
        
        // RBO
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);  
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        
        // NORMAL FBO
        normal_fbo = new FrameBuffer(m_width, m_height, rbo);
        
        // DEPTH FBO
        lambert_depth_fbo = new FrameBuffer(m_width, m_height, rbo);
        
        // SPHEREMAP FBO
        spheremap_fbo = new FrameBuffer(m_width, m_height, rbo);
        
        // COLOR FBO
        color_fbo = new FrameBuffer(m_width, m_height, rbo);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            cout << "ERROR: Framebuffer is not complete" << endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void update_window(GLFWwindow* window, GLint width, GLint height)
    {
        ShaderScene::update_window(window, width, height);

        delete normal_fbo;
        delete lambert_depth_fbo;
        delete spheremap_fbo;
        delete color_fbo;

        glDeleteRenderbuffers(1, &rbo);
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);  
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        normal_fbo = new FrameBuffer(width, height, rbo);
        lambert_depth_fbo = new FrameBuffer(width, height, rbo);
        spheremap_fbo = new FrameBuffer(width, height, rbo);
        color_fbo = new FrameBuffer(width, height, rbo);
    }

    void update_scene(Camera* camera, glm::mat4& view, glm::mat4& projection, GLfloat deltaTime, bool is_main_scene = false) override
    {
        normal_shader->SetVec3("pointLightPosition", 1, glm::value_ptr(lightPos0));
        normal_shader->SetFloat("highlight_threshold", highlightThreshold);
        
        glEnable(GL_DEPTH_TEST);
        
        // RENDER ON NORMAL FBO
        normal_fbo->bind();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        draw_objects(view, projection, normal_shader);
        
        // RENDER ON DEPTH FBO
        lambert_depth_shader->SetFloat("highlight_threshold", highlightThreshold);
        lambert_depth_shader->SetVec3("pointLightPosition", 1, glm::value_ptr(lightPos0));

        lambert_depth_fbo->bind();
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        draw_objects(view, projection);
        
        // RENDER ON SPHEREMAP FBO
        spheremap_fbo->bind();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDepthFunc(GL_LEQUAL);
        glColorMask(0xFF, 0xFF, 0xFF, 0xFF);
        
        glm::mat4 spheremapViewMatrix = glm::mat4(
            camera->WorldFront.x, 0.0, -camera->WorldFront.z, 0.0,
            0.0,                  1.0, 0.0,                   0.0,
            camera->WorldFront.z, 0.0, camera->WorldFront.x,  0.0,
            0.0,                  0.0, 0.0,                   1.0
        );
        spheremap_shader->SetFloat("viewAngleY", glm::asin(camera->Front.y) / glm::pi<float>());
        spheremap_shader->SetFloat("hatching_repeat", hatchingRepeat);
        
        spheremapObject->draw(spheremapViewMatrix, projection);
        
        glDepthFunc(GL_LESS);
        
        // RENDER ON SCREEN FBO
        color_fbo->bind();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        draw_objects(view, projection, color_shader);

        skybox_shader->Use();
        skybox_shader->SetInt("tCube", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyCubeMap->name());
        glDepthFunc(GL_LEQUAL);
        skyboxCubeObject->draw(glm::mat4(glm::mat3(view)), projection);
        glDepthFunc(GL_LESS);

        if(is_main_scene)
        {
            color_fbo->bind_read();
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            
            glBlitFramebuffer(
                0, 0, m_width, m_height,
                0, 0, m_width, m_height,
                GL_DEPTH_BUFFER_BIT,
                GL_NEAREST
            );
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void draw(glm::mat4& view, glm::mat4& projection, Shader* override_shader = nullptr) override
    {
        screen_shader->Use();
        
        screen_shader->SetInt("normalTexture", 0);
        screen_shader->SetInt("lambertdepthTexture", 1);
        screen_shader->SetInt("colorTexture", 2);
        screen_shader->SetInt("hatchTexture", 3);
        
        screen_shader->SetVec3("sky_color", 1, glm::value_ptr(skyColor));
        screen_shader->SetVec3("edge_color", 1, glm::value_ptr(edgeColor));
        screen_shader->SetFloat("color_saturation", colorSaturation);
        screen_shader->SetFloat("color_brightness", colorBrightness);
        screen_shader->SetFloat("highlight_threshold", highlightThreshold);
        screen_shader->SetFloat("edge_threshold", edgeThreshold);
        screen_shader->SetFloat("noise_frequency_edge", noiseFrequencyEdge);
        screen_shader->SetFloat("noise_strength_edge", noiseStrengthEdge);
        screen_shader->SetFloat("noise_strength_color", noiseStrengthColor);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, normal_fbo->texture_name());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, lambert_depth_fbo->texture_name());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, color_fbo->texture_name());
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, spheremap_fbo->texture_name());
        
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

        ImGui::SeparatorText("Hatch");
        ImGui::SliderFloat("Hatch texture repetition", &hatchingRepeat, 0.0f, 100.0f);
        ImGui::SeparatorText("Colors");
        ImGui::ColorEdit3("Sky Color", (float*)&skyColor);
        ImGui::SliderFloat("Highlight threshold", &highlightThreshold, 0.0f, 1.0f, "%.3f");
        ImGui::ColorEdit3("Edge Color", (float*)&edgeColor);
        ImGui::SliderFloat("Color saturation", &colorSaturation, 0.0f, 1.0f);
        ImGui::SliderFloat("Color brightness", &colorBrightness, 0.0f, 1.0f);
        ImGui::SeparatorText("Edge");
        ImGui::SliderFloat("Edge threshold", &edgeThreshold, 0.0f, 2.0f, "%.3f");
        ImGui::SeparatorText("Noise");
        ImGui::SliderFloat("Noise frequency edge", &noiseFrequencyEdge, 0.0f, 500.0f);
        ImGui::SliderFloat("Noise strength edge", &noiseStrengthEdge, 0.0f, 0.005f, "%.5f");
        ImGui::SliderFloat("Noise strength color", &noiseStrengthColor, 0.0f, 0.005f, "%.5f");

        ShaderScene::drawImGui();
    }

private:
    GLboolean wireframe = GL_FALSE;

    glm::vec3 lightPos0 = glm::vec3(5.0f, 10.0f, 10.0f);

    float hatchingRepeat = 40.0f;
    glm::vec3 skyColor = glm::vec3(0.5f, 0.75f, 1.000f);
    glm::vec3 edgeColor = glm::vec3(0.25f);
    float colorSaturation = 0.4f;
    float colorBrightness = 0.9f;
    float highlightThreshold = 0.9f;
    float edgeThreshold = 0.005f;
    float noiseFrequencyEdge = 40.0f;
    float noiseStrengthEdge = 0.00175f;
    float noiseStrengthColor = 0.0025f;

    Texture* hatch_texture;
    TextureCubeMap* skyCubeMap;

    Shader* lambert_depth_shader;
    Shader* color_shader;
    Shader* normal_shader;
    Shader* spheremap_shader;
    Shader* skybox_shader;
    Shader* screen_shader;

    ScreenQuadObject* screen_quad;
    ModelObject* spheremapObject;
    ModelObject* skyboxCubeObject;

    GLuint rbo;

    FrameBuffer* normal_fbo;
    FrameBuffer* lambert_depth_fbo;
    FrameBuffer* spheremap_fbo;
    FrameBuffer* color_fbo;
};