//
//  orbit.h
//  GLFW3_test
//
//  Created by Si Li on 9/16/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __GLFW3_test__orbit__
#define __GLFW3_test__orbit__

#include "OGLShader.h"
#include "rk547m.h"
#include <iostream>
#include <set>
using namespace std;

class RenderableOrbit : public OGLShader
{
public:
    RenderableOrbit(GLenum _drawType);
    
    void init() override;
    void loadPath();
    void update();
//    bool nextMesh();
    void calcTrajectory(int &pathSteps);
    void draw(glm::mat4 &camera, glm::vec3 color);
public:
    
    vector<vector<float> > paths;
    float x,y;
    float apo, peri;
    glm::vec3 apoPos, periPos;
};
#endif /* defined(__GLFW3_test__orbit__) */
