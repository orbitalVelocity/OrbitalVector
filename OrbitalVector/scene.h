//
//  scene.h
//  GLFW3_test
//
//  Created by Si Li on 9/14/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __GLFW3_test__scene__
#define __GLFW3_test__scene__

#include <iostream>
#include "includes.h"
#include "ogl.h"
#include "orbit.h"
#include "camera.h"

#include "tiny_obj_loader.h"

extern std::vector<tinyobj::shape_t> shapes;
extern std::vector<tinyobj::material_t> materials;

//Scene graph handles managements of all physical objects
//objects in real space (ships, planets, projectiles)
//NOT UI
//invokes game logic
class Scene {
public:
    Scene() :
    orbit(GL_LINES)
        {
        };
    void init(int, int);
    void update();
    
public:
    friend class GameLogic;
    friend class UserInput;
    friend class Renderer;
    
    Camera camera;
    glm::vec3 lightPos;
    
    bool debug;
    glm::vec3 rayStart, rayEnd;
    glm::vec3 obj;
    
    Orbit orbit;
};



#endif /* defined(__GLFW3_test__scene__) */
