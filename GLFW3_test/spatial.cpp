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
void Spatial::rotate(float dx, float dy)
{
    y += dy;
    x += dx;
    x = (x > 360) ? x - 360 : x;
    y = (y > 180) ? 180 : y;
    y = (y < 0) ? 0 : y;
    
    orientation = glm::rotate(glm::mat4(), -y, glm::vec3(1.0f, 0.0f, 0.0f));
    orientation = glm::rotate(orientation, -x, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Spatial::rotate(glm::mat4 &m)
{
    orientation = glm::rotate(m, -90.0f, glm::vec3(1, 0, 0));
}

glm::mat4 Spatial::transform()
{
    return position * size * orientation;
}