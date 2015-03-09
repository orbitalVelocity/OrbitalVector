//
//  gameLogic.cpp
//  GLFW3_test
//
//  Created by Si Li on 10/2/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "gameLogic.h"
#include "ecs.h"

GameLogic::GameLogic(GLFWwindow *w, Scene &s, UserInput &i)
           : window(w), scene(s), userInput(i)
{
    activeShip = 0;
    deltaMove = 2;
    deltaRotate = 2;
    timeWarp = 1;
    //create Spatial objects for each thing FIXME not done yet
    sGlobe.resize(3);
    sGlobe[0].scale(glm::vec3(30));
    sGlobe[1].scale(glm::vec3(.05));
    sGlobe[2].scale(glm::vec3(.05));
    sOrbit.push_back(Spatial());
    sShip.push_back(Spatial());
    sShip[0].scale(glm::vec3(1));
}

//void GameLogic::linePick(vector<float> &shortestDist, int &closestObj)
void GameLogic::linePick()
{
    //line pick
    //code taken from
    //http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
    double mouseX, mouseY;
    int screenWidth, screenHeight;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    glfwGetWindowSize(window, &screenWidth, &screenHeight);
    
    //get ray casted under cursor
    mouseY = screenHeight - mouseY; //for some reason, mouseY is flipped from tutorial
    auto mouseX_NDC = ((float)mouseX/(float)screenWidth  - 0.5f) * 2.0f;
    auto mouseY_NDC = ((float)mouseY/(float)screenHeight - 0.5f) * 2.0f;
    glm::vec4 lRayStart_NDC(
                       mouseX_NDC, mouseY_NDC,
                       -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
                       1.0f
                       );
    glm::vec4 lRayEnd_NDC(
                     mouseX_NDC, mouseY_NDC,
                     0.0,
                     1.0f
                     );
    glm::mat4 M = glm::inverse(scene.camera.matrix());
    glm::vec4 lRayStart_world  = M * lRayStart_NDC;
    lRayStart_world /= lRayStart_world.w;
    glm::vec4 lRayEnd_world    = M * lRayEnd_NDC  ;
    lRayEnd_world   /= lRayEnd_world.w  ;
	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = glm::normalize(lRayDir_world);
    
    //iterate over all objects and find shortest distance
    shortestDist.clear();
    auto objIdx = 0;
    mouseHover = -1;
//    for (const auto &b : sys)   //TODO: optimize w/ octree
    for (auto i=0; i < getNumberOfEntities(); i++)
    {
        auto posNDC = scene.camera.matrix() * world * glm::vec4(getEntityPosition(i), 1.0);
        posNDC /= posNDC.w;
        auto mouseNDC = glm::vec2(mouseX_NDC * screenWidth, mouseY_NDC * screenHeight);
        auto screenPosNDC = glm::vec2(posNDC.x * screenWidth, posNDC.y * screenHeight);
        auto dist = glm::length(mouseNDC - screenPosNDC);
        shortestDist.push_back(dist);
        
        const int thresholdInPixels = 40;
        if (dist < thresholdInPixels) {
            if (userInput.lmbPressed)
                selected = objIdx;
            else {
                mouseHover = objIdx;
                cout << "mouse hover is: " << mouseHover << endl;
            }
        }
        
        objIdx++;
    }
}

//have gamelogic.update() call linePick
//have main reach in and display distance and debug stats

void GameLogic::missileLogic(float dt)
{
    //for each missile, aim at selected target and fire
    auto start = sysIndexOffset[BodyType::PROJECTILE];
    auto stop = start + numBodyPerType[BodyType::PROJECTILE];
//    for (int i = start; i < stop; i++)
    for (int i = 0; i < getNumberOfMissiles(); i++)
    {
        if (selected == i) {
            continue;
        }
        //find vector to target
        glm::vec3 targetVector = glm::normalize(sys[selected].sn.pos - getMissilePos(i));
        //add a bit of velocity in that direction
        float scale = 0.1;
        *getMissileVelocityPointer(i) += targetVector * scale;
//        sys[i].sn.vel += targetVector * scale;
        
        cout << "update projectile: " << i << endl;
    }
}

void GameLogic::userInteraction()
{
    linePick();
    
    double mx, my;
    static double prevMX = 0, prevMY = 0;
    glfwGetCursorPos(window, &mx, &my);     //consider consolidating this line w/ the one in linePick();
    double _x = mx - prevMX;
    double _y = my - prevMY;
    prevMX = mx;
    prevMY = my;
    double mouseScale = .1;
    
    if (userInput.rmbPressed) {
        scene.camera.rotate(_y*mouseScale, _x*mouseScale);
    } else if (userInput.lmbPressed) {
        sShip[0].rotate(-_x*mouseScale, _y*mouseScale, 0.0f);
    }
    
    //scroll behavior
    scene.camera.offsetPos(glm::vec3(0,0, -userInput.yScroll));
    userInput.yScroll = 0;
}

void GameLogic::handleCollision()
{
    if (true)
    {
        //contains elements to be removed
        std::vector<bool> markedForRemoval(sys.size(), false);
        
        //find collisions and mark candidates for deletion
        markForDeletion(sys, markedForRemoval);
        
        //remove all marked elements
        //FIXME: only works because first element never designed to be removed
        auto it = sys.end() - 1;
        for (int i = (int)sys.size()-1; i >= 0; --i, --it)
        {
            if (markedForRemoval[i]) {
                removeFromSys(it);
            }
        }
    }
 
}

void GameLogic::update(float dt)
{
    userInteraction();

    float gameDT = dt * timeWarp;

    missileLogic(gameDT);
    
    //calculate new position/velocity
//    auto sys = getAllOrbitalObjects();
//    orbitPhysicsUpdate(gameDT, ks, sys, false);
//    setAllOrbitalObjects(sys);
    updateOrbitalPhysics(dt, ks, false);
    
    //calculate trajectories every 30 frames
    static int orbitCount = 0;
    if (orbitCount++ % 30 == 0) {
        scene.orbit.update();
    }
    
    handleCollision();

    //update positions
    //central planet
    sGlobe[0].move(sys[0].sn.pos);
    //UI
    //prograde
    auto progradeOffset = glm::normalize(getMyShipVel());//sys[1].sn.vel);
    sGlobe[1].move(getMyShipPos()+progradeOffset);
    //retrograde
    sGlobe[2].move(getMyShipPos()-progradeOffset);
  
    //center the world around player ship
    world = glm::translate(glm::mat4(), -getMyShipPos());//sys[1].sn.pos);
}

void GameLogic::processActionList(std::vector<ActionType> &actionList)
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
        
        if (action == ActionType::newShip)
        {
            sShip.push_back(Spatial(200.0));    //Spatial constructor inserts body into sys already!
            sShip.back().scale(glm::vec3(1));
        }
        if (action == ActionType::fireGun)
        {
            double m = 0.0;
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
            body bullet(state(pos, vel), 10, gm, nullptr, BodyType::SHIP);
//            addSatellite(bullet);
            InsertToSys(bullet, BodyType::PROJECTILE);
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