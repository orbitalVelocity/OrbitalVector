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

    systems.add<TagSystem>();
    systems.add<UserInputSystem>(legacyUserInput);
    systems.add<MissileSystem>();
    systems.add<CollisionSystem>();
    systems.add<DebugTextSystem>(&textObj);
    systems.add<ShipSystem>();
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
    dt *= legacyUserInput->timeWarp;
    assert(myShip.valid());
    systems.system<UserInputSystem>()->update(entities, events, dt,
                                              legacyUserInput, myShip,
                                             pWindow, camera);
    //systems.update<TagSystem>(dt); //TODO: Do this only after unserialization.
    systems.update<MissileSystem>(dt);
    systems.update<ShipSystem>(dt);
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
//            textObj.guiText.push_back({getVec2(vp, position->pos),
//                15.0f, missile->debugName});
            events.emit<GUITextEvent>(getVec2(vp, position->pos),
                                      15.0f, missile->debugName);
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






