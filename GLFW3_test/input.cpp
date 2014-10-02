//
//  input.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/29/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "input.h"
#include "includes.h"
#include <GLFW/glfw3.h>
#include "camera.h"

using namespace std;

class UserInput
{
public:
    /* Contains all data members for user input states */
    float yScroll, xScroll;
    float xMouse, yMouse;
    
public:
    static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
#if 0
        if (key == GLFW_KEY_F && action == GLFW_PRESS)
            sys[1].incCustom(.1, cameraVector);
        if (key == GLFW_KEY_G && action == GLFW_PRESS)
            sys[1].incPrograde(-.1);
        if (key == GLFW_KEY_COMMA && action == GLFW_PRESS)
            timeWarp /= 2;
        if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS)
            timeWarp *= 2;
        
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        {
            double m = 1e-42;
            double G = 6.673e-11;
            double gm = m * G;
            auto pos = sys[1].sn.pos + glm::normalize(camera.forward()) * 10.0f;
            auto vel = sys[1].sn.vel + glm::normalize(camera.forward()) * 3.0f;
            body bullet(state(pos, vel), 10, gm, nullptr, objType::SHIP);
            addSatellite(bullet);
        }
#endif

    }
#if 0
    bool lmbPressed, rmbPressed;
    static void mb(GLFWwindow* window, int button, int action, int mods)
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
    
    static void scroll(GLFWwindow* window, double xoffset, double yoffset)
    {
        yScroll = yoffset;
        fov -= yScroll;
        fov = (fov < 10 ) ? 10 : fov;
        fov = (fov > 120) ? 120 : fov;
        xScroll = xoffset;
    }
#endif
};