#pragma once

using namespace std;

#include <utils/linedrawer.h>

#include <utils/shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

LineDrawer::LineDrawer(glm::mat4& view, glm::mat4& projection) : m_view(view), m_projection(projection)
{
    line_shader = new Shader("../../shaders/basic.vert", "../../shaders/fullcolor.frag");
}

GLuint VBO, VAO;

GLfloat vertices[] = {
        0, 0, 0,
        0, 0, 0
    };

void LineDrawer::draw(glm::vec3 start, glm::vec3 end, glm::vec3 color, glm::mat4& model)
{
    if (
        start.x != vertices[0] || start.y != vertices[1] || start.z != vertices[2] ||
        end.x != vertices[3] || end.y != vertices[4] || end.z != vertices[5]
    )
    {
        vertices[0] = start.x;
        vertices[1] = start.y;
        vertices[2] = start.z;
        vertices[3] = end.x;
        vertices[4] = end.y;
        vertices[5] = end.z;
        
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        
        // -- Bind --
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        
        // -- Unbinding --
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    line_shader->Use();
    
    glUniformMatrix4fv(glGetUniformLocation(line_shader->Program, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(m_projection));
    glUniformMatrix4fv(glGetUniformLocation(line_shader->Program, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(m_view));
    glUniformMatrix4fv(glGetUniformLocation(line_shader->Program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(line_shader->Program, "colorIn"), 1, glm::value_ptr(color));
    
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}

LineDrawer::~LineDrawer()
{
    line_shader->Delete();
    delete line_shader;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}