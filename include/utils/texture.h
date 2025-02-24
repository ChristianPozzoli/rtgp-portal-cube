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

class Texture
{
    public:
        Texture(const char* path)
        {
            setupTexture(path, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST);
        }
        
        Texture(const char* path, int wrap_st, int min_filter, int mag_filter)
        {
            setupTexture(path, wrap_st, min_filter, mag_filter);
        }

        ~Texture()
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
        
        void setupTexture(const char* path, int wrap_st, int min_filter, int mag_filter)
        {
            int w, h, channels; // channels = how many color channel are in the image
            unsigned char* image;

            image = stbi_load(path, &w, &h, &channels, STBI_rgb);
            
            if(image == nullptr)
            {
                std::cout << "Failed to load image!" << std::endl;
                return;
            }

            glGenTextures(1, &m_textureName);
            glBindTexture(GL_TEXTURE_2D, m_textureName);

            if(channels == 3)
            {
                // level argument = mipmap level, 0 with only one mipmap
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            }
            else if(channels == 4)
            {
                // level argument = mipmap level, 0 with only one mipmap
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            }
            else
            {
                std::cout << "Wrong number of channel in texture!" << std::endl;
                glDeleteTextures(1, &m_textureName);
                m_textureName = 0;
                stbi_image_free(image);
                return;
            }
            
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_st);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_st);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
            
            stbi_image_free(image);

            glBindTexture(GL_TEXTURE_2D, 0);
        }
};