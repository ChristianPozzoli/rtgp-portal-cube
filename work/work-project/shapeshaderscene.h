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
        delete skybox_shader;
        delete screen_shader;

        delete screen_fbo;
        delete screen_quad;

        delete shapeParent;
        delete skyboxCubeObject;
        delete skyCubeMap;

        glDeleteRenderbuffers(1, &rbo);

        for (auto i = models.begin(); i != models.end(); ++i)
        {
            delete (*i);
        }
    }

    void setup_scene() override
    {
        illum_shader = new Shader(
            (SHADER_PATH + "illumination_model.vert").c_str(),
            (SHADER_PATH + "illumination_model.frag").c_str()
        );
        skybox_shader = new Shader(
            (SHADER_PATH + "skybox.vert").c_str(),
            (SHADER_PATH + "skybox.frag").c_str()
        );
        screen_shader = new Shader(
            (SHADER_PATH + "screen.vert").c_str(),
            (SHADER_PATH + "screen.frag").c_str()
        );

        skyCubeMap = new TextureCubeMap("../../textures/cube/space/", ".png");
        
        floorObject = new ModelObject("Floor", "../../models/plane.obj", *illum_shader, glm::vec3(0.0f, -1.0f, 0.0f), 10.0f);
        floorObject->setColor(glm::vec3(0.33f));

        Model* sphereModel = new Model("../../models/sphere.obj");
        Model* cubeModel = new Model("../../models/cube.obj");
        Model* coneModel = new Model("../../models/shapes_scene/cone.fbx");
        Model* exagonModel = new Model("../../models/shapes_scene/exagon.fbx");
        Model* tetrahedronModel = new Model("../../models/shapes_scene/tetrahedron.fbx");
        Model* dodecahedronModel = new Model("../../models/shapes_scene/dodecahedron.fbx");
        Model* octahedronModel = new Model("../../models/shapes_scene/octahedron.fbx");
        Model* mengerSpongeModel = new Model("../../models/shapes_scene/menger_sponge.fbx");
        Model* pyramidStepsModel = new Model("../../models/shapes_scene/pyramid_steps.fbx");
        Model* octahedronTruncModel = new Model("../../models/shapes_scene/octahedron_trunc.fbx");
        Model* tetrahedronTruncModel = new Model("../../models/shapes_scene/tetrahedron_trunc.fbx");

        models.push_back(sphereModel);
        models.push_back(cubeModel);
        models.push_back(coneModel);
        models.push_back(exagonModel);
        models.push_back(tetrahedronModel);
        models.push_back(dodecahedronModel);
        models.push_back(octahedronModel);
        models.push_back(mengerSpongeModel);
        models.push_back(pyramidStepsModel);
        models.push_back(octahedronTruncModel);
        models.push_back(tetrahedronTruncModel);
        
        skyboxCubeObject = new ModelObject("SkyboxCube_1", cubeModel, *skybox_shader);
        
        ModelObject* sphereObject = new ModelObject("Sphere_1", sphereModel, *illum_shader, glm::vec3(0.0f, 1.0f, -10.0f));
        sphereObject->setScale(glm::vec3(1.5f, 1.0f, 0.5f));
        ModelObject* cubeObject = new ModelObject("Cube_1", cubeModel, *illum_shader, glm::vec3(-5.0f, 1.0f, -10.0f));
        ModelObject* coneObject = new ModelObject("Cone_1", coneModel, *illum_shader, glm::vec3(5.0f, 1.0f, -10.0f));
        ModelObject* exagonObject = new ModelObject("Exagon_1", exagonModel, *illum_shader, glm::vec3(5.0f, 1.0f, -10.0f), 1.5f);
        ModelObject* tetrahedronObject = new ModelObject("Tetrahedron_1", tetrahedronModel, *illum_shader, glm::vec3(5.0f, 1.0f, -10.0f), 1.5f);
        ModelObject* dodecahedronObject = new ModelObject("Dodecahedron_1", dodecahedronModel, *illum_shader, glm::vec3(5.0f, 1.0f, -10.0f), 1.5f);
        ModelObject* octahedronObject = new ModelObject("Octahedron_1", octahedronModel, *illum_shader, glm::vec3(5.0f, 1.0f, -10.0f), 1.5f);
        ModelObject* mengerSpongeObject = new ModelObject("MengerSponge_1", mengerSpongeModel, *illum_shader, glm::vec3(5.0f, 1.0f, -10.0f), 1.5f);
        ModelObject* pyramidStepsObject = new ModelObject("PyramidSteps_1", pyramidStepsModel, *illum_shader, glm::vec3(5.0f, 1.0f, -10.0f), 1.5f);
        ModelObject* octahedronTruncObject = new ModelObject("OctahedronTrunc_1", octahedronTruncModel, *illum_shader, glm::vec3(5.0f, 1.0f, -10.0f), 1.5f);
        ModelObject* tetrahedronTruncObject = new ModelObject("TetrahedronTrunc_1", tetrahedronTruncModel, *illum_shader, glm::vec3(5.0f, 1.0f, -10.0f), 1.5f);
        
        add_internal_object(floorObject);
        add_internal_object(sphereObject);
        add_internal_object(cubeObject);
        add_internal_object(coneObject);
        add_internal_object(exagonObject);
        add_internal_object(tetrahedronObject);
        add_internal_object(dodecahedronObject);
        add_internal_object(octahedronObject);
        add_internal_object(mengerSpongeObject);
        add_internal_object(pyramidStepsObject);
        add_internal_object(octahedronTruncObject);
        add_internal_object(tetrahedronTruncObject);
        
        float step = 2 * glm::pi<float>() / (internal_objects->size() - 1);
        glm::vec3 center = glm::vec3(5.0f, 0.2f, 10.0f);
        float distance = 20.0f;
        float current_step = 0;
        
        glm::vec3 K = glm::vec3(1.0, 2.0 / 3.0, 1.0 / 3.0);
        
        shapeParent = new SceneObject("", center);

        for (auto i = internal_objects->begin(); i != internal_objects->end(); ++i)
        {
            if (*i != floorObject)
            {
                DrawableSceneObject& obj = **i;
                shapeParent->addChild(&obj);
                obj.setPosition(glm::vec3( distance * glm::cos(current_step), 1.0f + (rand() % 5),  distance * glm::sin(current_step)));
                current_step += step;
                glm::vec3 rotation = obj.position() * 10.0f + GLfloat(rand());
                obj.setRotation(rotation);
                
                float hue = 4 * current_step / glm::pi<float>();
                glm::vec3 p = abs(glm::fract(hue + K) * 6.0f - 3.0f);
                obj.setColor(0.5f * glm::clamp(p - K.x, 0.0f, 1.0f));
            }
        }

        
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

    void update_scene(Camera* camera, glm::mat4& view, glm::mat4& projection, GLfloat deltaTime, bool is_main_scene = false) override
    {
        for (auto i = internal_objects->begin(); i != internal_objects->end(); ++i) {
            if (*i != floorObject) {
                DrawableSceneObject* obj = *i;
                
                glm::vec3 rotation = obj->rotation();
                rotation = glm::mod(rotation + deltaTime * 50, 360.0f);
                obj->setRotation(rotation);
            }
        }

        for (auto i = external_objects->begin(); i != external_objects->end(); ++i) {
            DrawableSceneObject* obj = *i;
            
            glm::vec3 rotation = obj->rotation();
            rotation = glm::mod(rotation + deltaTime * 50, 360.0f);
            obj->setRotation(rotation);
        }

        glm::vec3 rotation = shapeParent->rotation();
        rotation.y = glm::mod(rotation.y + deltaTime * 10, 360.0f);
        shapeParent->setRotation(rotation);

        glEnable(GL_DEPTH_TEST);
        
        setup_illum_shader(*illum_shader);
        illum_shader->SetFloat("time", glfwGetTime());
        
        // RENDER ON SCREEN FBO
        screen_fbo->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        draw_objects(view, projection);

        skybox_shader->Use();
        skybox_shader->SetInt("tCube", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyCubeMap->name());
        glDepthFunc(GL_LEQUAL);
        skyboxCubeObject->draw(glm::mat4(glm::mat3(view)), projection);
        glDepthFunc(GL_LESS);
        
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
    Shader* skybox_shader;
    Shader* screen_shader;

    FrameBuffer* screen_fbo;

    ScreenQuadObject* screen_quad;

    GLuint rbo;
    
    SceneObject* shapeParent;
    TextureCubeMap* skyCubeMap;
    ModelObject* skyboxCubeObject;
    ModelObject* floorObject;
    vector<Model*> models;

    glm::vec3 lightPos0 = glm::vec3(5.0f, 10.0f, 10.0f);
    glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 ambientColor = glm::vec3(0.1f, 0.1f, 0.1f);
    GLfloat Kd = 1.25f;
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