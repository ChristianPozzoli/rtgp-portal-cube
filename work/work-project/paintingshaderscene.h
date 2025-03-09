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

class PaintingShaderScene : public ShaderScene
{
public:
    PaintingShaderScene (std::string m_name, GLFWwindow* window, GLint width, GLint height) : ShaderScene(m_name), window(window), width(width), height(height)
    {}

    ~PaintingShaderScene()
    {
        delete screen_quad;
        delete color_fbo;
        delete blur_fbo;
        delete mean_fbo;

        delete illum_shader;
        delete blur_shader;
        delete mean_shader;
        delete paint_shader;

        glDeleteRenderbuffers(1, &rbo);
    }

    void setup_scene() override
    {
        illum_shader = new Shader(
            (SHADER_PATH + "illumination_model.vert").c_str(),
            (SHADER_PATH + "painting/illumination_model.frag").c_str()
        );
        blur_shader = new Shader(
            (SHADER_PATH + "screen.vert").c_str(),
            (SHADER_PATH + "painting/blur.frag").c_str()
        );
        mean_shader = new Shader(
            (SHADER_PATH + "screen.vert").c_str(),
            (SHADER_PATH + "painting/mean.frag").c_str()
        );
        paint_shader = new Shader(
            (SHADER_PATH + "screen.vert").c_str(),
            (SHADER_PATH + "painting/painting.frag").c_str()
        );

        ModelObject* floorObject = new ModelObject("Floor", "../../models/plane.obj", *illum_shader, glm::vec3(0.0f, -1.0f, 0.0f));
        floorObject->setScale(glm::vec3(10.0f, 1.0f, 10.0f));
        floorObject->setColor(glm::vec3(0.0f, 0.5f, 0.0f));
        
        ModelObject* bunnyObject = new ModelObject("Bunny", "../../models/bunny_lp.obj", *illum_shader, glm::vec3(0.0f, 1.0f, -5.0f), 0.5f);
        bunnyObject->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
        
        ModelObject* sphereObject = new ModelObject("Sphere", "../../models/sphere.obj", *illum_shader, glm::vec3(5.0f, 1.0f, -5.0f), 1.5f);
        sphereObject->setColor(glm::vec3(1.0f, 1.0f, 0.0f));
        
        ModelObject* cubeObject = new ModelObject("Cube", "../../models/cube.obj", *illum_shader, glm::vec3(-5.0f, 1.0f, -5.0f), 1.5f);
        cubeObject->setColor(glm::vec3(0.1f, 0.3f, 1.0f));

        ModelObject* orangeObject = new ModelObject("Orange", "../../models/painting_scene/orange.fbx", *illum_shader, glm::vec3(-4.0f, -0.6f, 10.0f), 3.0f, glm::vec3(-75.0f, 50.0f, 17.0f));
        orangeObject->setTexture("../../textures/painting_scene/orange.jpeg");
        ModelObject* watermelonObject = new ModelObject("Watermelon", "../../models/painting_scene/watermelon.fbx", *illum_shader, glm::vec3(17.0f, 1.0f, 12.0f), 4.0f, glm::vec3(180.0f, 0.0f, 120.0f));
        watermelonObject->setTexture("../../textures/painting_scene/watermelon.png");
        ModelObject* pomegranateObject = new ModelObject("Pomegranate", "../../models/painting_scene/pomegranate.fbx", *illum_shader, glm::vec3(3.0f, -1.0f, 20.0f), 5.0f, glm::vec3(0.0f, 130.0f, 0.0f));
        pomegranateObject->setTexture("../../textures/painting_scene/pomegranate.jpeg");
        ModelObject* strawberryObject = new ModelObject("Strawberry", "../../models/painting_scene/strawberry.fbx", *illum_shader, glm::vec3(8.0f, 1.0f, 0.0f), 3.5f, glm::vec3(182.0f, 10.0f, 109.0f));
        strawberryObject->setTexture("../../textures/painting_scene/strawberry.jpeg");

        add_internal_object(floorObject);
        add_internal_object(bunnyObject);
        add_internal_object(cubeObject);
        add_internal_object(sphereObject);
        add_internal_object(orangeObject);
        add_internal_object(watermelonObject);
        add_internal_object(pomegranateObject);
        add_internal_object(strawberryObject);

        width_fraction = width / resolution_fraction;
        height_fraction = height / resolution_fraction;

        // RBO
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_fraction, height_fraction);  
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // SCREEN FBO
        color_fbo = new FrameBuffer(width_fraction, height_fraction, rbo);
        blur_fbo = new FrameBuffer(width_fraction, height_fraction, rbo);
        mean_fbo = new FrameBuffer(width_fraction, height_fraction, rbo);

        screen_quad = new ScreenQuadObject();
    }

    void update_scene(Camera* camera, glm::mat4& view, glm::mat4& projection, Shader* override_shader = nullptr) override
    {
        glEnable(GL_DEPTH_TEST);
        
        // RENDER ON COLOR FBO
        color_fbo->bind();
        
        setup_illum_shader(*illum_shader);
        glViewport(0, 0, width_fraction, height_fraction);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        draw_objects(view, projection);
        
        // RENDER ON DEFAULT FBO
        color_fbo->bind_read();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        
        glBlitFramebuffer(
            0, 0, width_fraction, height_fraction,
            0, 0, width, height,
            GL_DEPTH_BUFFER_BIT,
            GL_NEAREST
        );
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void draw(glm::mat4& view, glm::mat4& projection, Shader* override_shader = nullptr) override
    {
        glDisable(GL_DEPTH_TEST);
        
        // BLUR FBO
        blur_fbo->bind();
        blur_shader->Use();
        blur_shader->SetInt("screenTexture", 0);
        blur_shader->SetFloat("offset_amount", offset_amount_blur);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, color_fbo->texture_name());
        
        screen_quad->draw();
        
        // MEAN FBO
        mean_fbo->bind();
        mean_shader->Use();
        
        mean_shader->SetInt("screenTexture", 0);
        mean_shader->SetInt("previousScreenTexture", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, blur_fbo->texture_name());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mean_fbo->texture_name());
        
        screen_quad->draw();
        
        // DEFAULT FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        paint_shader->Use();
        
        paint_shader->SetInt("screenTexture", 0);
        paint_shader->SetFloat("offset_amount", offset_amount_paint);
        paint_shader->SetInt("samples_dimension", samples_dimension);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mean_fbo->texture_name());
        
        screen_quad->draw();
        
        glEnable(GL_DEPTH_TEST);
    }

    void drawImGui() override
    {
        if (!ImGui::CollapsingHeader(m_name.c_str()))
        {
            return;
        }
        
        if (ImGui::SliderFloat("Resolution fraction", &resolution_fraction, 1.0f, 50.0f))
        {
            delete color_fbo;
            delete blur_fbo;
            delete mean_fbo;

            width_fraction = width / resolution_fraction;
            height_fraction = height / resolution_fraction;
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_fraction, height_fraction);  
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            color_fbo = new FrameBuffer(width_fraction, height_fraction, rbo);
            mean_fbo = new FrameBuffer(width_fraction, height_fraction, rbo);
            blur_fbo = new FrameBuffer(width_fraction, height_fraction, rbo);
        }
        
        ImGui::SliderInt("Sample dimension", &samples_dimension, 0.0f, 10.0f);
        ImGui::SliderFloat("Paint offset amount", &offset_amount_paint, 0.0f, 500.0f);
        ImGui::SliderFloat("Blur offset amount", &offset_amount_blur, 0.0f, 500.0f);

        ShaderScene::drawImGui();
    }

private:
    GLFWwindow* window;
    GLint width;
    GLint height;
    
    glm::vec3 lightPos0 = glm::vec3(5.0f, 10.0f, 10.0f);
    GLfloat Kd = 0.5f;
    GLfloat Ks = 0.4f;
    GLfloat Ka = 0.1f;
    GLfloat shininess = 25.0f;
    GLfloat alpha = 0.2f;
    GLfloat F0 = 0.9f;

    GLfloat width_fraction;
    GLfloat height_fraction;
    GLfloat resolution_fraction = 10.0f;
    GLfloat offset_amount_paint = 200.0f;
    GLfloat offset_amount_blur = 200.0f;
    GLint samples_dimension = 3;

    Shader* illum_shader;
    Shader* blur_shader;
    Shader* mean_shader;
    Shader* paint_shader;

    FrameBuffer* color_fbo;
    FrameBuffer* blur_fbo;
    FrameBuffer* mean_fbo;

    ScreenQuadObject* screen_quad;

    GLuint rbo;

    void setup_illum_shader(Shader& illum_shader)
    {
        illum_shader.Use();

        illum_shader.SetVec3("pointLightPosition", 1, glm::value_ptr(lightPos0));

        illum_shader.SetFloat("Kd", Kd);
        illum_shader.SetFloat("Ks", Ks);
        illum_shader.SetFloat("Ka", Ka);
        illum_shader.SetFloat("shininess", shininess);
        illum_shader.SetFloat("alpha", alpha);
        illum_shader.SetFloat("F0", F0);
    }
};