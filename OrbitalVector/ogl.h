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
        for (auto &shaderID : shaderIDs)
            glDeleteShader(shaderID);
        
        for (auto &v : vbo)
            glDeleteBuffers(1, &v);
        glDeleteBuffers(1, &elementBuffer);
        
        glDeleteVertexArrays(1, &vao);
    }
    void init();
    void loadShaders(string vs, string fs, bool useProg=true);
    void newProgram(map<GLuint, string> &shaders, bool useProg=true);
    void loadIco();
    void loadGrid();
    void loadAttrib(string name, vector<float> &input,
                    GLuint hint, GLuint type=GL_ARRAY_BUFFER);
    void update();
    void drawIndexed(glm::mat4 &model,
                     glm::vec3 &color,
                     GLuint* indices);
    void drawIndexed(glm::mat4 &world,
                     Camera &camera,
                     glm::mat4 &mvp,
                     GLuint* indices);
    void drawIndexed(glm::mat4 &world,
                     Camera &camera,
                     glm::vec3 light, glm::mat4 &mvp,
                     glm::vec3 color, GLuint* indices);
    void draw(glm::mat4 &camera, glm::vec3 color);
    void draw(glm::mat4 &camera);
public:
    GLuint programPipeline;
    GLuint shaderProgram;
    GLuint vsProgram, fsProgram;
    vector<GLuint> shaderIDs;
    GLuint vao, elementBuffer;
    vector<GLuint> vbo;
    int vboIdx;
    GLenum drawType;
    int drawCount;
    
    GLuint textProgram;
    GLint uniform_tex;
    GLint uniform_color;
    GLint attribute_coord;

    bool depthTexture;
#if TRANSFORM
#endif
};

#endif /* defined(__GLFW3_test__ogl__) */
