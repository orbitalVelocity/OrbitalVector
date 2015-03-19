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

GameSingleton myGameSingleton("testMap");

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
    entityx::ComponentHandle<Velocity> velocity = missile.component<Velocity>;
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


std::vector<body> getAllOrbitalObjects()
{
    entityx::ComponentHandle<Position> position;
    entityx::ComponentHandle<Velocity> velocity;
    //TODO: use auto & instead for optimizatioN?
    auto numEntities = 0;
    std::vector<body> newSys;
    for (auto entity : myGameSingleton.entities.entities_with_components(position, velocity))
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
void updateOrbitalPhysics(float dt, vector<vector<state> > &ks, bool adaptive)
{
#if OLDECS
//    orbitPhysicsUpdate(dt, ks, sys, adaptive);
//#else
    auto sys2 = getAllOrbitalObjects();
    
    //check if sys and sys2 match up
    //all sys and sys2 should be exactly the same!
//    assert(sys.size() == sys2.size());
//    for (int i=0; i < sys.size(); i++)
//    {
//        const auto syspos = sys[i].sn.pos;
//        const auto sys2pos = sys2[i].sn.pos;
//        const auto sysvel = sys[i].sn.vel;
//        const auto sys2vel = sys2[i].sn.vel;
//        assert(syspos == sys2pos);
//        assert(sysvel == sys2vel);
//    }

    orbitPhysicsUpdate(dt, ks, sys2, adaptive);
    
    setAllOrbitalObjects(sys2);
    //FIXME: super hacky get rid of this asap: when getting rid of sys in general
    sys = sys2;
    
#endif
}

void setAllOrbitalObjects(std::vector<body> _sys)
{
    entityx::ComponentHandle<Position> position;
    entityx::ComponentHandle<Velocity> velocity;
    entityx::ComponentHandle<GM> gm;
    entityx::ComponentHandle<Radius> radius;
    entityx::ComponentHandle<Parent> parent;
    entityx::ComponentHandle<OrbitalBodyType> orbitalBodyType;
    auto index = 0;
    BodyType types[] = {BodyType::GRAV, BodyType::SHIP, BodyType::MISSILE};
    for (auto type : types)
    {
        for( auto entity : myGameSingleton.entities.entities_with_components(position, velocity, gm, radius, parent, orbitalBodyType))
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


GameSingleton::GameSingleton(std::string filename) {
//        systems.add<DebugSystem>();
//        systems.add<MovementSystem>();
//        systems.add<CollisionSystem>();
    
    load(filename);
    
//        for (auto e : entity_data()) {
//            entityx::Entity entity = entities.create();
//            entity.assign<Position>(e.component<Position>());
//            entity.assign<Velocity>(e.component<Velocity>());
//        }
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
    
    if (type == BodyType::MISSILE and not selectedEntities.empty()) {
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

void GameSingleton::load(std::string filename)
{
    return;
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
               glm::vec3(0,0,-.1),
               {},
               gm,
               32,
               nullEntity.id(),
               BodyType::GRAV
               );
    
    
    m = 1e5;
    gm = m * G;
    
    myShip = entities.create();
    loadEntity(myShip,
               {110,0,0},
               {0,0,2.3},
               {},
               gm,
               10,
               mainGrav.id(),
               BodyType::SHIP
               );
}

void createRandomShip()
{
    myGameSingleton.createRandomShip();
}

void GameSingleton::createRandomShip()
{
    auto newShip = myGameSingleton.entities.create();
    
    double m = 7e12;
    double G = 6.673e-11;
    double gm = m * G;
    
    //FIXME: use std::rand instead
    srand ((unsigned int)time(NULL));
    
    auto r = (rand() / 1000) % 300;
    r += 100;
    float v = std::sqrt(gm/r);
    glm::vec3 rad(r, 0, 0);
    glm::vec3 vel(0, 0, v);
    cout << "new ship: r: " << r << ", v: " << v << endl;
    m = 1e1;
    gm = m * G;

    myGameSingleton.loadEntity(newShip,
                       rad,
                       vel,
                       {},
                       gm,
                       r,
                       mainGrav.id(),
                       BodyType::SHIP);
}

//FIXME: merge back into constructor after getting rid of gamelogic and scene classes
void GameSingleton::init(UserInput *ui, TextRenderer *text)
{
    //FIXME: remove UI asap
    legacyUserInput = ui;
    
    systems.add<LinePickSystem>();
    systems.add<UserInputSystem>(ui, selectedEntities, mouseOverEntities);
    systems.add<MissileSystem>();
    systems.add<CollisionSystem>();
    systems.add<DebugTextSystem>(text);
    systems.configure();
    
}

void GameSingleton::update(double dt)
{
    systems.system<LinePickSystem>()->update(entities, events, dt, pWindow, pCamera);
    systems.system<UserInputSystem>()->update(entities, events, dt, legacyUserInput, myShip);
    systems.update<MissileSystem>(dt);
//        systems.update<MovementSystem>(dt);
    systems.update<CollisionSystem>(dt);
    systems.update<DebugTextSystem>(dt); //this does nothing right now
//        systems.update<CollisionSystem>(dt);
    
}






