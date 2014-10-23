//
//  gameLogic.cpp
//  GLFW3_test
//
//  Created by Si Li on 10/2/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "gameLogic.h"

using namespace std;

GameLogic::GameLogic()
{
    activeShip = 0;
    deltaMove = 2;
    deltaRotate = 2;
    timeWarp = 1;
    //create Spatial objects for each thing FIXME not done yet
    sGlobe.resize(3);
    sGlobe[0].scale(glm::vec3(10));
    sGlobe[1].scale(glm::vec3(.05));
    sGlobe[2].scale(glm::vec3(.05));
    sOrbit.push_back(Spatial());
    sShip.push_back(Spatial());
    sShip[0].scale(glm::vec3(.001));
}

void GameLogic::update(float dt)
{
    //calculate new position/velocity
    float gameDT = dt * timeWarp;
    orbitDelta(gameDT, ks, sys, false);
    
    //central planet
    sGlobe[0].move(sys[0].sn.pos);
    //UI
    //prograde
    auto progradeOffset = glm::normalize(sys[1].sn.vel);
    sGlobe[1].move(sys[1].sn.pos+progradeOffset);
    //retrograde
    sGlobe[2].move(sys[1].sn.pos-progradeOffset);
}

void GameLogic::processActionList(vector<ActionType> &actionList)
{
    glm::vec3 forwardVector;
    for (auto &action : actionList )
    {
        switch (action) {
            case ActionType::transForward:
                forwardVector = glm::vec3(sShip[activeShip].orientation * glm::vec4(0, 0, 1, 1));
                sys[1].incCustom(.1, forwardVector);
                break;
            case ActionType::yawLeft:
                sShip[activeShip].rotate(deltaMove, 0, 0);
                break;
            case ActionType::yawRight:
                sShip[activeShip].rotate(-deltaMove, 0, 0);
                break;
            case ActionType::pitchDown:
                sShip[activeShip].rotate(0, -deltaMove, 0);
                break;
            case ActionType::pitchUp:
                sShip[activeShip].rotate(0, deltaMove, 0);
                break;
            case ActionType::rollCCW:
                sShip[activeShip].rotate(0, 0, -deltaMove);
                break;
            case ActionType::rollCW:
                sShip[activeShip].rotate(0, 0, deltaMove);
                break;
            case ActionType::timeWarpMore:
                timeWarp *= 2;
                break;
            case ActionType::timeWarpLess:
                timeWarp /= 2;
                break;
            default:
                break;
        }
        
        
        if (action == ActionType::fireGun)
        {
            double m = 1e-42;
            double G = 6.673e-11;
            double gm = m * G;
            auto shipVector = glm::vec3(sShip[activeShip].orientation * glm::vec4(0,0,1,1));
            cout << "ship orientation: " << printVec3(shipVector) << "\n";
            auto pos = sys[1].sn.pos
            + glm::normalize(shipVector)
            * 10.0f;
            auto vel = sys[1].sn.vel
            + glm::normalize(shipVector)
            * 3.0f;
            body bullet(state(pos, vel), 10, gm, nullptr, objType::SHIP);
            addSatellite(bullet);
        }

    }
    actionList.clear();
}

void GameLogic::addSatellite(body &b)
{
    sys.push_back(b);
    for (auto &k : ks)
    {
        k.clear();
        k.resize(sys.size());
    }
}