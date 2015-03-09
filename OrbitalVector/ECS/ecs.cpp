//
//  ecs.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include <stdio.h>
#include "ecs.h"

Level myLevel("testMap");

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
    int offset = sysIndexOffset[BodyType::PROJECTILE];
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
    return &(sys[index].sn.vel);
#else
    ecs.
    entityx::ComponentHandle<Velocity> velocity = missile.component<Velocity>;
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
    return numBodyPerType[BodyType::PROJECTILE];
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
    for (auto entity : myLevel.entities.entities_with_components(position, velocity))
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
    
    assert(sys.size() == numEntities);
    return newSys;
}

void updateOrbitalPhysics(float dt, vector<vector<state> > &ks, bool adaptive)
{
#if OLDECS
    
    auto sys2 = getAllOrbitalObjects();
    
//    all sys and sys2 should be exactly the same!
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

    
//    orbitPhysicsUpdate(dt, ks, sys, adaptive);
    orbitPhysicsUpdate(dt, ks, sys2, adaptive);
    setAllOrbitalObjects(sys2);
    sys = sys2;
    
    //check if sys and sys2 match up
#else
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
    BodyType types[] = {BodyType::GRAV, BodyType::SHIP, BodyType::PROJECTILE};
    for (auto type : types)
    {
        for( auto entity : myLevel.entities.entities_with_components(position, velocity, gm, radius, parent, orbitalBodyType))
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


void initECS()
{
    
}





