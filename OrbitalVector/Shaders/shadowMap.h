//
//  h
//  OrbitalVector
//
//  Created by Si Li on 3/4/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__shadowMap__
#define __OrbitalVector__shadowMap__

#include <stdio.h>
#include "OGLShader.h"

class ShadowMapShader : public OGLShader
{
public:
    ShadowMapShader(GLenum drawType) : OGLShader(drawType) {}
    void init() override
    {
        depthTexture = true;
        loadShaders("simplePassthrough.vs", "depth.fs", true);
        setAttribute("position");
    }
    void renderPass(GLuint vao, GLuint fbName) override
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbName);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);
    }
};

#endif /* defined(__OrbitalVector__shadowMap__) */
