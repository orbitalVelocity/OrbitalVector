//
//  camera.h
//  GLFW3_test
//
//  Created by Si Li on 9/14/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __GLFW3_test__camera__
#define __GLFW3_test__camera__

#include <iostream>
#include "includes.h"

class Camera {
public:
    Camera();
    void setPosition(const glm::vec3 pos);
    glm::vec3 getPosition() const;
    void setFocus(const glm::vec3 focus);
    void setClip(float near, float far);
    void offsetPos(const glm::vec3 offset);
    void rotate(float up, float right);
    void setFOV(float f);
    float getFOV() const;
    void setAspectRatio(float ar);
    glm::vec3 forward() const;
    glm::vec3 right() const;
    glm::vec3 up() const;
    glm::mat4 orientation() const;
    glm::mat4 matrix() const;
    glm::mat4 projection() const;
    glm::mat4 view() const;
    
public:
    glm::vec3 position;
    glm::vec3 focus;
    float fov;
    float hAngle, vAngle;
    float nearPlane, farPlane;
    float ratio;
    
};
#endif /* defined(__GLFW3_test__camera__) */
