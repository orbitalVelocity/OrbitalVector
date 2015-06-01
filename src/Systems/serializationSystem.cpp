//
//  serializationSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 5/31/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "serializationSystem.h"
#include "cameraComponent.h"
#include "missileComponent.h"
#include "shipComponent.h"
#include "playerControlComponent.h"
#include "shadowComponent.h"
#include "velocityComponent.h"
#include "componentTypes.h"

#define SERIALIZE(X, T, C) if(entity.has_component<C>()) { \
    auto chandler = entity.component<C>()->serialize(X); \
    archive( cereal::make_nvp("T", *(chandler.get()) ) };

void SerializationSystem::update(entityx::EntityManager &entities, entityx::EventManager &events, double dt)
{
    cereal::JSONOutputArchive archive( std::cout );
    //iterate over all entities
    for (auto entity : entities.entities_for_debugging())
    {
    //serialize over all components of each entity
        //need a list of all components!
        if (entity.has_component<PlayerControl>()) {
            auto pc = entity.component<PlayerControl>();
            archive( cereal::make_nvp("playerControl", *(pc.get()) ) );
        }
        //repeat for all components???
    }
}