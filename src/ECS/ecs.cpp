//
//  ecs.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include <stdio.h>
#include "ecs.h"
#include "tagSystem.h"
#include "userInputSystem.h"
#include "missileSystem.h"
#include "collisionSystem.h"
#include "debugTextSystem.h"
#include "orbitalPhysicsSystem.h"
#include "shipSystem.h"
#include "shipComponent.h"
#include "missileComponent.h"
#include "velocityComponent.h"
#include "playerControlComponent.h"

#define OLDCAMERA true

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
    auto cameraEntity = entities.create();
    auto cameraHandle = cameraEntity.assign<Camera>();

#if OLDCAMERA
    camera.setPosition(glm::vec3(0.0f, 0.0f, 20.0f));
    camera.setFocus(glm::vec3(0, 0, 0.0f));
    camera.setClip(0.01f, 2000.0f);
    camera.setFOV(45.0f);
    camera.setAspectRatio((float)width/(float)height);
#endif
    cameraHandle->copy(camera);
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
    myShip.component<PlayerControl>()->focusOnEntity = myShip;
    assert(myShip.valid());
}

GameSingleton::GameSingleton(std::string filename)
    : renderer(userInput, camera)
{
    
    scene.init();
    legacyUserInput = &userInput;

    systems.add<TagSystem>();
    systems.add<UserInputSystem>(legacyUserInput);
    systems.add<MissileSystem>();
    systems.add<CollisionSystem>();
    
    systems.add<DebugTextSystem>(&textObj);
    systems.add<ShipSystem>();
    systems.add<OrbitalPhysicsSystem>();
    systems.configure();
}

void GameSingleton::update(double dt)
{
    dt *= legacyUserInput->timeWarp;
   
    
#if OLDCAMERA
    auto ch = Camera::getCamera(entities);
    ch->copy(camera);
#endif
    
    //FIXME: no "game lost" condition yet
    assert(myShip.valid());
    
    systems.update<DebugTextSystem>(dt);
    systems.system<UserInputSystem>()->update(entities, events, dt,
                                              legacyUserInput, myShip,
                                              pWindow);
    
#if OLDCAMERA
    camera.copy(*ch.get());
#endif
    //systems.update<TagSystem>(dt); //TODO: Do this only after unserialization.
    systems.update<MissileSystem>(dt);
    systems.update<ShipSystem>(dt);
    systems.update<OrbitalPhysicsSystem>(dt);
    systems.update<CollisionSystem>(dt);
    
    //move the world in the OPPOSITE direction of the focus
    auto playerControl = myShip.component<PlayerControl>();
    auto focus = playerControl->focusOnEntity;
    auto lastFocus = playerControl->lastEntityFocused;
    
    //TODO: this should go in the cameraSystem!
    if (focus.valid()) {
        glm::vec3 desiredPosition;

        //animation
        if (playerControl->switchedFocus and lastFocus.valid()) {
           //lerp between the two positions
            auto focusPosition = focus.component<Position>()->pos;
            auto lastFocusPosition = lastFocus.component<Position>()->pos;
            
            float progress = playerControl->getProgress(dt);
//            std::cout << progress << ",";
            desiredPosition = glm::lerp(lastFocusPosition, focusPosition, progress);
        } else {
            desiredPosition = focus.component<Position>()->pos;
        }
        world = glm::translate(glm::mat4(), -desiredPosition);
    }
    //myShip.component<Position>()->pos);
    
    //TODO: update orbits only when necessary
    {
        renderer.orbit.update(entities);
        renderer.menuCircle.update(entities, events, dt);
    }
}






