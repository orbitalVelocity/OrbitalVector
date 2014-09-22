//
//  orbit.h
//  GLFW3_test
//
//  Created by Si Li on 9/16/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __GLFW3_test__orbit__
#define __GLFW3_test__orbit__

#include "includes.h"
#include "rk547m.h"
#include <iostream>
using namespace std;

class Orbit
{
public:
    Orbit(GLenum _drawType);
    ~Orbit()
    {
        glDeleteProgram(shaderProgram);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        
        glDeleteBuffers(1, &vbo);
        
        glDeleteVertexArrays(1, &vao);
    }
    void init();
    void newProgram(map<GLuint, string> &shaders);
    void loadPath();
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
    
    
    float x,y;
    int drawCount;
};

#endif /* defined(__GLFW3_test__orbit__) */
