//
//  spatial.h
//  GLFW3_test
//
//  Created by Si Li on 9/21/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __GLFW3_test__spatial__
#define __GLFW3_test__spatial__

#include "includes.h"

class Spatial
{
public:
    Spatial() : x(0), y(90) {
        yAxis = glm::vec3(1.0f, 0, 0);
        xAxis = glm::vec3(0, 1.0f, 0);
    }
    void move(glm::vec3 move);
    void scale(glm::vec3 move);
    void rotate(glm::mat4 &m);
    void rotate(float, float);
    glm::mat4 transform();
public:
    glm::mat4 position;
    glm::mat4 size;
    glm::mat4 orientation;
    
    float x, y;
    glm::vec3 xAxis, yAxis;
};
#endif /* defined(__GLFW3_test__spatial__) */
