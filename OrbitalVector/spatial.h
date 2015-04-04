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
#include "rk547m.h"
/*
 * container for size and orientation, along with convenient functions
 * Refactor Goal: get rid of this, 
 *      size and orientations should be separate collections
 *      wrapper class around those individually
 */
class Spatial
{
public:
    Spatial() : x(0), y(00) {
        yAxis = glm::vec3(1.0f, 0, 0);
        xAxis = glm::vec3(0, 1.0f, 0);
        zAxis = glm::vec3(0, 0, 1.0f);
    }

    Spatial(float r);
    void move(glm::vec3 move);
    void scale(glm::vec3 move);
    void rotate(glm::mat4 &m);
    void rotate(float, float, float);
    glm::mat4 transform();
public:
    glm::mat4 position;
    glm::mat4 size;
    glm::mat4 orientation;
    
    body m_body;
    float x, y;
    glm::vec3 xAxis, yAxis, zAxis;
};
#endif /* defined(__GLFW3_test__spatial__) */
