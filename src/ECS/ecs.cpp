//
//  ecs.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include <stdio.h>
#include "ecs.h"
#include "linePickSystem.h"
#include "userInputSystem.h"
#include "missileSystem.h"
#include "collisionSystem.h"
#include "debugTextSystem.h"
#include "orbitalPhysicsSystem.h"

//GameSingleton myGameSingleton("testMap");

glm::vec3 getShipPos(int index)
{
#if OLDECS
    int offset = sysIndexOffset[BodyType::SHIP];
    return sys[index+offset].sn.pos;
#else
    //convert index to ship index or something
    //keep another vector to store just ship?
    int shipIndex = convertindextoshipindex(index);
    return getEntityPosition(shipIndex);
#endif
}

glm::vec3 getMissilePos(int index)
{
#if OLDECS
    int offset = sysIndexOffset[BodyType::MISSILE];
    return sys[index+offset].sn.pos;
#else
    //convert index to missile index or something
    //keep another vector to store just missile?
    int missileIndex = convertindextomissileindex(index);
    return getEntityPosition(missileIndex);
#endif
}

glm::vec3* getMissileVelocityPointer(int index)
{
#if OLDECS
    int offset = sysIndexOffset[BodyType::MISSILE];
    return &(sys[index+offset].sn.vel);
#else
    Velocity::Handle velocity = missile.component<Velocity>;
    myGameSingleton.entities.entities_with_components(position, velocity)
    return velocity->vel;
#endif
}

glm::vec3 getEntityPosition(int index)
{
#if OLDECS
    return sys[index].sn.pos;
#else
    return ecs.get<Position>(index);
#endif
}


int getNumberOfShips()
{
    return numBodyPerType[BodyType::SHIP];
}
int getNumberOfMissiles()
{
    return numBodyPerType[BodyType::MISSILE];
}

int getNumberOfEntities()
{
#if OLDECS
    return sys.size();
#else
    return ecs.size();
#endif
}


std::vector<body> getAllOrbitalObjects(entityx::EntityManager &entities)
{
    Position::Handle position;
    Velocity::Handle velocity;
    //TODO: use auto & instead for optimizatioN?
    auto numEntities = 0;
    std::vector<body> newSys;
    for (auto entity : entities.entities_with_components(position, velocity))
    {
        numEntities++;
        auto pos = entity.component<Position>();
        auto vel = entity.component<Velocity>();
        auto gm = entity.component<GM>();
        auto r = entity.component<Radius>();
        auto parent = entity.component<Parent>();
        auto bt = entity.component<OrbitalBodyType>();
        newSys.push_back(body(state(pos->pos, vel->vel),
                              gm->gm,
                              r->radius,
                              nullptr,//FIXME: should be parent->parent,
                              bt->orbitalBodyType));
    }
    
//    assert(sys.size() == numEntities);
    return newSys;
}

/**
 *  Take all entities in ECS and convert to a vector<body> a la sys
 *  Run orbitPhysicsUpdate as usual and convert it back
 *  backwards compatibility is acheived by updating sys as well
 *  Caveat: adding/removing units must be done via ECS
 */
void updateOrbitalPhysics(entityx::EntityManager &entities, float dt, vector<vector<state> > &ks, bool adaptive)
{
#if OLDECS
    auto sys2 = getAllOrbitalObjects(entities);

    orbitPhysicsUpdate(dt, ks, sys2, adaptive);
    
    setAllOrbitalObjects(entities, sys2);
    sys = sys2;
    
#endif
}

void setAllOrbitalObjects(entityx::EntityManager &entities, std::vector<body> _sys)
{
    Position::Handle position;
    Velocity::Handle velocity;
    GM::Handle gm;
    Radius::Handle radius;
    Parent::Handle parent;
    OrbitalBodyType::Handle orbitalBodyType;
    auto index = 0;
    BodyType types[] = {BodyType::GRAV, BodyType::SHIP, BodyType::MISSILE};
    for (auto type : types)
    {
        for( auto entity : entities.entities_with_components(position, velocity, gm, radius, parent, orbitalBodyType))
        {
            if (orbitalBodyType->orbitalBodyType == type) {
                position->pos = _sys[index].sn.pos;
                velocity->vel = _sys[index].sn.vel;
                index++;
            }
        }
    }
}


glm::vec3 getMyShipPos()
{
#if OLDECS
    return sys[1].sn.pos;
#else
    return myshipentity.component<Position>();
#endif
}

glm::vec3 getMyShipVel()
{
#if OLDECS
    return sys[1].sn.vel;
#else
    return myshipentity.component<Velocity>();
#endif
}




void GameSingleton::loadEntity(entityx::Entity entity,
                    glm::vec3 pos,
                    glm::vec3 vel,
                    glm::mat4 orientation,
                    double _gm,
                    float r,
                    entityx::Entity::Id _parent,
                    BodyType bt
                    )
{
    entity.assign<Position>(pos);
    entity.assign<Velocity>(vel);
    entity.assign<GM>(_gm);
    entity.assign<Parent>(_parent);
    entity.assign<OrbitalBodyType>(bt);
    entity.assign<Orientation>(orientation);
    entity.assign<Radius>(r);
    
    body body(state(pos, vel),
              _gm,
              r,
              nullptr,
              bt);
    InsertToSys(body, bt);
    if (bt == BodyType::MISSILE or bt == BodyType::SHIP)
    {
        entity.assign<OrbitPath>();
    }
    if (bt == BodyType::MISSILE)
    {
        entity.assign<Missile>();
    }
    if (bt == BodyType::SHIP)
    {
        entity.assign<Ship>();
    }
}

void GameSingleton::createEntity(glm::vec3 pos,
                  glm::vec3 vel,
                  glm::mat4 orientation,
                  double gm,
                  float r,
                  int type)
{
    auto newShip = entities.create();
    loadEntity(newShip,
               pos,
               vel,
               orientation,
               gm,
               r,
               mainGrav.id(),
               (BodyType)type);
    
    auto &selectedEntities = myShip.component<PlayerControl>()->selectedEntities;
    if (type == BodyType::MISSILE and not selectedEntities.empty()) {
        assert(myShip.valid());
        newShip.assign<MissileLogic>(myShip, selectedEntities.front());
    }

}

void GameSingleton::createShip(
                    glm::vec3 pos,
                    glm::vec3 vel,
                    glm::mat4 orientation,
                    double gm,
                    float r)
{
    auto newShip = entities.create();
    loadEntity(newShip,
               pos,
               vel,
               orientation,
               gm,
               r,
               mainGrav.id(),
               BodyType::SHIP);
}


void GameSingleton::initCamera(int width, int height) {
    camera.setPosition(glm::vec3(0.0f, 0.0f, 20.0f));
    camera.setFocus(glm::vec3(0, 0, 0.0f));
    camera.setClip(0.01f, 2000.0f);
    camera.setFOV(45.0f);
    camera.setAspectRatio((float)width/(float)height);
}

void GameSingleton::load(std::string, int width, int height )
{
    initCamera(width, height);
    
    //FIXME: this is because InsertToSys creates new entities directly
    double m = 7e12;
    double G = 6.673e-11;
    double gm = m * G;
    
    //load json and create entities
    auto nullEntity = entities.create();
    nullEntity.invalidate();
    mainGrav = entities.create();
    loadEntity(mainGrav,
               {},
               glm::vec3(0,0,0),
               {},
               gm,
               30,
               nullEntity.id(),
               BodyType::GRAV
               );
    
    
    m = 1e5;
    gm = m * G;
    
    myShip = entities.create();
    loadEntity(myShip,
               glm::vec3(110,0.1,0),
               glm::vec3(0,2.3,-0.1),
               glm::rotate(glm::mat4(), (float)M_PI/2, glm::vec3(1, 0, 0)),
               gm,
               1,
               mainGrav.id(),
               BodyType::SHIP
               );
    myShip.assign<PlayerControl>();
    assert(myShip.valid());
}

GameSingleton::GameSingleton(std::string filename)
    : renderer(userInput, camera)
{
    
    scene.init();
    legacyUserInput = &userInput;

    systems.add<UserInputSystem>(legacyUserInput);
    systems.add<MissileSystem>();
    systems.add<CollisionSystem>();
    systems.add<DebugTextSystem>(&textObj);
    systems.add<OrbitalPhysicsSystem>();
    systems.configure();
}

//TODO: all of this includes and template function just to print OE! must find a more elegant solution
#include "vectord.h"
#include "integration.h"
#include "twobody.h"

#include "orbitalelements.h"
#include "oeconvert.h"

#include "componentTypes.h" //FIXME: hack, need to refactor

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out << std::setprecision(n) << a_value;
    return out.str();
}

void GameSingleton::update(double dt)
{
    //requires timeWarp declared in this class
    //requires tighter integration w/ userInput, it needs access to timeWarp variable
//    dt *= timeWarpFactor;
    assert(myShip.valid());
    systems.system<UserInputSystem>()->update(entities, events, dt,
                                              legacyUserInput, myShip,
                                             pWindow, camera);
    systems.update<MissileSystem>(dt);
    systems.update<OrbitalPhysicsSystem>(dt);
    systems.update<CollisionSystem>(dt);
    systems.update<DebugTextSystem>(dt); //this does nothing right now
    
    //move the world in the OPPOSITE direction of the focus
    world = glm::translate(glm::mat4(), -myShip.component<Position>()->pos);
    
    auto totalOffset = glm::vec2(0, 0.02);
    auto offset = glm::vec2(0, 0.02);
    auto vp = camera.matrix() * world;
    auto printOE = [&](string name, float element, glm::vec3 pos)
    {
        auto orbitParamString = name + to_string_with_precision(element);
        textObj.guiText.push_back({getVec2(vp, pos)+totalOffset, 15.0f, orbitParamString});
        totalOffset += offset;
    };
    auto UITextSetup = [&](){
        Ship::Handle ship;
        Missile::Handle missile;
        Position::Handle position;
        Velocity::Handle velocity;
        OrbitPath::Handle orbit;
        
        
        textObj.guiText.clear();
        for (auto entity : entities.entities_with_components(ship, position, velocity, orbit))
        {
            textObj.guiText.push_back({getVec2(vp, position->pos),
                15.0f, ship->debugName})
            ;
            
            //print out orbital elements
            auto parentEntityID = entity.component<Parent>()->parent;
            auto parentEntity = entities.get(parentEntityID);
            auto parentPosition = parentEntity.component<Position>();
            auto parentGM = parentEntity.component<GM>();
            auto posVel = toPosVelVector(position->pos, velocity->vel);
            auto oe = rv2oe(parentGM->gm, posVel);
            
            printOE("sma: ", oe.sma, position->pos);
            printOE("ecc: ", oe.ecc, position->pos);
            printOE("inc: ", oe.inc, position->pos);
            printOE("aop: ", oe.aop, position->pos);
            printOE("lan: ", oe.lan, position->pos);
            printOE("tra: ", oe.tra, position->pos);
            totalOffset = offset;
            
        }
        for (auto entity : entities.entities_with_components(missile, position, orbit))
        {
            auto vp = camera.matrix() * world;
            textObj.guiText.push_back({getVec2(vp, position->pos),
                15.0f, missile->debugName});
        }
    };
    UITextSetup();
    
    //calculate trajectories every 30 frames
//    static int orbitCount = 1;
//    if (orbitCount++ % 1 == 0)
    {
        renderer.orbit.update(entities);
        renderer.menuCircle.update(entities, events, dt);
    }
}






