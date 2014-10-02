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
#include "camera.h"
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
        
        for (auto &v : vbo)
            glDeleteBuffers(1, &v);
        glDeleteBuffers(1, &elementBuffer);
        
        glDeleteVertexArrays(1, &vao);
    }
    void init();
    void loadShaders(string vs, string fs);
    void newProgram(map<GLuint, string> &shaders);
    void loadIco();
    void loadGrid();
    void loadAttrib(string name, vector<float> &input,
                    GLuint hint, GLuint type=GL_ARRAY_BUFFER);
    void update();
    void drawIndexed(Camera &camera, glm::vec3 light, glm::mat4 &mvp,
                     glm::vec3 color, GLuint* indices);
    void draw(glm::mat4 &camera, glm::vec3 color);
public:
    GLuint fragmentShader, shaderProgram, vertexShader;
    GLuint vao, elementBuffer;
    vector<GLuint> vbo;
    int vboIdx;
    GLenum drawType;
    int drawCount;
    
    GLuint textProgram;
    GLint uniform_tex;
    GLint uniform_color;
    GLint attribute_coord;

#if TRANSFORM
#endif
};

#endif /* defined(__GLFW3_test__ogl__) */
