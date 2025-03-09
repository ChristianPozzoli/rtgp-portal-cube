#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <string>
#include <utils/camera.h>
#include <utils/shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

using namespace std;

class ShaderScene
{
public:
    ShaderScene(std::string name, GLint width, GLint height) : m_name(name), m_width(width), m_height(height)
    {
        external_objects = new vector<DrawableSceneObject*>;
        internal_objects = new vector<DrawableSceneObject*>;
    }

    ~ShaderScene()
    {
        delete external_objects;

        for (vector<DrawableSceneObject*>::iterator iter = internal_objects->begin(); iter < internal_objects->end(); ++iter)
        {
            DrawableSceneObject* object = *iter;
            remove_internal_object(object);
            delete object;
        }

        delete internal_objects;
    }

    virtual void setup_scene() {}

    virtual void update_window(GLFWwindow* window, GLint width, GLint height)
    {
        m_width = width;
        m_height = height;
        cout << "[" << m_name << "] New size: " << width << ", " << height << endl;
    }

    virtual void update_scene(Camera* camera, glm::mat4& view, glm::mat4& projection, Shader* override_shader = nullptr)
    {}

    void add_external_object(DrawableSceneObject* obj)
    {
        external_objects->push_back(obj);
    }

    void remove_external_object(DrawableSceneObject* obj)
    {
        external_objects->erase(std::remove(external_objects->begin(), external_objects->end(), obj), external_objects->end());
    }

    
    virtual void drawImGui()
    {
        if (!ImGui::TreeNode((m_name + " objects").c_str()))
        {
            return;
        }

        for (vector<DrawableSceneObject*>::iterator iter = external_objects->begin(); iter < external_objects->end(); ++iter)
        {
            DrawableSceneObject* object = *iter;
            object->drawImGui();
        }
        for (vector<DrawableSceneObject*>::iterator iter = internal_objects->begin(); iter < internal_objects->end(); ++iter)
        {
            DrawableSceneObject* object = *iter;
            object->drawImGui();
        }

        ImGui::TreePop();
    }

    virtual void draw(glm::mat4& view, glm::mat4& projection, Shader* override_shader = nullptr)
    {
        draw_objects(view, projection, override_shader);
    }

protected:
    GLint m_width;
    GLint m_height;
    const string SHADER_PATH = "../../shaders/";

    std::string m_name;
    vector<DrawableSceneObject*>* external_objects;
    vector<DrawableSceneObject*>* internal_objects;

    void draw_objects(glm::mat4& view, glm::mat4& projection, Shader* override_shader = nullptr)
    {
        for (vector<DrawableSceneObject*>::iterator iter = external_objects->begin(); iter < external_objects->end(); ++iter)
        {
            DrawableSceneObject* object = *iter;
            object->draw(view, projection, override_shader);
        }
        for (vector<DrawableSceneObject*>::iterator iter = internal_objects->begin(); iter < internal_objects->end(); ++iter)
        {
            DrawableSceneObject* object = *iter;
            object->draw(view, projection, override_shader);
        }
    }

    void add_internal_object(DrawableSceneObject* obj)
    {
        internal_objects->push_back(obj);
    }

    void remove_internal_object(DrawableSceneObject* obj)
    {
        internal_objects->erase(std::remove(internal_objects->begin(), internal_objects->end(), obj), internal_objects->end());
    }
};