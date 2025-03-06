#pragma once

using namespace std;

#include <string>
#include <utils/shader.h>
#include <utils/texture.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

class DrawableObject
{
    public:
        DrawableObject(Shader& shader) : m_shader(shader), m_texture(nullptr)
        {}

        ~DrawableObject()
        {
            if(m_texture)
            {
                delete m_texture;
                m_texture = 0;
            }
        }

        Shader& shader()
        {
            return m_shader;
        }

        Texture* texture()
        {
            return m_texture;
        }

        string subroutine()
        {
            return m_subroutine;
        }

        void select_subroutine(string subroutine)
        {
            m_subroutine = subroutine;
        }

        void clear_subroutine()
        {
            m_subroutine.clear();
        }

        void setTexture(Texture* texture)
        {
            m_texture = texture;
        }

        virtual void draw(glm::mat4& view, glm::mat4& projection, Shader* = nullptr) = 0;

    protected:
        Shader& m_shader;
        string m_subroutine;
        Texture* m_texture;

        virtual void drawObject() = 0;
};