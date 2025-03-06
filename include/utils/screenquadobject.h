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

class ScreenQuadObject
{
public:
    ScreenQuadObject()
    {
        GLfloat vertices[] = {
         1.0f,  1.0f,    1.0f, 1.0f, // Top Right
         1.0f, -1.0f,    1.0f, 0.0f, // Bottom Right
        -1.0f, -1.0f,    0.0f, 0.0f, // Bottom Left
        -1.0f,  1.0f,    0.0f, 1.0f  // Top Left
        };
        GLuint indices[] = {  // Note that we start from 0!
            0, 3, 1,  // First Triangle
            1, 3, 2   // Second Triangle
        };

        // VBO Vertex Buffer Obj
        // VAO Vertex Array Obj
        // EBO Element Buffer Obj

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        // -- Bind --
        // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

        // -- Unbinding --
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

        glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
    }

    void draw()
    {
        drawObject();
    }

    ~ScreenQuadObject()
    {}

private:
    GLuint VBO, VAO, EBO;

    void drawObject()
    {
        glBindVertexArray(VAO);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // Unbind from VAO
    }
};