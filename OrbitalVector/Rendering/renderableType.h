//
//  renderableType.h
//  GLFW3_test
//
//  Created by Si Li on 9/7/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __GLFW3_test__renderableType__
#define __GLFW3_test__renderableType__

#include "includes.h"
#include "rk547m.h"
#include "camera.h"
#include <iostream>
using namespace std;

class RenderableType
{
public:
    RenderableType(GLenum _drawType);
    ~RenderableType();
    
    virtual void init(){}//FIXME: make full virtual
    void loadShaders(string vs, string fs, bool useProg=true);
    void newProgram(map<GLuint, string> &shaders, bool useProg=true);
    
    virtual void loadAsset(){}//FIXME: make full virtual
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
};

#endif /* defined(__GLFW3_test__renderableType__) */
