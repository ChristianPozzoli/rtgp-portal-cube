#pragma once

#include <iostream>
#include <glad/glad.h>

using namespace std;

class FrameBuffer {
public:
    FrameBuffer(GLint width, GLint height, GLuint rbo) : m_rbo(rbo), m_fbo(0), m_texture(0)
    {
        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            cout << "ERROR: Framebuffer is not complete" << endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ~FrameBuffer() {
        glDeleteFramebuffers(1, &m_fbo);
        glDeleteTextures(1, &m_texture);

        m_fbo = 0;
        m_texture = 0;
    }

    GLuint fbo() {
        return m_fbo;
    }

    GLuint rbo() {
        return m_rbo;
    }

    GLuint texture() {
        return m_texture;
    }

    void bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }

    void unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

private:
    GLuint m_fbo;
    GLuint m_rbo;
    GLuint m_texture;
};