//
//  blur.h
//  OrbitalVector
//
//  Created by Si Li on 3/5/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_blur_h
#define OrbitalVector_blur_h
#include "OGLShader.h"
using namespace glm;

class BlurShader: public OGLShader
{
public:
    BlurShader(GLenum drawType) : OGLShader(drawType), offsetx(0.0), offsety(0.0) {}
    void init() override
    {
        loadShaders("passthrough.vs", "bloom.fs", true);
        //        loadAttribute("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
        setAttribute("position");
    }
    void setOffsets(float x, float y)
    {
        offsetx = x;
        offsety = y;
    }
    void renderPass(GLuint vao, GLuint frameBufferName, GLuint renderedTexture)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferName);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //TODO: can uniforms be set during initialization?
        glUseProgram(shaderProgram);
        auto loc = glGetUniformLocation(shaderProgram, "renderedTexture");
        glUniform1i(loc, 0);
        
        assert(offsetx != offsety && "offsets should be asymetrical for 1D blur");
        loc = glGetUniformLocation(shaderProgram, "offset");
        glUniform2fv(loc, 1, value_ptr(vec2(offsetx, offsety)));
        
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        check_gl_error();
    }
public:
    GLuint renderedTexture;
    float offsetx, offsety;
};

#endif
