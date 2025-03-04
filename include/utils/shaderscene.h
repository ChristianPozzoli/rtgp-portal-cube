#pragma once

using namespace std;

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <string>
#include <utils/scene.h>
#include <utils/camera.h>
#include <utils/shader.h>
#include <utils/drawablesceneobject.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

class ShaderScene
{
public:
    ShaderScene(std::string name) : m_name(name)
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

    virtual void setup_scene(GLFWwindow* window, int width, int height) {}

    virtual void update_scene(Camera* camera, glm::mat4& view, glm::mat4& projection, Shader* override_shader = nullptr)
    {
        draw(view, projection);
    }

    virtual void delete_scene() {}

    void add_external_object(DrawableSceneObject* obj) {
        external_objects->push_back(obj);
    }

    void remove_external_object(DrawableSceneObject* obj) {
        external_objects->erase(std::remove(external_objects->begin(), external_objects->end(), obj), external_objects->end());
    }

    
    virtual void drawImGui() {
        if (ImGui::CollapsingHeader((m_name + " objects").c_str()))
        {
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
        }
    }

protected:
    std::string m_name;
    vector<DrawableSceneObject*>* external_objects;
    vector<DrawableSceneObject*>* internal_objects;

    void add_internal_object(DrawableSceneObject* obj) {
        internal_objects->push_back(obj);
    }

    void remove_internal_object(DrawableSceneObject* obj) {
        internal_objects->erase(std::remove(internal_objects->begin(), internal_objects->end(), obj), internal_objects->end());
    }
    void draw(glm::mat4& view, glm::mat4& projection, Shader* override_shader = nullptr)
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
};