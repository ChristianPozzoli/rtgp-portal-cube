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
#include <utils/texture.h>
#include <utils/model.h>
#include <utils/camera.h>

#include <utils/modelobject.h>
#include <utils/planeobject.h>
#include <utils/screenquadobject.h>
#include <utils/shaderscene.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI 3.14159265359

using namespace std;

class SketchShaderScene : public ShaderScene
{
public:
    SketchShaderScene (std::string m_name, GLFWwindow* window, GLint width, GLint height) : ShaderScene(m_name), window(window), width(width), height(height)
    {}

    ~SketchShaderScene()
    {
        delete screen_quad;
        delete spheremapObject;

        delete illum_shader;
        delete color_shader;
        delete depth_shader;
        delete normal_shader;
        delete spheremap_shader;
        delete screen_shader;
    }

    void setup_scene() override
    {
        illum_shader = new Shader(
            (SHADER_PATH + "illumination_model.vert").c_str(),
            (SHADER_PATH + "sketch_shaders/lambert.frag").c_str()
        );
        color_shader = new Shader(
            (SHADER_PATH + "basic.vert").c_str(),
            (SHADER_PATH + "fullcolor.frag").c_str()
        );
        depth_shader = new Shader(
            (SHADER_PATH + "basic.vert").c_str(),
            (SHADER_PATH + "depth.frag").c_str()
        );
        normal_shader = new Shader(
            (SHADER_PATH + "illumination_model.vert").c_str(),
            (SHADER_PATH + "sketch_shaders/normal.frag").c_str()
        );
        spheremap_shader = new Shader(
            (SHADER_PATH + "sketch_shaders/spheremap.vert").c_str(),
            (SHADER_PATH + "sketch_shaders/spheremap.frag").c_str()
        );
        screen_shader = new Shader(
            (SHADER_PATH + "screen.vert").c_str(),
            (SHADER_PATH + "sketch_shaders/screen.frag").c_str()
        );

        Texture* hatch_texture = new Texture("../../textures/hatch_rgb.png");
        hatch_texture->setWrapS(GL_REPEAT);
        hatch_texture->setWrapT(GL_REPEAT);
        
        ModelObject* bunnyObject = new ModelObject("Bunny", "../../models/bunny_lp.obj", *illum_shader, glm::vec3(- 6.0f, - 0.7f, 9.0f), 0.1f);
        bunnyObject->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
        bunnyObject->setColor(glm::vec3(1.0f, 0.441f, 0.0f));

        ModelObject* sphereObject = new ModelObject("Sphere", "../../models/sphere.obj", *illum_shader, glm::vec3(- 3.5f, - 0.55f, 16.0f), 0.35f);
        sphereObject->setColor(glm::vec3(1.0f, 0.0f, 0.0f));

        ModelObject* cubeObject = new ModelObject("Cube", "../../models/cube.obj", *illum_shader, glm::vec3(-5.0f, 1.0f, -5.0f), 1.5f);
        cubeObject->setColor(glm::vec3(0.0f, 0.0f, 1.0f));

        spheremapObject = new ModelObject("sphereMap", "../../models/sphere.obj", *spheremap_shader);
        spheremapObject->setTexture(hatch_texture);

        ModelObject* floorObject = new ModelObject("Floor", "../../models/plane.obj", *illum_shader, glm::vec3(0.0f, -1.0f, 0.0f));
        floorObject->setScale(glm::vec3(10.0f, 1.0f, 10.0f));
        floorObject->setColor(glm::vec3(0.0f, 0.5f, 0.0f));
        
        ModelObject* treeObject = new ModelObject("Stylized tree", "../../models/sketch_scene/stylized_tree_trunk.fbx", *illum_shader);
        treeObject->setPosition(glm::vec3(3.0f, - 1.0f, -2.0f));
        treeObject->setScale(0.25f);
        treeObject->setColor(glm::vec3(0.343f, 0.212f, 0.0f));
        ModelObject* leavesObject = new ModelObject("Leaves", "../../models/sketch_scene/stylized_tree_leaves.fbx", *illum_shader);
        leavesObject->setColor(glm::vec3(1.0f, 0.681f, 0.991f));
        treeObject->addChild(leavesObject);
        
        ModelObject* oldTreeObject = new ModelObject("Old tree", "../../models/sketch_scene/old_tree.fbx", *illum_shader);
        Texture* oldTreeTexture = new Texture("../../textures/sketch_scene/old_tree.png");
        oldTreeObject->setTexture(oldTreeTexture);
        oldTreeObject->setPosition(glm::vec3(-2.0f, - 0.6f, 16.0f));
        oldTreeObject->setRotation(glm::vec3(180.0f, 220.0f, 0.0f));
        oldTreeObject->setScale(0.75f);
        oldTreeObject->setColor(glm::vec3(1.0f, 0.559f, 0.0f));
        
        ModelObject* benchObject = new ModelObject("Bench", "../../models/sketch_scene/bench.fbx", *illum_shader);
        Texture* benchTexture = new Texture("../../textures/sketch_scene/bench.png");
        benchObject->setTexture(benchTexture);
        benchObject->setPosition(glm::vec3(2.0f, - 1.0f, -2.0f));
        benchObject->setRotation(glm::vec3(0.0f, 120.0f, 0.0f));
        
        ModelObject* swingObject = new ModelObject("Swing", "../../models/sketch_scene/swing.fbx", *illum_shader);
        swingObject->setColor(glm::vec3(0.0f, 0.412f, 1.0f));
        swingObject->setPosition(glm::vec3(0.2f, -0.5f, 15.5f));
        swingObject->setRotation(glm::vec3(0.0f, 5.0f, 0.0f));
        swingObject->setScale(0.6f);
        
        ModelObject* sliderObject = new ModelObject("Slider", "../../models/sketch_scene/slider.fbx", *illum_shader);
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
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);  
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // NORMAL FBO
        glGenFramebuffers(1, &normal_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, normal_fbo);

        glGenTextures(1, &normal_fbo_texture);
        glBindTexture(GL_TEXTURE_2D, normal_fbo_texture);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normal_fbo_texture, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
        
        // DEPTH FBO
        glGenFramebuffers(1, &depth_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);

        glGenTextures(1, &depth_fbo_texture);
        glBindTexture(GL_TEXTURE_2D, depth_fbo_texture);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depth_fbo_texture, 0);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
        
        // SPHEREMAP FBO
        glGenFramebuffers(1, &spheremap_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, spheremap_fbo);

        glGenTextures(1, &spheremap_fbo_texture);
        glBindTexture(GL_TEXTURE_2D, spheremap_fbo_texture);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, spheremap_fbo_texture, 0);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

        // SCREEN FBO
        glGenFramebuffers(1, &screen_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

        glGenTextures(1, &screen_fbo_texture);
        glBindTexture(GL_TEXTURE_2D, screen_fbo_texture);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_fbo_texture, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            cout << "ERROR: Framebuffer is not complete" << endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void update_scene(Camera* camera, glm::mat4& view, glm::mat4& projection, Shader* override_shader = nullptr) override
    {
        setup_illum_shader(illum_shader);
        normal_shader->Use();
        normal_shader->SetVec3("pointLightPosition", 1, glm::value_ptr(lightPos0));
        
        glEnable(GL_DEPTH_TEST);
        
        // RENDER ON NORMAL FBO
        glBindFramebuffer(GL_FRAMEBUFFER, normal_fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        draw_objects(view, projection, normal_shader);
        
        // RENDER ON DEPTH FBO
        glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        draw_objects(view, projection, depth_shader);
        
        // RENDER ON spherEMAP FBO
        glBindFramebuffer(GL_FRAMEBUFFER, spheremap_fbo);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDepthFunc(GL_LEQUAL);
        
        glm::mat4 spheremapViewMatrix = glm::mat4(
            camera->WorldFront.x, 0.0, -camera->WorldFront.z, 0.0,
            0.0,                  1.0, 0.0,                   0.0,
            camera->WorldFront.z, 0.0, camera->WorldFront.x,  0.0,
            0.0,                  0.0, 0.0,                   1.0
        );
        spheremap_shader->SetFloat("viewAngleY", glm::asin(camera->Front.y) / PI);
        spheremap_shader->SetFloat("hatching_repeat", hatchingRepeat);
        spheremapObject->draw(spheremapViewMatrix, projection);
        
        glDepthFunc(GL_LESS);
        
        // RENDER ON SCREEN FBO
        glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        draw_objects(view, projection);
        
        // RENDER ON DEFAULT FBO
        glBindFramebuffer(GL_READ_FRAMEBUFFER, screen_fbo);
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
        
        screen_shader->SetInt("normalTexture", 0);
        screen_shader->SetInt("depthTexture", 1);
        screen_shader->SetInt("screenTexture", 2);
        screen_shader->SetInt("hatchTexture", 3);
        screen_shader->SetInt("paperTexture", 4);
        
        screen_shader->SetVec3("background_color", 1, glm::value_ptr(backgroundColor));
        screen_shader->SetVec3("edge_color", 1, glm::value_ptr(edgeColor));
        screen_shader->SetFloat("color_saturation", colorSaturation);
        screen_shader->SetFloat("color_brightness", colorBrightness);
        screen_shader->SetFloat("edge_threshold", edgeThreshold);
        screen_shader->SetFloat("noise_frequency_edge", noiseFrequencyEdge);
        screen_shader->SetFloat("noise_strength_edge", noiseStrengthEdge);
        screen_shader->SetFloat("noise_strength_color", noiseStrengthColor);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, normal_fbo_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depth_fbo_texture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, screen_fbo_texture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, spheremap_fbo_texture);
        
        glDisable(GL_DEPTH_TEST);
        
        screen_quad->draw();
        
        glEnable(GL_DEPTH_TEST);
    }

    void delete_scene()
    {
        glDeleteRenderbuffers(1, &rbo);
        glDeleteFramebuffers(1, &normal_fbo);
        glDeleteFramebuffers(1, &depth_fbo);
        glDeleteFramebuffers(1, &spheremap_fbo);
        glDeleteFramebuffers(1, &screen_fbo);

        illum_shader->Delete();
        color_shader->Delete();
        depth_shader->Delete();
        normal_shader->Delete();
        spheremap_shader->Delete();
        screen_shader->Delete();
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
        ImGui::ColorEdit3("Background Color", (float*)&backgroundColor);
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
    GLFWwindow* window;
    GLint width;
    GLint height;
    GLboolean wireframe = GL_FALSE;

    glm::vec3 lightPos0 = glm::vec3(5.0f, 10.0f, 10.0f);

    GLfloat Kd = 0.5f;
    GLfloat Ks = 0.4f;
    GLfloat Ka = 0.1f;
    GLfloat shininess = 25.0f;
    GLfloat alpha = 0.2f;
    GLfloat F0 = 0.9f;

    float hatchingRepeat = 40.0f;
    glm::vec3 backgroundColor = glm::vec3(1.0f);
    glm::vec3 edgeColor = glm::vec3(0.25f);
    float colorSaturation = 0.4f;
    float colorBrightness = 0.9f;
    float edgeThreshold = 0.005f;
    float noiseFrequencyEdge = 40.0f;
    float noiseStrengthEdge = 0.00175f;
    float noiseStrengthColor = 0.0025f;

    void setup_illum_shader(Shader&);

    Shader* illum_shader;
    Shader* color_shader;
    Shader* depth_shader;
    Shader* normal_shader;
    Shader* spheremap_shader;
    Shader* screen_shader;

    ScreenQuadObject* screen_quad;
    ModelObject* spheremapObject;

    GLuint rbo;
    GLuint normal_fbo;
    GLuint normal_fbo_texture;
    GLuint depth_fbo;
    GLuint depth_fbo_texture;
    GLuint spheremap_fbo;
    GLuint spheremap_fbo_texture;
    GLuint screen_fbo;
    GLuint screen_fbo_texture;

    void setup_illum_shader(Shader* illum_shader)
    {
        illum_shader->Use();
        GLint pointLightLocation = glGetUniformLocation(illum_shader->Program, "pointLightPosition");
        GLint matSpecularLocation = glGetUniformLocation(illum_shader->Program, "specularColor");
        GLint matAmbientLocation = glGetUniformLocation(illum_shader->Program, "ambientColor");
        GLint kdLocation = glGetUniformLocation(illum_shader->Program, "Kd");
        GLint ksLocation = glGetUniformLocation(illum_shader->Program, "Ks");
        GLint kaLocation = glGetUniformLocation(illum_shader->Program, "Ka");
        GLint shininessLocation = glGetUniformLocation(illum_shader->Program, "shininess");
        GLint alphaLocation = glGetUniformLocation(illum_shader->Program, "alpha");
        GLint f0Location = glGetUniformLocation(illum_shader->Program, "F0");
        
        glUniform3fv(pointLightLocation, 1, glm::value_ptr(lightPos0));
        glUniform1f(kdLocation, Kd);
        glUniform1f(ksLocation, Ks);
        glUniform1f(kaLocation, Ka);
        glUniform1f(shininessLocation, shininess);
        glUniform1f(alphaLocation, alpha);
        glUniform1f(f0Location, F0);
    }
};