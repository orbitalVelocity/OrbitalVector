//
//  spatial.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/21/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "spatial.h"
#include "stdlib.h"
#include "time.h"

Spatial::Spatial(float r) : x(0), y(90) {
    yAxis = glm::vec3(1.0f, 0, 0);
    xAxis = glm::vec3(0, 1.0f, 0);
    zAxis = glm::vec3(0, 0, 1.0f);
    
    double m = 7e12;
    double G = 6.673e-11;
    double gm = m * G;
    
    srand ((unsigned int)time(NULL));
    
    r = (rand() / 1000) % 300;
    if (r < 50) {
        r = 50;
    }
    float v = std::sqrt(gm/r);
    glm::vec3 rad(r, 0, 0);
    glm::vec3 vel(0, 0, v);
    cout << "new ship: r: " << r << ", v: " << v << endl;
    m = 1e1;
    gm = m * G;
    m_body = body(state(rad, vel),
                    gm,
                    20,
                    nullptr,
                    BodyType::SHIP
                    );

//    InsertToSys(tmp, BodyType::SHIP);

}

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