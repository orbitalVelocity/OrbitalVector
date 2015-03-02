//
//  gameLogic.h
//  GLFW3_test
//
//  Created by Si Li on 10/2/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __GLFW3_test__gameLogic__
#define __GLFW3_test__gameLogic__

#include <iostream>
#include "includes.h"
#include "input.h"
#include "spatial.h"
#include "rk547m.h"
#include "scene.h"
#include "input.h"


using namespace std;


class GameLogic{
public:
    float deltaMove, deltaRotate;
    float timeWarp;
    int activeShip;
    vector<Spatial> sGlobe, sOrbit, sShip;
    GLFWwindow *window;
    Scene &scene;
    UserInput &userInput;
    //UI related
    int selected, mouseHover; //sysIdx
    //debug related
    vector<float> shortestDist;
public:
    GameLogic(GLFWwindow*, Scene &s, UserInput &i) ;
    void update(float dt);
    
    /**
     processes list of actions
     @param actionList &actionList      list of actions
     */
    void processActionList(vector<ActionType> &actionList);
    void addSatellite(body &);
    void userInteraction();
    void linePick();//vector<float> &, int &);
    void missileLogic(float dt);
    void handleCollision();
};
#endif /* defined(__GLFW3_test__gameLogic__) */
