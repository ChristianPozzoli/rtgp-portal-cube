#pragma once

using namespace std;

#include <unordered_map>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <utils/mesh.h>


class Model
{
public:
    vector<Mesh> meshes;

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    Model(Model&& move) = default;
    Model& operator=(Model&&) noexcept = default;

    Model(const string& path)
    {
        this->loadModel(path);
    }

    void Draw()
    {
        for (GLuint i = 0; i < this->meshes.size(); i++)
        {
            this->meshes[i].Draw();
        }
    }

private:
    void loadModel(string path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            cout << "assimp error!" << endl;
            return;
        }

        this->ProcessNode(scene->mRootNode, scene);
    }

    void ProcessNode(aiNode* node, const aiScene* scene)
    {
        for(GLuint i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            this->meshes.emplace_back(processMesh(mesh));
        }
        
        for(GLuint i = 0; i < node->mNumChildren; i++)
        {
            this->ProcessNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh)
    {
        vector<Vertex> vertices;
        vector<GLuint> indices;
        // unordered_map<GLuint, std::vector<GLuint>> verticeToFace;
        
		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace* face = &mesh->mFaces[i];

			for (GLuint j = 0; j < face->mNumIndices; j++)
			{
                GLuint vertexIndex = face->mIndices[j];
				indices.emplace_back(vertexIndex);
                // verticeToFace[vertexIndex].emplace_back(i);
			}
		}

        for(GLuint i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector;

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;

            if(mesh->mTextureCoords[0])
            {
                glm::vec2 v;
                v.x = mesh->mTextureCoords[0][i].x;
                v.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = v;

                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;

                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            // auto f = verticeToFace[i].begin();
            // auto end = verticeToFace[i].end();

            // vertex.AverageNormal = glm::vec3(0.0);
            // while (f != end) {
            //     const aiFace& face = mesh->mFaces[*f];
            //     bool found = false;
            //     for (GLuint j = 0; j < face.mNumIndices && !found; j++)
            //     {
            //         GLuint vertexIndex = face.mIndices[j];
            //         if(vertexIndex == i) {
            //             glm::vec3 n;
            //             n.x = mesh->mNormals[face.mIndices[j]].x;
            //             n.y = mesh->mNormals[face.mIndices[j]].y;
            //             n.z = mesh->mNormals[face.mIndices[j]].z;
            //             vertex.AverageNormal += n;
            //             found = true;
            //         }
            //     }

            //     ++f;
            // }

            // vertex.AverageNormal = glm::normalize(vertex.AverageNormal);

            vertices.emplace_back(vertex);
        }

        return Mesh(vertices, indices);
    }
};