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
        DrawableObject(Shader& shader) : m_shader(shader), m_texture(nullptr), m_tex_repetition(0), imgui_tex_repetition(0)
        {}

        ~DrawableObject()
        {
            if(m_texture && !external_texture)
            {
                delete m_texture;
            }
            m_texture = 0;
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

        GLfloat texture_repetition() {
            return m_tex_repetition;
        }

        void select_subroutine(string subroutine)
        {
            m_subroutine = subroutine;
        }

        void clear_subroutine()
        {
            m_subroutine.clear();
        }

        void setTexture(Texture* texture, GLfloat repetition = 1)
        {
            if(m_texture && !external_texture)
            {
                delete m_texture;
            }
            external_texture = true;
            m_texture = texture;
            m_tex_repetition = repetition;
            imgui_tex_repetition = m_tex_repetition;
        }

        void setTexture(char* texture_path, GLfloat repetition = 1)
        {
            if(m_texture && !external_texture)
            {
                delete m_texture;
            }
            external_texture = false;
            m_texture = new Texture(texture_path);
            m_tex_repetition = repetition;
            imgui_tex_repetition = m_tex_repetition;
        }
        
        void setTextureRepetition(GLfloat repetition) {
            m_tex_repetition = repetition;
            imgui_tex_repetition = m_tex_repetition;
        }

        virtual void draw(glm::mat4& view, glm::mat4& projection, Shader* = nullptr) = 0;

    protected:
        Shader& m_shader;
        string m_subroutine;
        Texture* m_texture;
        bool external_texture;

        GLfloat imgui_tex_repetition;

        virtual void drawObject() = 0;
    private:
        GLfloat m_tex_repetition;
};