#pragma once

using namespace std;

#include <string>
#include <utils/shader.h>
#include <utils/model.h>
#include <utils/drawablesceneobject.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

class ModelObject : public DrawableSceneObject
{
public:
    ModelObject(std::string name, std::string modelPath, Shader& shader, glm::vec3 position = glm::vec3(0.0f), GLfloat scale = 1.0f, glm::vec3 rotation = glm::vec3(0.0f)) :
        DrawableSceneObject(name, shader, position, scale, rotation)
    {
        m_model = new Model(modelPath);
    }

    ~ModelObject()
    {
        delete m_model;
    }

    Model& model() {
        return *m_model;
    }

private:
    Model* m_model;

    void drawObject()
    {
        m_model->Draw();
    }
};