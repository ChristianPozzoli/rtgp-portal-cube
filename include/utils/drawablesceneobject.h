#pragma once

using namespace std;

#include <string>
#include <utils/shader.h>
#include <utils/sceneobject.h>
#include <utils/drawableobject.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

class DrawableSceneObject : public DrawableObject, public SceneObject
{
    public:
        DrawableSceneObject(Shader& shader, glm::vec3 position = glm::vec3(0.0f), GLfloat scale = 1.0f, glm::vec3 rotation = glm::vec3(0.0f)) :
            DrawableObject(shader),
            SceneObject(position, scale, rotation)
        {}

        ~DrawableSceneObject()
        {}

        void draw(glm::mat4& view, glm::mat4& projection, Shader* override_shader = nullptr)
        {
            Shader* shader;
            if(override_shader)
            {
                shader = override_shader;
                override_shader->Use();
            }
            else
            {
                shader = &m_shader;
                m_shader.Use(/*m_subroutine*/);
            }
            
            glUniformMatrix4fv(glGetUniformLocation(shader->Program, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(shader->Program, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(view));

            glm::mat4 mmatrix = modelMatrix();
            
            glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(view * mmatrix));
            glUniformMatrix4fv(glGetUniformLocation(shader->Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(mmatrix));
            glUniformMatrix3fv(glGetUniformLocation(shader->Program, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
            
            GLint colorLocation = glGetUniformLocation(shader->Program, "colorIn");
            if(colorLocation != -1) {
                glUniform3fv(colorLocation, 1, glm::value_ptr(color()));
            }

            Texture* tex = texture();
            float textured = tex && tex->name() != 0;

            GLint texturedLocation = glGetUniformLocation(shader->Program, "textured");
            if(texturedLocation != -1)
            {
                glUniform1f(texturedLocation, textured);
            }

            if(textured)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tex->name());
            }

            drawObject();
        }

    protected:
        virtual void drawObject() = 0;
};