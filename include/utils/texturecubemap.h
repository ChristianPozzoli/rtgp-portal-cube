#pragma once

using namespace std;

#include <string>
#include <utils/shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#endif

class TextureCubeMap
{
    public:
        TextureCubeMap(const char* path_root, const char* file_extension)
        {
            setupTexture(path_root, file_extension, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
        }
        
        TextureCubeMap(const char* path_root, const char* file_extension, int wrap_str, int min_filter, int mag_filter)
        {
            setupTexture(path_root, file_extension, wrap_str, min_filter, mag_filter);
        }

        ~TextureCubeMap()
        {
            if(!m_textureName) return;

            glDeleteTextures(1, &m_textureName);
            m_textureName = 0;
        }

        GLuint name()
        {
            return m_textureName;
        }

        void setWrapS(int wrap_s)
        {
            glBindTexture(GL_TEXTURE_2D, m_textureName);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void setWrapT(int wrap_t)
        {
            glBindTexture(GL_TEXTURE_2D, m_textureName);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void setWrapR(int wrap_r)
        {
            glBindTexture(GL_TEXTURE_2D, m_textureName);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrap_r);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void setMinFilter(int min_filter)
        {
            glBindTexture(GL_TEXTURE_2D, m_textureName);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void setMagFilter(int mag_filter)
        {
            glBindTexture(GL_TEXTURE_2D, m_textureName);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

    private:
        GLuint m_textureName = 0;

        void setUpSideTexture(std::string path, GLuint side_name)
        {
            int w, h, channels; // channels = how many color channel are in the image
            unsigned char* image;
            
            image = stbi_load(path.c_str(), &w, &h, &channels, STBI_default);
            
            if(image == nullptr)
            {
                std::cout << "Failed to load cubemap image!" << std::endl;
                return;
            }

            if(channels == 3)
            {
                // level argument = mipmap level, 0 with only one mipmap
                glTexImage2D(side_name, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            }
            else if(channels == 4)
            {
                // level argument = mipmap level, 0 with only one mipmap
                glTexImage2D(side_name, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            }
            else
            {
                std::cout << "Wrong number of channel in cubemap texture!" << std::endl;
            }

            stbi_image_free(image);
        }
        
        void setupTexture(const char* path_root, const char* file_extension, int wrap_str, int min_filter, int mag_filter)
        {
            glGenTextures(1, &m_textureName);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureName);

            setUpSideTexture(path_root + std::string("posx") + file_extension, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
            setUpSideTexture(path_root + std::string("negx") + file_extension, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
            setUpSideTexture(path_root + std::string("posy") + file_extension, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
            setUpSideTexture(path_root + std::string("negy") + file_extension, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
            setUpSideTexture(path_root + std::string("posz") + file_extension, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
            setUpSideTexture(path_root + std::string("negz") + file_extension, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, min_filter);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mag_filter);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap_str);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap_str);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap_str);

            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
};