//
//  Highpass.h
//  OrbitalVector
//
//  Created by Si Li on 3/4/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_Highpass_h
#define OrbitalVector_Highpass_h

#include "OGLShader.h"

class HighpassShader: public OGLShader
{
public:
    HighpassShader(GLenum drawType) : OGLShader(drawType) {}
    void init() override
    {
        loadShaders("passthrough.vs", "highpass.fs", true);
//        loadAttribute("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
        setAttribute("position");
    }
    void renderPass(GLuint vao, GLuint frameBufferName, GLuint renderedTexture)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
        
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);
        check_gl_error();
        
        auto loc = glGetUniformLocation(shaderProgram, "renderedTexture");
        glUniform1i(loc, 0);
        
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
        check_gl_error();
    }
public:
    GLuint renderedTexture;
};
#endif
