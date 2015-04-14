//
//  input.h
//  GLFW3_test
//
//  Created by Si Li on 9/29/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __GLFW3_test__input__
#define __GLFW3_test__input__

#include <iostream>
#include "includes.h"
#include <GLFW/glfw3.h>
#include "camera.h"


enum class ActionType {
    pitchUp, pitchDown,
    yawLeft, yawRight,
    rollCCW, rollCW,
    
    transForward, transBackward,
    transLeft, transRight,
    transUp, transDown,
    
    timeWarpMore, timeWarpLess,
    
    fireGun, fireMissile, fireBeam,
    
    newShip,
    spawnMenu,
    invalid
};

class UserInput
{
public:
    static UserInput *event_handling_instance;
    /* Contains all data members for user input states */
    float yScroll, xScroll;
    float xMouse, yMouse;
    bool lmbPressed, rmbPressed;
    std::vector<ActionType> actionList;
    
    //is this bad? probably
    //FIXME: put this else where that makes more sense
    float timeWarp = 1.0;
public:
    UserInput();
    void setEventHandling() {
        if (event_handling_instance)
            assert(false && "already exists an input instance");
        event_handling_instance = this;
    }
    
    void key(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    void mb(GLFWwindow* window, int button, int action, int mods);
    
    void scroll(GLFWwindow* window, double xoffset, double yoffset);
    
    static void key_callback_dispatch(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (event_handling_instance)
            event_handling_instance->key(window, key, scancode, action, mods);
    }
    static void mb_callback_dispatch(GLFWwindow* window, int button, int action, int mods) {
        if (event_handling_instance)
            event_handling_instance->mb(window, button, action, mods);
    }
    static void scroll_callback_dispatch(GLFWwindow* window, double xoffset, double yoffset) {
        if (event_handling_instance)
            event_handling_instance->scroll(window, xoffset, yoffset);
    }
};
#endif /* defined(__GLFW3_test__input__) */
