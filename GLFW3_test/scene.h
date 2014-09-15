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

using namespace std;

//Scene graph handles managements of all (?) objects
//invokes game logic
class Scene {
public:
    Scene(){};
public:
    vector<OGL> renderables;
    
};

#endif /* defined(__GLFW3_test__scene__) */
