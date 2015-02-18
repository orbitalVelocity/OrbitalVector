//
//  gameLogic.cpp
//  GLFW3_test
//
//  Created by Si Li on 10/2/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "gameLogic.h"

using namespace std;
using namespace glm;

void initSys(EntityManager &em)
{
    auto planetCM = em.getComponentManager(Family::GRAV);
    auto shipCM   = em.getComponentManager(Family::SHIP);
    //hardcoded, need to go into object creation code
    double m = 7e12;
    double G = 6.673e-11;
    double gm = m * G;
#define oldway 0
#if oldway
    sys.push_back(body(state(glm::vec3(), glm::vec3(0, 0, -.1)),
                       gm,
                       10,
                       nullptr,
                       Family::GRAV
                       )
                  );
#else
    auto planet = body(state(glm::vec3(), glm::vec3(0, 0, -.1) ),
                       gm, 40
                       );
//    InsertToSys(planet, Family::GRAV);
    planetCM.addEntry(planet);
#endif
    
    
    glm::vec3 rad(110, 0, 0);
    glm::vec3 vel(0, 0, 2.3);
    m = 1e5;
    gm = m * G;
    
#if oldway
    sys.push_back(body(state(rad, vel),
                       gm,
                       1,
                       nullptr,
                       Family::SHIP
                       )
                  );
#else
    auto ship = body(state(rad, vel),
                     gm, 3
                     );
//    InsertToSys(ship, Family::SHIP);
    shipCM.addEntry(ship);
#endif
}

GameLogic::GameLogic(GLFWwindow *w, Scene *s, UserInput *i)
           : window(w), _scene(s), _userInput(i)
{
    activeShip = 0;
    deltaMove = 2;
    deltaRotate = 2;
    timeWarp = 1;
    //create Spatial objects for each thing FIXME not done yet
#if OLDWAY
    sGlobe.resize(3);
    sGlobe[0].scale(glm::vec3(30));
    sGlobe[1].scale(glm::vec3(.05));
    sGlobe[2].scale(glm::vec3(.05));
    sOrbit.push_back(Spatial());
    sShip.push_back(Spatial());
    sShip[0].scale(glm::vec3(1));
#endif
    initSys(entityManager);
    
    auto &scene = *_scene;
    scene.orbit.entityManager = &entityManager;
    
    //load scene
//    double m = 0.0;
//    double G = 6.673e-11;
//    double gm = m * G;
//    entityManager.addEntity(Family::GRAV, glm::vec3(), glm::vec3(), gm, 10e3, Spatial(), Unit());
//    
//    //add player ship
//    
//    entityManager.addEntity(Family::SHIP, glm::vec3(), glm::vec3(), gm, 10e3, Spatial(), Unit());
}

void GameLogic::linePick(vector<float> &shortestDist, int &closestObj)
{
    auto &scene = *_scene;
    auto &userInput = *_userInput;
    
    //line pick
    //code taken from
    //http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
    double mouseX, mouseY;
    int screenWidth, screenHeight;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    glfwGetWindowSize(window, &screenWidth, &screenHeight);
    mouseY = screenHeight - mouseY; //for some reason, mouseY is flipped from tutorial
    auto mouseX_NDC = ((float)mouseX/(float)screenWidth  - 0.5f) * 2.0f;
    auto mouseY_NDC = ((float)mouseY/(float)screenHeight - 0.5f) * 2.0f;
    vec4 lRayStart_NDC(
                       mouseX_NDC, mouseY_NDC,
                       -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
                       1.0f
                       );
    vec4 lRayEnd_NDC(
                     mouseX_NDC, mouseY_NDC,
                     0.0,
                     1.0f
                     );
    mat4 M = inverse(scene.camera.matrix());
    vec4 lRayStart_world  = M * lRayStart_NDC;
    lRayStart_world /= lRayStart_world.w;
    vec4 lRayEnd_world    = M * lRayEnd_NDC  ;
    lRayEnd_world   /= lRayEnd_world.w  ;
	vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = normalize(lRayDir_world);
    
    shortestDist.clear();
    auto objIdx = 0;
    mouseHover = -1;
    for (const auto &b : sys)   //todo: optimize w/ octree
    {
        auto posNDC = scene.camera.matrix() * world * vec4(b.sn.pos, 1.0);
        posNDC /= posNDC.w;
        auto mouseNDC = vec2(mouseX_NDC * screenWidth, mouseY_NDC * screenHeight);
        auto screenPosNDC = vec2(posNDC.x * screenWidth, posNDC.y * screenHeight);
        auto dist = length(mouseNDC - screenPosNDC);
        shortestDist.push_back(dist);
        
        if (dist < 40) {
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

void GameLogic::missileLogic(float dt)
{
    //for each missile, aim at selected target and fire
#if OLDWAY
    auto start = sysIndexOffset[BodyType::PROJECTILE];
    auto stop = start + numBodyPerType[BodyType::PROJECTILE];
    for (int i = start; i < stop; i++)
    {
        if (selected == i) {
            continue;
        }
        //find vector to target
        vec3 targetVector = normalize(sys[selected].sn.pos - sys[i].sn.pos);
        //add a bit of velocity in that direction?
        float scale = 0.1;
        sys[i].sn.vel += targetVector * scale;
        cout << "update projectile: " << i << endl;
    }
#else
    auto componentManager = entityManager.getComponentManager(Family::SHIP);
    auto sys = componentManager.getSysComponents();
    for (int i = 0; i < componentManager.size(); i++)
    {
        vec3 targetVector = normalize(sys[selected].sn.pos - sys[i].sn.pos);
        //add a bit of velocity in that direction?
        float scale = 0.1;
        sys[i].sn.vel += targetVector * scale;
        cout << "update projectile: " << i << endl;
    }
#endif
    
}

void GameLogic::update(float dt)
{
    auto &scene = *_scene;
    auto &userInput = *_userInput;
    
    //get pos/vel vectors from desired collection
    auto gravCM = entityManager.getComponentManager(Family::GRAV);
    auto shipCM = entityManager.getComponentManager(Family::SHIP);
    
    auto gravPosVector = gravCM.getPosComponents();
    auto shipPosVector = shipCM.getPosComponents();
    auto posVector = appendVector(gravPosVector, shipPosVector);
    
    
    auto gravVelVector = gravCM.getVelComponents();
    auto shipVelVector = shipCM.getVelComponents();
    auto velVector = appendVector(gravVelVector, shipVelVector);
    
    auto gravRadiusVector = gravCM.getRadiusComponents();
    auto shipRadiusVector = shipCM.getRadiusComponents();
    auto radiusVector = appendVector(gravRadiusVector, shipRadiusVector);
    
    auto gravGMVector = gravCM.getGMComponents();
    auto shipGMVector = shipCM.getGMComponents();
    auto GMVector = appendVector(gravGMVector, shipGMVector);
    
    assert(posVector.size() == velVector.size());
    
    //convert pos/vel vectors to sys vector
    vector<body> sys2;
    //TODO: Check if sys2 is suppose to have grav + ships in vector
    sys2.reserve(posVector.size());
    for (int i = 0; i < posVector.size(); i++) {
        sys2.emplace_back(body(state(posVector[i], velVector[i]),
                               GMVector[i],
                               radiusVector[i]
                               )
                          );
    }
    
    //calculate new position/velocity
    float gameDT = dt * timeWarp;
    orbitDelta(gameDT, ks, sys, false);
    
    //convert sys back to pos/vel vectors
    for (int i = 0; i < shipPosVector.size(); i++) {
        shipPosVector[i] = sys2[i+gravPosVector.size()].sn.pos;
        shipVelVector[i] = sys2[i+gravPosVector.size()].sn.vel;
    }
    //check if actual components are updated
    //TODO: check if shipCM is a copy of entityManager.shipCM??
    assert(shipPosVector[0] == shipCM.getPos(0));
    missileLogic(gameDT);

   
#if 1
    double mx, my;
    static double prevMX = 0, prevMY = 0;
    glfwGetCursorPos(window, &mx, &my);
    double _x = mx - prevMX;
    double _y = my - prevMY;
    prevMX = mx;
    prevMY = my;
    double mouseScale = .1;
    
    if (userInput.rmbPressed) {
        scene.camera.rotate(_y*mouseScale, _x*mouseScale);
    } else if (userInput.lmbPressed) {
        auto compManager = entityManager.getComponentManager(Family::SHIP);
        auto spatial = compManager.getSpatial(0);
        spatial.rotate(-_x*mouseScale, _y*mouseScale, 0.0f);
        compManager.setSpatial(0, spatial);
    }
    
    //scroll behavior
    scene.camera.offsetPos(vec3(0,0, -userInput.yScroll));
    userInput.yScroll = 0;
    
    //calculate trajectories
    static int orbitCount = 0;
    if (orbitCount++ % 10 == 0) {
        scene.orbit.update();
    }
    
    //remove elements
    auto findCollisions = [&](ComponentManager &cm1, ComponentManager &cm2, vector<bool> &mark1ForRemoval, vector<bool> &mark2ForRemoval)
    {
        //get pos and radius
        auto &sys1 = cm1.getSysComponents();
        auto &sys2 = cm2.getSysComponents();
        auto &rad1 = cm1.getRadiusComponents();
        auto &rad2 = cm2.getRadiusComponents();
       
        mark1ForRemoval.resize(sys1.size(), false);
        mark2ForRemoval.resize(sys2.size(), false);
        //find all collisions
        for (int i=0; i<sys1.size(); i++) {
            for (int j=0; j<sys2.size(); j++) {
                auto distance = glm::length(sys1[i].sn.pos - sys2[j].sn.pos);
                auto minDistance = rad1[i] + rad2[j];
                if (distance <= minDistance) {
                    mark1ForRemoval[i] = true;
                    mark2ForRemoval[i] = true;
                }
            }
        }
    };
    //for (int i = Family::SHIP; i < Family::MAX_FAMILIES; i++)
    {
        //FIXME this doesn't work at all, need to reformulate algorithm for
        //comparing items of different families and remove them all...'
        
        //find collisions between GRAV and (ships, missiles, projectiles)
        auto &cms = entityManager.getComponentManagers();
        vector<vector<bool> > markForRemoval;
        markForRemoval.resize(Family::MAX_FAMILIES);
        for (int i = Family::SHIP; i < Family::MAX_FAMILIES; i++)
        {
            findCollisions(cms[Family::GRAV], cms[i],
                           markForRemoval[Family::GRAV], markForRemoval[i]);
        }
        
        //find collisions between ships and (missiles, projectiles)
        for (int i = Family::MISSILE; i < Family::MAX_FAMILIES; i++)
        {
            findCollisions(cms[Family::SHIP], cms[i],
                           markForRemoval[Family::SHIP], markForRemoval[i]);
        }
        
        //for each collection, perform removal
        //(optional) compare all missiles against projectiles
        //mark elements that needs to be removed
//        vector<bool> markedForRemoval(sys.size(), false);
//        markForDeletion(sys, markedForRemoval);
        
        //remove all marked elements
        //FIXME: only works because first element never designed to be removed
        for (auto typeIndex = 0; typeIndex < Family::MAX_FAMILIES; typeIndex++)
        {
            if (Family::GRAV == typeIndex) {
                continue;
            }
            //start from last elem and remove
            assert(markForRemoval[typeIndex].size() == cms[typeIndex].size());
            for (auto index = cms[typeIndex].size()-1; index >= 0; index++) {
                if (markForRemoval[typeIndex][index]) {
                    cms[typeIndex].removeEntry(index);
                }
            }
//                sys.erase(it);
//                removeFromSys(it);
                //get a component manager, remove elements from markForRemoval[type]
                
        }
    }
    
    
    world = translate(mat4(), -sys[1].sn.pos);
#endif
}

void GameLogic::processActionList(vector<ActionType> &actionList)
{
    glm::vec3 forwardVector;
    auto compManager = entityManager.getComponentManager(Family::SHIP);
    auto sShip = compManager.getSpatialComponents();
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
            sShip.push_back(Spatial(3.0));    //Spatial constructor inserts body into sys already!
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
            body bullet(state(pos, vel), 1, gm);
//            addSatellite(bullet);
//            InsertToSys(bullet, Family::PROJECTILE);
            auto componentManager = entityManager.getComponentManager(Family::PROJECTILE);
            componentManager.addEntry(pos, vel, gm, 1, Spatial(), Unit());
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