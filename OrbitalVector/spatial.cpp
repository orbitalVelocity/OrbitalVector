//
//  spatial.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/21/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "spatial.h"

void Spatial::scale(const glm::vec3 s)
{
    size = glm::scale(glm::mat4(), s);
}
void Spatial::move(glm::vec3 m)
{
    position = glm::translate(glm::mat4(), m);
}
void Spatial::rotate(float dx, float dy, float dz)
{
    y += dy;
    x += dx;
//    x = (x > 360) ? x - 360 : x;
//    y = (y > 180) ? 180 : y;
//    y = (y < 0) ? 0 : y;
#if 0
    auto rotateYAxis = glm::rotate(glm::mat4(), -dx, xAxis);
    yAxis = glm::vec3(rotateYAxis * glm::vec4(yAxis, 1.0f));
    auto rotateXAxis = glm::rotate(glm::mat4(), -dy, yAxis);
    xAxis = glm::vec3(rotateXAxis * glm::vec4(xAxis, 1.0f));
    orientation = glm::rotate(glm::mat4(), -x, xAxis);
    orientation = glm::rotate(orientation, -y, yAxis);
#else
    auto orientation2 = glm::rotate(glm::mat4(), dx, xAxis);
    orientation2 = glm::rotate(orientation2, -dy, yAxis);
    orientation2 = glm::rotate(orientation2, dz, zAxis);
    orientation = orientation * orientation2;
#endif
}

void Spatial::rotate(glm::mat4 &m)
{
    orientation = glm::rotate(m, -90.0f, glm::vec3(1, 0, 0));
}

glm::mat4 Spatial::transform()
{
    return position * size * orientation;
}