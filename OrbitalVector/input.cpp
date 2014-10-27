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
}

void UserInput::key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        actionList.push_back(ActionType::transForward);
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
        actionList.push_back(ActionType::transBackward);
    if (key == GLFW_KEY_COMMA && action == GLFW_PRESS)
        actionList.push_back(ActionType::timeWarpLess);
    if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS)
        actionList.push_back(ActionType::timeWarpMore);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        actionList.push_back(ActionType::fireGun);

    if (key == GLFW_KEY_A)// && action == GLFW_PRESS)
        actionList.push_back(ActionType::yawLeft);
	if (key == GLFW_KEY_D)// && action == GLFW_PRESS)
        actionList.push_back(ActionType::yawRight);
	if (key == GLFW_KEY_W)// && action == GLFW_PRESS)
        actionList.push_back(ActionType::pitchDown);
	if (key == GLFW_KEY_S)// && action == GLFW_PRESS)
        actionList.push_back(ActionType::pitchUp);
	if (key == GLFW_KEY_Q)// && action == GLFW_PRESS)
        actionList.push_back(ActionType::rollCCW);
	if (key == GLFW_KEY_E)// && action == GLFW_PRESS)
        actionList.push_back(ActionType::rollCW);
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
        globalShowFXAAAAirection ^= true;
	if (key == GLFW_KEY_R)// && action == GLFW_PRESS)
        globalReload = true;
    
	if (key == GLFW_KEY_1  && action == GLFW_PRESS)// && action == GLFW_PRESS)
        renderStage ^= stage1;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)// && action == GLFW_PRESS)
        renderStage ^= stage2;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)// && action == GLFW_PRESS)
        renderStage ^= stage3;
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)// && action == GLFW_PRESS)
        renderStage ^= stage4;
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)// && action == GLFW_PRESS)
        renderStage ^= stage5;
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)// && action == GLFW_PRESS)
        renderStage ^= stage6;
	if (key == GLFW_KEY_7 && action == GLFW_PRESS)// && action == GLFW_PRESS)
        renderStage ^= stage7;
	if (key == GLFW_KEY_8 && action == GLFW_PRESS)// && action == GLFW_PRESS)
        renderStage ^= stage8;
	if (key == GLFW_KEY_9 && action == GLFW_PRESS)// && action == GLFW_PRESS)
        renderStage ^= stage9;
	if (key == GLFW_KEY_0 && action == GLFW_PRESS)// && action == GLFW_PRESS)
        renderStage ^= stage10;
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