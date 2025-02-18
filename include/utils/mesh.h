#pragma once

using namespace std;

#include <vector> // C++ Vector library

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Mesh {
public:
    vector<Vertex> vertices;
    vector<GLuint> indices;

    GLuint VAO;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(vector<Vertex>& vertices, vector<GLuint>& indices) noexcept : vertices(std::move(vertices)), indices(std::move(indices))
    {
        this->setupMesh();
    }

    Mesh(Mesh&& move) noexcept :
        vertices(std::move(move.vertices)),
        indices(std::move(move.indices)),
        VAO(std::move(move.VAO)),
        VBO(std::move(move.VBO)),
        EBO(std::move(move.EBO))
    {
        move.VAO = 0;
    }

    Mesh& operator=(Mesh&& move) noexcept
    {
        freeGPUResources();

        if(move.VAO)
        {
            vertices = std::move(move.vertices);
            indices = std::move(move.indices);
            VAO = move.VAO;
            VBO = move.VBO;
            EBO = move.EBO;

            move.VAO = 0;
        }
        else
        {
            VAO = 0;
        }

        return *this;
    }

    ~Mesh() noexcept
    {
        freeGPUResources();
    }

    void Draw()
    {
        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // Bind to nothing, clears binding
    }

private:
    GLuint VBO, EBO;

    void setupMesh() {
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        glGenBuffers(1, &this->EBO);
        
        // VAO is made "active"    
        glBindVertexArray(this->VAO);
        
        // we copy data in the VBO - we must set the data dimension, and the pointer to the structure cointaining the data
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
        
        // we copy data in the EBO - we must set the data dimension, and the pointer to the structure cointaining the data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

        // we set in the VAO the pointers to the vertex positions (with the relative offsets inside the data structure)
        // these will be the positions to use in the layout qualifiers in the shaders ("layout (location = ...)"")    
        int attributeLocation = 0;
        glEnableVertexAttribArray(attributeLocation);
        glVertexAttribPointer(attributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
        
        attributeLocation = 1;
        glEnableVertexAttribArray(attributeLocation);
        glVertexAttribPointer(attributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
        
        attributeLocation = 2;
        glEnableVertexAttribArray(attributeLocation);
        glVertexAttribPointer(attributeLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
        
        attributeLocation = 3;
        glEnableVertexAttribArray(attributeLocation);
        glVertexAttribPointer(attributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
        
        attributeLocation = 4;
        glEnableVertexAttribArray(attributeLocation);
        glVertexAttribPointer(attributeLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));
        
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
        glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
    }

    void freeGPUResources()
    {
        if(this->VAO)
        {
            glDeleteVertexArrays(1, &this->VAO);
            glDeleteBuffers(1, &this->VBO);
            glDeleteBuffers(1, &this->EBO);
        }
    }
};

