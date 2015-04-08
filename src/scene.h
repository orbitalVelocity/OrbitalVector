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
#include "camera.h"

#include "tiny_obj_loader.h"

extern std::vector<tinyobj::shape_t> shapes;
extern std::vector<tinyobj::material_t> materials;

/**
 Contains game state for a mission, menus, etc
 
 Used for campaign missions, skermishes, and full screen menus
    such as save/load/new game menus
    
 @code
 OR make menus a different thing entirely
 */
class Scene {
public:
    void init();
    void update();
    
public:
    

    //lights are entities with self emitters
        //brightness,           type: cone/bulbs/area/cubic
        //range (rendering optimization) in meters
        //falloff (inverse)     linear, square
    
    
    //no need for these classes! get rid of them!
    friend class UserInput;
    friend class Renderer;
    
};



#endif /* defined(__GLFW3_test__scene__) */
