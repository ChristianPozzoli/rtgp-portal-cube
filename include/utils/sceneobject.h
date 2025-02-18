#pragma once

using namespace std;

#include <string>
#include <utils/shader.h>
#include <utils/model.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

class SceneObject
{
    public:
        SceneObject(glm::vec3 position = glm::vec3(0.0f), GLfloat scale = 1.0f, glm::vec3 rotation = glm::vec3(0.0f)) :
            m_modelMatrix(glm::mat4(1.0f)),
            m_position(position),
            m_rotation(rotation),
            m_scale(glm::vec3(scale)),
            m_color(glm::vec4(1.0f)),
            parent(nullptr)
        {
            children = new vector<SceneObject*>;
            updateModelMatrix();
        }

        SceneObject(SceneObject* parent, glm::vec3 position = glm::vec3(0.0f), GLfloat scale = 1.0f, glm::vec3 rotation = glm::vec3(0.0f)) :
            m_modelMatrix(glm::mat4(1.0f)),
            m_position(position),
            m_rotation(rotation),
            m_scale(glm::vec3(scale)),
            m_color(glm::vec4(1.0f)),
            parent(parent)
        {
            children = new vector<SceneObject*>;
            updateModelMatrix();
        }

        ~SceneObject()
        {
            delete children;
            children = nullptr;
            parent = nullptr;
        }

        glm::vec3 position()
        {
            return m_position;
        }

        glm::vec3 worldPosition()
        {
            if(parent)
            {
                glm::vec4 worldPosition = parent->modelMatrix() * glm::vec4(m_position, 1.0f);
                return worldPosition / worldPosition.w;
            }
            
            return m_position;
        }

        glm::vec3 scale()
        {
            return m_scale;
        }

        glm::vec3 rotation()
        {
            return m_rotation;
        }

        glm::vec4 color() {
            return m_color;
        }

        glm::mat4 modelMatrix() {

            if (!parent) return m_modelMatrix;

            if (dirty)
            {
                m_cached_composedModelMatrix = parent->modelMatrix() * m_modelMatrix;
                dirty = false;
            }

            return m_cached_composedModelMatrix;
        }

        void setPosition(glm::vec3 position)
        {
            m_position = position;
            updateModelMatrix();
        }

        void setRotation(glm::vec3 rotation)
        {
            m_rotation = rotation;
            updateModelMatrix();
        }

        void setScale(glm::vec3 scale)
        {
            m_scale = scale;
            updateModelMatrix();
        }

        void setScale(float scale)
        {
            m_scale = glm::vec3(scale);
            updateModelMatrix();
        }

        void setColor(glm::vec3 color) {
            m_color = glm::vec4(color, 1.0f);
        }

        void setColor(glm::vec4 color) {
            m_color = color;
        }

        void addChild(SceneObject* child)
        {
            if(child->parent == nullptr)
            {
                child->parent = this;
            }
            else if (child->parent != this)
            {
                return;
            }

            children->push_back(child);
        }

        void removeChild(SceneObject* child)
        {
            if(child->parent != this) return;

            children->erase(std::remove(children->begin(), children->end(), child), children->end());
            child->parent = nullptr;
        }

        void setParent(SceneObject* parent)
        {
            if(this->parent)
            {
                this->parent->removeChild(this);
            }

            if(!parent) return;
            
            this->parent = parent;
            parent->addChild(this);

            dirty = true;
        }

    protected:
        vector<SceneObject*>* children;
        SceneObject* parent;

    private:
        glm::mat4 m_modelMatrix;

        glm::vec3 m_position;
        glm::vec3 m_scale;
        glm::vec3 m_rotation;
        glm::vec4 m_color;
        
        bool dirty = true;
        glm::mat4 m_cached_composedModelMatrix;

        void updateModelMatrix()
        {
            m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position);
            m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(m_scale));

            dirty = true;
            for (auto child = children->begin(); child < children->end(); child++)
            {
                (*child)->dirty = true;
            }
        }
};