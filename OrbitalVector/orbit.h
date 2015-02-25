//
//  orbit.h
//  GLFW3_test
//
//  Created by Si Li on 9/16/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

/*
 * Orbit class is a specialized OGL class (should be a subclass really)
 *
 * Refactor Goal:
 *      Make Orbit a subclass of OGL
 * States:
 *      same stuff as OGL
 *
 * Functions:
 *      update/recompute trajectory
 *      draw
 *
 */

#ifndef __GLFW3_test__orbit__
#define __GLFW3_test__orbit__

#include "includes.h"
#include "EntityManager.h"
#include "rk547m.h"
#include <iostream>
#include <set>
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
    bool nextMesh();
    void calcTrajectory(int &pathSteps);
    void draw(glm::mat4 &camera, glm::vec3 color);
public:
    GLuint fragmentShader, shaderProgram, vertexShader;
    GLuint vbo, vao;
    GLenum drawType;
    
    GLuint textProgram;
    GLint uniform_tex;
    GLint uniform_color;
    GLint attribute_coord;
    
    vector<vector<float> > paths;
    float x,y;
    int drawCount;
    float apo, peri;
    glm::vec3 apoPos, periPos;
    
//    vector<vector<state> > ks;
    EntityManager *entityManager;
};
#endif /* defined(__GLFW3_test__orbit__) */
