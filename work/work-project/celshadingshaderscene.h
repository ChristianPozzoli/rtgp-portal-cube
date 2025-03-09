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
    CelShadingShaderScene (std::string m_name, GLint width, GLint height) : ShaderScene(m_name, width, height)
    {}

    ~CelShadingShaderScene()
    {
        delete screen_quad;
        delete drawer_model;
        delete plane_model;
        delete wall_texture;
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
        

        plane_model = new Model("../../models/plane.obj");
        ModelObject* floorObject = new ModelObject("Floor", plane_model, *cel_shading, glm::vec3(5.0f, -1.0f, 10.0f), 1.0f);
        floorObject->setTexture("../../textures/cel_shading_scene/floor.png", 3.5f);

        wall_texture = new Texture("../../textures/cel_shading_scene/wall.jpg");

        ModelObject* wallObject_1 = new ModelObject("Wall_1", plane_model, *cel_shading, glm::vec3(5.0f, 0.0f, 5.0f), 1.5f, glm::vec3(90.0f, 0.0f, 0.0f));
        wallObject_1->setTexture(wall_texture, 3.5f);
        ModelObject* wallObject_2 = new ModelObject("Wall_2", plane_model, *cel_shading, glm::vec3(5.0f, 0.0f, 15.0f), 1.5f, glm::vec3(90.0f, 0.0f, 180.0f));
        wallObject_2->setTexture(wall_texture, 3.5f);
        ModelObject* wallObject_3 = new ModelObject("Wall_3", plane_model, *cel_shading, glm::vec3(0.0f, 0.0f, 10.0f), 1.5f, glm::vec3(90.0f, 0.0f, -90.0f));
        wallObject_3->setTexture(wall_texture, 3.5f);
        ModelObject* wallObject_4 = new ModelObject("Wall_4", plane_model, *cel_shading, glm::vec3(10.0f, 0.0f, 10.0f), 1.5f, glm::vec3(90.0f, 0.0f, 90.0f));
        wallObject_4->setTexture(wall_texture, 3.5f);
        ModelObject* wallObject_5 = new ModelObject("Wall_5", plane_model, *cel_shading, glm::vec3(5.0f, 2.5f, 10.0f), 1.5f, glm::vec3(180.0f, 0.0f, 0.0f));
        wallObject_5->setTexture(wall_texture, 3.0f);

        // we load the model(s) (code of Model class is in include/utils/model.h)
        ModelObject* bunnyObject = new ModelObject("Bunny", "../../models/bunny_lp.obj", *cel_shading, glm::vec3(8.0f, -0.85f, 7.0f), 0.05f);
        bunnyObject->setColor(glm::vec3(0.500f, 0.471f, 0.441f));
        
        // we load the model(s) (code of Model class is in include/utils/model.h)
        ModelObject* sphereObject = new ModelObject("Sphere", "../../models/sphere.obj", *cel_shading, glm::vec3(0.9f, -0.79f, 10.5f), 0.2f);
        sphereObject->setColor(glm::vec3(0.564f, 0.731f, 1.000f));
        
        ModelObject* duckObject = new ModelObject("Duck", "../../models/cel_shading_scene/rubber_duck_toy_lp.gltf", *cel_shading, glm::vec3(2.0f, -1.0f, 13.0), 1.0f, glm::vec3(-90.0f, 0.0, 150.0f));
        duckObject->setTexture("../../textures/cel_shading_scene/rubber_duck_toy_diff_1k.jpg");
        
        ModelObject* teddyObject = new ModelObject("Teddy", "../../models/cel_shading_scene/teddybear.fbx", *cel_shading, glm::vec3(5.0, 0.05f, 14.0f), 1.0f, glm::vec3(-90.0f, 0.0, 180.0f));
        teddyObject->setTexture("../../textures/cel_shading_scene/teddybear_base.png");
        
        ModelObject* teddyObject2 = new ModelObject("Teddy 2", "../../models/cel_shading_scene/teddybear_2.gltf", *cel_shading, glm::vec3(8.5f, 0.06f, 10.0f), 0.5f, glm::vec3(-90.0f, 0.0, -75.0));
        teddyObject2->setTexture("../../textures/cel_shading_scene/teddybear_2.png");
        
        ModelObject* spidermanObject = new ModelObject("Spiderman", "../../models/cel_shading_scene/spiderman_toy_lp.gltf", *cel_shading, glm::vec3(5.2f, -1.07f, 6.0f), 0.15f, glm::vec3(-90.0f, 0.0, 0.0));
        spidermanObject->setTexture("../../textures/cel_shading_scene/spiderman_toy.png");
        
        ModelObject* cribObject = new ModelObject("Crib", "../../models/cel_shading_scene/baby_crib.gltf", *cel_shading, glm::vec3(4.0, -1.0f, 6.0), 0.3f, glm::vec3(0.0f, 30.0f, 0.0));
        cribObject->setTexture("../../textures/cel_shading_scene/baby_crib.jpeg");
        
        ModelObject* cubesToyObject = new ModelObject("Cubes", "../../models/cel_shading_scene/cubes_toy.gltf", *cel_shading, glm::vec3(1.75f, -1.0f, 8.85f), 0.085f, glm::vec3(0.0, 0.0, 0.0));
        cubesToyObject->setTexture("../../textures/cel_shading_scene/cubes_toy.png");
        
        ModelObject* giraffeToyObject = new ModelObject("Giraffe", "../../models/cel_shading_scene/giraffe_toy.fbx", *cel_shading, glm::vec3(8.0f, -0.68f, 13.5f), 1.0f, glm::vec3(0.0, -140.0f, 0.0));
        giraffeToyObject->setTexture("../../textures/cel_shading_scene/giraffe_toy.jpeg");

        drawer_model = new Model("../../models/cel_shading_scene/drawer.fbx");

        ModelObject* drawerBlueObject = new ModelObject("Drawer Blue", drawer_model, *cel_shading, glm::vec3(8.5f, -1.0f, 10.0f), 0.65f, glm::vec3(0.0, -90.0f, 0.0));
        drawerBlueObject->setTexture("../../textures/cel_shading_scene/drawer_blue.png");

        ModelObject* drawerWoodObject = new ModelObject("Drawer Wood", drawer_model, *cel_shading, glm::vec3(5.0f, -1.0f, 14.0f), 0.65f, glm::vec3(0.0, 180.0f, 0.0));
        drawerWoodObject->setTexture("../../textures/cel_shading_scene/drawer_wood.png");

        screen_quad = new ScreenQuadObject;
    
        add_internal_object(floorObject);
        add_internal_object(wallObject_1);
        add_internal_object(wallObject_2);
        add_internal_object(wallObject_3);
        add_internal_object(wallObject_4);
        add_internal_object(wallObject_5);
        add_internal_object(bunnyObject);
        add_internal_object(sphereObject);
        add_internal_object(duckObject);
        add_internal_object(teddyObject);
        add_internal_object(teddyObject2);
        add_internal_object(spidermanObject);
        add_internal_object(cribObject);
        add_internal_object(cubesToyObject);
        add_internal_object(giraffeToyObject);
        add_internal_object(drawerBlueObject);
        add_internal_object(drawerWoodObject);

        // RBO
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);  
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // SCREEN FBO
        screen_fbo = new FrameBuffer(m_width, m_height, rbo);
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
            0, 0, m_width, m_height,
            0, 0, m_width, m_height,
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
                delete lut_cel_shading_texture;
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
    glm::vec3 lightPos0 = glm::vec3(5.0f, 2.0f, 10.0f);
    
    Shader* cel_shading;
    Shader* back_face_shader;
    Shader* screen_shader;

    Texture* lut_cel_shading_texture;

    Model* plane_model;
    Model* drawer_model;
    Texture* wall_texture;
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

        cel_shading.SetVec3("pointLightPosition", 1, glm::value_ptr(lightPos0));

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