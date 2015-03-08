//
//  ecs.h
//  OrbitalVector
//
//  Created by Si Li on 3/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_ECS_H
#define OrbitalVector_ECS_H
#include "includes.h"
#include "rk547m.h"
#include "entityx/entityx.h"
#include "componentTypes.h"

#define OLDECS true

const int SHIPOFFSET = 1;
const int MISSILEOFFSET = 3; //only works with 1 grav, 2 ships, and 1 missile

glm::vec3 getShipPos(int index);
glm::vec3 getMissilePos(int index);
glm::vec3 getEntityPosition(int index);
int getNumberOfEntities();


class Level : public entityx::EntityX {
private:
    
public:
    explicit Level(std::string filename) {
//        systems.add<DebugSystem>();
//        systems.add<MovementSystem>();
//        systems.add<CollisionSystem>();
        systems.configure();
        
        load(filename);
        
        for (auto e : entity_data()) {
            entityx::Entity entity = entities.create();
//            entity.assign<Position>(e.component<Position>());
//            entity.assign<Velocity>(e.component<Velocity>());
        }
    }

    //loads level of name filename
    void load(std::string filename);
   
    //grabs entity from data loaded in load
    std::vector<entityx::Entity> entity_data();
    
    void update(entityx::TimeDelta dt) {
//        systems.update<DebugSystem>(dt);
//        systems.update<MovementSystem>(dt);
//        systems.update<CollisionSystem>(dt);
    }
    
};

#endif
