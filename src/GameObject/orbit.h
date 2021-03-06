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
#include "entityx/Entity.h"
#include <iostream>
#include <set>
#include "includes.h"

class RenderableOrbit : public OGLShader
{
    //FIXME: delete when refactored
    glm::mat4 transform;
public:
    RenderableOrbit(GLenum _drawType);
    
    void init() override;
    void update(entityx::EntityManager &);
    void draw(glm::mat4 &camera, glm::vec3 color);
    void draw(GLuint vao, int drawCount, glm::mat4 &camera, glm::vec3 color);
    
};
#endif /* defined(__GLFW3_test__orbit__) */
