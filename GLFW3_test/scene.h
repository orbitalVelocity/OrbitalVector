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
#include "gameLogic.h"
#include "input.h"


using namespace std;

//Scene graph handles managements of all (?) objects
//invokes game logic
class Scene {
public:
    Scene(GameLogic *_gl, UserInput *_ui)
        : orbit(GL_LINES),
          globe(GL_TRIANGLES),
          grid(GL_LINEAR_ATTENUATION),
          ship(GL_TRIANGLES),
          _gameLogic(_gl),
          _userInput(_ui)
        {};
    void init(int, int);
    void render();
    void update();
    void setActiveShip(int s) { shipIdx = s; }
    
public:
    int shipIdx;
    GLFWwindow *window;
    vector<OGL> renderables;
    Orbit orbit;
    OGL globe, grid, ship;
    glm::vec3 lightPos;
    Camera camera;
    friend class GameLogic;
    GameLogic *_gameLogic;
    friend class UserInput;
    UserInput *_userInput;
};

#endif /* defined(__GLFW3_test__scene__) */
