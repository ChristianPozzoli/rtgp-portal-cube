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

class DitherShaderScene : public ShaderScene
{
public:
    DitherShaderScene (std::string m_name, GLint width, GLint height) : ShaderScene(m_name, width, height)
    {}

    ~DitherShaderScene()
    {
        delete illum_shader;
        delete screen_shader;

        delete swordModel;
        delete swordTexture;

        delete screen_fbo;
        delete screen_quad;

        glDeleteRenderbuffers(1, &rbo);
    }

    void setup_scene() override
    {
        illum_shader = new Shader(
            (SHADER_PATH + "illumination_model.vert").c_str(),
            (SHADER_PATH + "dithering/illumination_model.frag").c_str()
        );
        screen_shader = new Shader(
            (SHADER_PATH + "screen.vert").c_str(),
            (SHADER_PATH + "dithering/dither.frag").c_str()
        );
        
        ModelObject* floorObject = new ModelObject("Floor", "../../models/plane.obj", *illum_shader, glm::vec3(0.0f, -1.0f, 0.0f), 20.0f);
        floorObject->setColor(glm::vec3(0.534f, 0.514f, 0.183f));
        
        ModelObject* sphereObject = new ModelObject("Sphere", "../../models/sphere.obj", *illum_shader, glm::vec3(60.0f, 60.0f, -60.0f), 1.5f);
        sphereObject->setColor(glm::vec3(1.0f, 1.0f, 0.0f));
        ModelObject* treasureObject = new ModelObject("Treasure", "../../models/dithering_scene/treasure.fbx", *illum_shader, glm::vec3(0.0f, -1.0f, 0.0f), 2.5f, glm::vec3(0.0f, -84.0f, 0.0f));
        treasureObject->setColor(glm::vec3(0.505f, 0.342f, 0.000f));
        ModelObject* krakenObject = new ModelObject("Kraken", "../../models/dithering_scene/kraken.fbx", *illum_shader, glm::vec3(26.0f, -4.0f, 0.0f), 2.5f, glm::vec3(0.0f, -60.0f, 0.0f));
        krakenObject->setTexture("../../textures/dithering_scene/kraken.png");
        ModelObject* tentaclesObject = new ModelObject("Tentacles", "../../models/dithering_scene/tentacles.fbx", *illum_shader, glm::vec3(5.0f, 8.0f, 10.0f), 0.8f);
        tentaclesObject->setTexture("../../textures/dithering_scene/tentacles.png");
        ModelObject* mantaObject = new ModelObject("Manta", "../../models/dithering_scene/manta.fbx", *illum_shader, glm::vec3(-16.0f, 18.0f, -2.0f), 8.0f, glm::vec3(0.0f, 0.0f, -38.0f));
        mantaObject->setTexture("../../textures/dithering_scene/manta.png");
        ModelObject* compassObject = new ModelObject("Compass", "../../models/dithering_scene/compass.fbx", *illum_shader, glm::vec3(-1.0f, -0.9f, 11.0f), 2.0f, glm::vec3(0.0f, 83.0f, -0.0f));
        compassObject->setTexture("../../textures/dithering_scene/compass.png");
        ModelObject* skullObject = new ModelObject("Skull", "../../models/dithering_scene/skull.fbx", *illum_shader, glm::vec3(-1.0f, -0.75f, 17.0f), 0.5f, glm::vec3(-45.0f, -22.0f, 180.0f));
        
        swordModel = new Model("../../models/dithering_scene/sword.gltf");
        swordTexture = new Texture("../../textures/dithering_scene/sword.png");
        ModelObject* swordObject_1 = new ModelObject("Sword 1", swordModel, *illum_shader, glm::vec3(-0.96f, 0.0f, 16.98f), 1.5f, glm::vec3(80.0f, 11.0f, 0.0f));
        swordObject_1->setTexture(swordTexture);
        ModelObject* swordObject_2 = new ModelObject("Sword 2", swordModel, *illum_shader, glm::vec3(12.5f, 3.2f, 7.3f), 1.5f, glm::vec3(-49.0f, -21.0f, 13.0f));
        swordObject_2->setTexture(swordTexture);
        ModelObject* swordObject_3 = new ModelObject("Sword 3", swordModel, *illum_shader, glm::vec3(-4.0f, -0.7f, 7.0f), 1.5f, glm::vec3(86.0f, 0.0f, 0.0f));
        swordObject_3->setTexture(swordTexture);

        ModelObject* shipObject = new ModelObject("Ship", "../../models/dithering_scene/ship.fbx", *illum_shader, glm::vec3(17.0f, 18.0f, 34.0f), 0.75f, glm::vec3(-90.0f, -22.0f, 265.0f));
        shipObject->setColor(glm::vec3(0.240f, 0.177f, 0.000f));
        
        add_internal_object(floorObject);
        add_internal_object(sphereObject);
        add_internal_object(treasureObject);
        add_internal_object(krakenObject);
        add_internal_object(tentaclesObject);
        add_internal_object(mantaObject);
        add_internal_object(compassObject);
        add_internal_object(skullObject);
        add_internal_object(swordObject_1);
        add_internal_object(swordObject_2);
        add_internal_object(swordObject_3);
        add_internal_object(shipObject);

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
        glClearColor(0.000f, 0.088f, 1.000f, 1.000f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        draw_objects(view, projection);
        
        if (is_main_scene)
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
        
        screen_shader->SetInt("screenTexture", 0);
        
        screen_shader->SetVec2("screen_size", 1, glm::value_ptr(glm::vec2(m_width, m_height)));
        screen_shader->SetFloat("color_factor", colorFactor);
        screen_shader->SetFloat("dither_factor", ditherFactor);
        screen_shader->SetFloat("time", glfwGetTime());
        
        screen_shader->Use();
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
        
        ImGui::SliderInt("Color Factor", &colorFactor, 0.0f, 10.0f);
        ImGui::SliderInt("Dither Factor", &ditherFactor, 1.0f, 24.0f);

        ShaderScene::drawImGui();
    }

private:
    Shader* illum_shader;
    Shader* screen_shader;

    FrameBuffer* screen_fbo;

    ScreenQuadObject* screen_quad;

    GLuint rbo;

    Model* swordModel;
    Texture* swordTexture;

    GLint colorFactor = 8;
    GLint ditherFactor = 2;

    glm::vec3 lightPos0 = glm::vec3(5.0f, 10.0f, 10.0f);
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

        illum_shader.SetFloat("Kd", Kd);
        illum_shader.SetFloat("Ks", Ks);
        illum_shader.SetFloat("Ka", Ka);
        illum_shader.SetFloat("shininess", shininess);
        illum_shader.SetFloat("alpha", alpha);
        illum_shader.SetFloat("F0", F0);
    }
};