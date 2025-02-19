#pragma once

using namespace std;

#include <string>
#include <utils/shader.h>
#include <utils/drawablesceneobject.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

class Scene
{
public:
    Scene()
    {
        objects = new vector<DrawableSceneObject*>;
    }

    ~Scene()
    {
        delete objects;
    }

    void add_object(DrawableSceneObject* obj) {
        objects->push_back(obj);
    }

    void remove_object(DrawableSceneObject* obj) {
        objects->erase(std::remove(objects->begin(), objects->end(), obj), objects->end());
    }

    void draw(glm::mat4& view, glm::mat4& projection, Shader* override_shader = nullptr)
    {
        for (vector<DrawableSceneObject*>::iterator iter = objects->begin(); iter < objects->end(); iter++)
        {
            DrawableSceneObject* object = *iter;
            object->draw(view, projection, override_shader);
        }
    }

    void drawImGui() {}

private:
    vector<DrawableSceneObject*>* objects;

    void drawObject() {}
};