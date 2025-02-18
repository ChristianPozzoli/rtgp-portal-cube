#pragma once

using namespace std;


#include <utils/shader.h>
#include <glm/glm.hpp>
#include <glad/glad.h>

class LineDrawer
{
public:
    LineDrawer(glm::mat4&, glm::mat4&);

    void draw(glm::vec3, glm::vec3, glm::vec3 = glm::vec3(1.0f), glm::mat4& = glm::mat4(1.0f));

    ~LineDrawer();

private:
    Shader* line_shader = nullptr;
    glm::mat4& m_view;
    glm::mat4& m_projection;
};