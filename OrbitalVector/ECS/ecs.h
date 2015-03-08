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
using namespace glm;
using namespace entityx;

const int SHIPOFFSET = 1;

vec3 getShipPos(int index);



class Level : public EntityX {
private:
    
public:
    explicit Level(string filename) {
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
    void load(string filename);
   
    //grabs entity from data loaded in load
    vector<Entity> entity_data();
    
    void update(TimeDelta dt) {
//        systems.update<DebugSystem>(dt);
//        systems.update<MovementSystem>(dt);
//        systems.update<CollisionSystem>(dt);
    }
    
};

#endif
