//
//  camera.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/14/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "camera.h"

Camera::Camera() :
    position(0, 10, 10),
    hAngle(0),
    vAngle(0),
    nearPlane(10e-3f),
    farPlane(10e4f),
    ratio(16.0f/9.0f),
    fov(45.0f)
{
}

void Camera::setPosition(const glm::vec3 pos)
{
    position = pos;
}

//glm::vec3 Camera::getPosition() const {
//    auto pos = glm::rotateX(position, -vAngle);
//    pos = glm::rotateY(pos, -hAngle);
//    
//    return pos;
//}

void Camera::setFocus(const glm::vec3 f)
{
    focus= f;
}



void Camera::offsetPos(const glm::vec3 offset)
{
    // TODO: change these constants based on OS.
#ifdef __APPLE__
    float scaling = 0.03f;
#elif WIN32
    float scaling = 0.03f;
#else
    float scaling = 0.18f;
#endif    
    position += offset*glm::length(position)*scaling;
    if (glm::length(position) < 4.0f) position *= 4.0f/glm::length(position);
}

float Camera::getFOV() const
{
    return fov;
}

void Camera::setFOV(float f)
{
    assert(fov > 0.0f && fov < 180.0f);
    fov = f;
}

void Camera::setClip(float near, float far)
{
    nearPlane = near;
    farPlane = far;
}

void Camera::setAspectRatio(float ar)
{
    ratio = ar;
}

void Camera::rotate(float up, float right)
{
    vAngle += up;
    hAngle += right;
    
    hAngle = (hAngle > 2*M_PI) ? hAngle - 2*M_PI: hAngle;
    vAngle = (vAngle > 0) ? 0 : vAngle;
    vAngle = (vAngle < -M_PI) ? -M_PI: vAngle;
}

glm::mat4 Camera::orientation() const {
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, vAngle, glm::vec3(1,0,0));
    orientation = glm::rotate(orientation, hAngle, glm::vec3(0,0,1));
    //rotate around forward vector
    //orientation = glm::rotate(orientation, rAngle, forward());
    return orientation;
}

glm::vec3 Camera::forward() const {
    glm::vec4 forward = glm::inverse(orientation()) * glm::vec4(0,0,-10,1);
    return glm::vec3(forward);
}

glm::vec3 Camera::right() const {
    glm::vec4 right = glm::inverse(orientation()) * glm::vec4(10,0,0,1);
    return glm::vec3(right);
}

glm::vec3 Camera::up() const {
    glm::vec4 up = glm::inverse(orientation()) * glm::vec4(0,0,10,1);
    return glm::vec3(up);
}

glm::vec3 Camera::getPosition() const {
    return glm::vec3(orientation() * glm::vec4(position, 1.0));
}
glm::mat4 Camera::matrix() const {
    return projection() * view();
}

glm::mat4 Camera::projection() const {
    return glm::perspective(fov, ratio, nearPlane, farPlane);
}

glm::mat4 Camera::view() const {
    return glm::translate(glm::mat4(), -position) * orientation();
}








