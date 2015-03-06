//
//  fxaa.h
//  OrbitalVector
//
//  Created by Si Li on 3/5/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_fxaa_h
#define OrbitalVector_fxaa_h
#include "OGLShader.h"

class FXAAShader: public OGLShader
{
public:
    FXAAShader(GLenum drawType) : OGLShader(drawType) {}
    void init() override
    {
        loadShaders("passthrough.vs", "fxaa.fs", true);
        //        loadAttribute("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
        setAttribute("position");
    }
    void renderPass(GLuint vao, GLuint frameBufferName, int fbWidth, int fbHeight, GLuint renderedTexture)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);
        
        glUseProgram(shaderProgram);
        auto loc = glGetUniformLocation(shaderProgram, "forwardTexture");
        glUniform1i(loc, 0);
        loc = glGetUniformLocation(shaderProgram, "resolution");
        float resolution[2] = {(float)fbWidth, (float)fbHeight};
        glUniform2f(loc, resolution[0], resolution[1]);
        loc = glGetUniformLocation(shaderProgram, "showDir");
        glUniform1i(loc, globalShowFXAAAAirection);
        
        glBindVertexArray(vao);
        glDisable(GL_BLEND);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_BLEND);
        check_gl_error();

    }
public:
    GLuint renderedTexture;
};
#endif
