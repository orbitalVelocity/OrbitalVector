//
//  input.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/29/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "input.h"

using namespace std;
UserInput * UserInput::event_handling_instance;

UserInput::UserInput()
{
    setEventHandling();
    globalReload = false;
    yScroll = 0;
    xScroll = 0;
    rmbPressed = false;
    lmbPressed = false;
}

void UserInput::key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (key) {
        case GLFW_KEY_A:
            actionList.push_back(ActionType::yawLeft);
            break;
        case GLFW_KEY_D:
            actionList.push_back(ActionType::yawRight);
            break;
        case GLFW_KEY_W:
            actionList.push_back(ActionType::pitchDown);
            break;
        case GLFW_KEY_S:
            actionList.push_back(ActionType::pitchUp);
            break;
        case GLFW_KEY_Q:
            actionList.push_back(ActionType::rollCCW);
            break;
        case GLFW_KEY_E:
            actionList.push_back(ActionType::rollCW);
            break;
            
        default:
            break;
    }
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_F:
                actionList.push_back(ActionType::transForward);
                break;
            case GLFW_KEY_G:
                actionList.push_back(ActionType::transBackward);
                break;
            case GLFW_KEY_COMMA:
                actionList.push_back(ActionType::timeWarpLess);
                break;
            case GLFW_KEY_PERIOD:
                actionList.push_back(ActionType::timeWarpMore);
                break;
            case GLFW_KEY_SPACE:
                actionList.push_back(ActionType::fireGun);
                break;
            case GLFW_KEY_T:
                actionList.push_back(ActionType::spawnMenu);
                break;
            case GLFW_KEY_B:
                globalShowFXAAAAirection ^= true;
                break;
            case GLFW_KEY_R:
                globalReload = true;
                break;
            case GLFW_KEY_1 :
                renderStage ^= stage1;
                break;
            case GLFW_KEY_2:
                renderStage ^= stage2;
                break;
            case GLFW_KEY_3:
                renderStage ^= stage3;
                break;
            case GLFW_KEY_4:
                renderStage ^= stage4;
                break;
            case GLFW_KEY_5:
                renderStage ^= stage5;
                break;
            case GLFW_KEY_6:
                renderStage ^= stage6;
                break;
            case GLFW_KEY_7:
                renderStage ^= stage7;
                break;
            case GLFW_KEY_8:
                renderStage ^= stage8;
                break;
            case GLFW_KEY_9:
                renderStage ^= stage9;
                break;
            case GLFW_KEY_0:
                renderStage ^= stage10;
                break;
                
            case GLFW_KEY_ENTER:
                actionList.push_back(ActionType::newShip);
                break;
        }
    }
}

void UserInput::mb(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            rmbPressed = true;
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            lmbPressed = true;
        }
    } else {
        rmbPressed = false;
        lmbPressed = false;
    }
}

void UserInput::scroll(GLFWwindow* window, double xoffset, double yoffset)
{
    yScroll = yoffset;
    xScroll = xoffset;
}