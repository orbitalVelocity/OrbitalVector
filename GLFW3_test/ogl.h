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
#include "rk547m.h"
#include <iostream>
using namespace std;

class OGL
{
public:
    OGL(GLenum _drawType);
    ~OGL()
    {
        glDeleteProgram(shaderProgram);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        
        glDeleteBuffers(1, &vbo);
        
        glDeleteVertexArrays(1, &vao);
    }
    void init();
    void newProgram(map<GLuint, string> &shaders);
    void update(float, float);
    void move(glm::vec3 move);
    void scale(glm::vec3 move);
    void loadIco();
    void loadGrid();
    void update();
    void draw(glm::mat4 &camera, glm::vec3 color);
public:
    GLuint fragmentShader, shaderProgram, vertexShader;
    GLuint vbo, vao;
    GLenum drawType;
    
    GLuint textProgram;
    GLint uniform_tex;
    GLint uniform_color;
    GLint attribute_coord;
    
    glm::mat4 position;
    glm::mat4 size;
    glm::mat4 orientation;
    float x,y;
    int drawCount;
#if TRANSFORM
#endif
};

#endif /* defined(__GLFW3_test__ogl__) */
