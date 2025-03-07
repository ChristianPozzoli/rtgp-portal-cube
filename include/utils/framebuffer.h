#pragma once

#include <iostream>
#include <glad/glad.h>

using namespace std;

class FrameBuffer {
public:
    FrameBuffer(GLint width, GLint height, GLuint rbo) : m_rbo(rbo), m_fbo_name(0), m_texture_name(0)
    {
        glGenFramebuffers(1, &m_fbo_name);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_name);

        glGenTextures(1, &m_texture_name);
        glBindTexture(GL_TEXTURE_2D, m_texture_name);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_name, 0);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            cout << "ERROR: Framebuffer is not complete" << endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ~FrameBuffer() {
        glDeleteFramebuffers(1, &m_fbo_name);
        glDeleteTextures(1, &m_texture_name);

        m_fbo_name = 0;
        m_texture_name = 0;
    }

    GLuint fbo_name() {
        return m_fbo_name;
    }

    GLuint rbo_name() {
        return m_rbo;
    }

    GLuint texture_name() {
        return m_texture_name;
    }

    void bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_name);
    }

    void bind_read() {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo_name);
    }

    void bind_draw() {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo_name);
    }

    void unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

private:
    GLuint m_fbo_name;
    GLuint m_rbo;
    GLuint m_texture_name;
};