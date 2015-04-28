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
#include "OGLShader.h"
#include "orbit.h"
#include "cameraComponent.h"

#include "tiny_obj_loader.h"

extern std::vector<glm::vec3> barycenters;
extern std::vector<tinyobj::shape_t> shapes;
extern std::vector<tinyobj::material_t> materials;

class Scene {
public:
    void init();
    void update();
    
public:
    
};



#endif /* defined(__GLFW3_test__scene__) */
