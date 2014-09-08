//
//  ogl.h
//  GLFW3_test
//
//  Created by Si Li on 9/7/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __GLFW3_test__ogl__
#define __GLFW3_test__ogl__

#include "includes.h"
#include <iostream>

class OGL
{
public:
    OGL();
    ~OGL()
    {
        glDeleteProgram(shaderProgram);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        
        glDeleteBuffers(1, &vbo);
        
        glDeleteVertexArrays(1, &vao);
    }
public:
    GLuint fragmentShader, shaderProgram, vertexShader;
    GLuint vbo, vao;
    
    GLuint textProgram;
    GLint uniform_tex;
    GLint uniform_color;
    GLint attribute_coord;
};

#endif /* defined(__GLFW3_test__ogl__) */
