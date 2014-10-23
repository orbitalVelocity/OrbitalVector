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

using namespace std;

class GameLogic{
public:
    float deltaMove, deltaRotate;
    float timeWarp;
    int activeShip;
    vector<Spatial> sGlobe, sOrbit, sShip;
public:
    GameLogic();
    void update(float dt);
    void processActionList(vector<ActionType> &actionList);
    void addSatellite(body &);
};
#endif /* defined(__GLFW3_test__gameLogic__) */
