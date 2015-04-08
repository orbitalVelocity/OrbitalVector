//
//  composite.h
//  OrbitalVector
//
//  Created by Si Li on 3/5/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_composite_h
#define OrbitalVector_composite_h
#include "OGLShader.h"

class CompositeShader: public OGLShader
{
public:
    CompositeShader(GLenum drawType) : OGLShader(drawType) {}
    void init() override
    {
        loadShaders("passthrough.vs", "highpass.fs", true);
        //        loadAttribute("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
        setAttribute("position");
    }
    void renderPass(GLuint vao, GLuint frameBufferName, GLuint renderedTexture0, GLuint renderedTexture1, GLuint renderedTexture2)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderedTexture0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, renderedTexture1);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, renderedTexture2);
        
        glUseProgram(shaderProgram);
        auto loc = glGetUniformLocation(shaderProgram, "tex0");
        glUniform1i(loc, 0);
        loc = glGetUniformLocation(shaderProgram, "tex1");
        glUniform1i(loc, 1);
        loc = glGetUniformLocation(shaderProgram, "tex2");
        glUniform1i(loc, 2);
        loc = glGetUniformLocation(shaderProgram, "enable");
        for (auto & r : rtForComp)
            if (r != 0) {
                r = 1;
            }
        glUniform1fv(loc, 3, rtForComp.data());
        
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        check_gl_error();
    }
public:
    GLuint renderedTexture;
    std::vector<float> rtForComp;
};
#endif
