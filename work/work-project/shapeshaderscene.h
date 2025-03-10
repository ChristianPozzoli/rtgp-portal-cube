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

class ShapeShaderScene : public ShaderScene
{
public:
    ShapeShaderScene (std::string m_name, GLint width, GLint height) : ShaderScene(m_name, width, height)
    {}

    ~ShapeShaderScene()
    {
        delete illum_shader;
        delete screen_shader;

        delete screen_fbo;
        delete screen_quad;

        glDeleteRenderbuffers(1, &rbo);
    }

    void setup_scene() override
    {
        illum_shader = new Shader(
            (SHADER_PATH + "illumination_model.vert").c_str(),
            (SHADER_PATH + "illumination_model.frag").c_str()
        );
        screen_shader = new Shader(
            (SHADER_PATH + "screen.vert").c_str(),
            (SHADER_PATH + "screen.frag").c_str()
        );
    
        ModelObject* sphereObject = new ModelObject("Sphere", "../../models/sphere.obj", *illum_shader, glm::vec3(0.0f, 1.0f, -10.0f), 1.5f);
        sphereObject->setColor(glm::vec3(1.0f, 0.0f, 0.0));
    
        ModelObject* floorObject = new ModelObject("Floor", "../../models/plane.obj", *illum_shader, glm::vec3(0.0f, -1.0f, 0.0f));
        floorObject->setScale(glm::vec3(10.0f, 1.0f, 10.0f));
        floorObject->setColor(glm::vec3(0.0f, 0.5f, 0.0f));

        add_internal_object(sphereObject);
        add_internal_object(floorObject);

        // RBO
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);  
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        screen_fbo = new FrameBuffer(m_width, m_height, rbo);        

        screen_quad = new ScreenQuadObject();
    }

    void update_window(GLFWwindow* window, GLint width, GLint height)
    {
        ShaderScene::update_window(window, width, height);

        delete screen_fbo;

        glDeleteRenderbuffers(1, &rbo);
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);  
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        screen_fbo = new FrameBuffer(width, height, rbo);
    }

    void update_scene(Camera* camera, glm::mat4& view, glm::mat4& projection, Shader* override_shader = nullptr, bool is_main_scene = false) override
    {
        glEnable(GL_DEPTH_TEST);
        
        setup_illum_shader(*illum_shader);
        
        // RENDER ON SCREEN FBO
        screen_fbo->bind();
        glClearColor(0.26f, 0.46f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        draw_objects(view, projection);
        
        if(is_main_scene)
        {
            screen_fbo->bind_read();
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
        glDisable(GL_DEPTH_TEST);
        
        screen_shader->Use();
        screen_shader->SetInt("screenTexture", 0);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screen_fbo->texture_name());

        screen_quad->draw();
        
        glEnable(GL_DEPTH_TEST);
    }

    void drawImGui() override
    {
        if (!ImGui::CollapsingHeader(m_name.c_str()))
        {
            return;
        }

        ImGui::DragFloat3("Light Position", (float*)&lightPos0);
        ImGui::SliderFloat("Kd", (float*)&Kd, 0.0f, 2.0f);
        ImGui::SliderFloat("Ks", (float*)&Ks, 0.0f, 2.0f);
        ImGui::SliderFloat("Ka", (float*)&Ka, 0.0f, 5.0f);
        ImGui::SliderFloat("Shininess", (float*)&shininess, 0.0f, 100.0f);
        ImGui::SliderFloat("Alpha", (float*)&alpha, 0.0f, 5.0f);
        ImGui::SliderFloat("F0", (float*)&F0, 0.0f, 100.0f);
        ImGui::ColorEdit3("Specular color", (float*)&specularColor);
        ImGui::ColorEdit3("Ambient color", (float*)&ambientColor);

        ShaderScene::drawImGui();
    }

private:
    Shader* illum_shader;
    Shader* screen_shader;

    FrameBuffer* screen_fbo;

    ScreenQuadObject* screen_quad;

    GLuint rbo;

    glm::vec3 lightPos0 = glm::vec3(5.0f, 10.0f, 10.0f);
    glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
    GLfloat Kd = 0.5f;
    GLfloat Ks = 0.4f;
    GLfloat Ka = 0.1f;
    GLfloat shininess = 25.0f;
    GLfloat alpha = 0.2f;
    GLfloat F0 = 0.9f;

    void setup_illum_shader(Shader& illum_shader)
    {
        illum_shader.Use();

        illum_shader.SetVec3("pointLightPosition", 1, glm::value_ptr(lightPos0));
        illum_shader.SetVec3("specularColor", 1, glm::value_ptr(specularColor));
        illum_shader.SetVec3("ambientColor", 1, glm::value_ptr(ambientColor));

        illum_shader.SetFloat("Kd", Kd);
        illum_shader.SetFloat("Ks", Ks);
        illum_shader.SetFloat("Ka", Ka);
        illum_shader.SetFloat("shininess", shininess);
        illum_shader.SetFloat("alpha", alpha);
        illum_shader.SetFloat("F0", F0);
    }
};