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

#define SERIALIZE(T, C)                         \
    if (e.has_component<C>()) {                 \
        auto pc = e.component<C>();             \
        ar( make_nvp(T, *(pc.get()) ) );        \
    }

namespace cereal
{
    template<class Archive>
    void save( Archive &ar, entityx::Entity const & entity )
    {
//        uint64_t id = entity.id().id();
//        ar( make_nvp("entityID", id) );
        assert(entity.has_component<Tag>());
        auto e = const_cast<entityx::Entity &>(entity);
        auto tag = e.component<Tag>();
        assert(entity.valid());
        ar( cereal::make_nvp("uuid", (uint)(tag.get()->tag)) );
        
        //serialize over all components of each entity
        SERIALIZE("playerControl", PlayerControl);
//        SERIALIZE("camera", CameraComponent);

    }
    
    template<class Archive>
    void load( Archive &ar, entityx::Entity & entity )
    {
        //        ar( entity.id() );//cereal::make_nvp("entityID", entity.id()) );
    }
}

void SerializationSystem::update(entityx::EntityManager &entities, entityx::EventManager &events, double dt)
{
    static bool run = true;
    if (not run) {
        return;
    }
    run = false;
    
    for (auto entity : entities.entities_for_debugging())
    {
        entity.assign<Tag>();
    }
    
    std::cout << "printing json dump!\n";
    cereal::JSONOutputArchive archive( std::cout );
    //iterate over all entities
    for (auto entity : entities.entities_for_debugging())
    {
        archive( cereal::make_nvp("entity", entity) );
    }
}