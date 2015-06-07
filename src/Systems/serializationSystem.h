//
//  serializationSystem.h
//  OrbitalVector
//
//  Created by Si Li on 5/31/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__serializationSystem__
#define __OrbitalVector__serializationSystem__

#include <stdio.h>
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>

#include "entityx/System.h"
#include "events.h"

class SerializationSystem: public entityx::System<SerializationSystem>,
public entityx::Receiver<SerializationSystem>
{
public:
    SerializationSystem() {}
    
    void configure(entityx::EventManager& events);
    
    void receive(const SerializeEvent &e);
    
    void update(entityx::EntityManager &entities,
                entityx::EventManager &events,
                double dt);
private:
    bool serializeFlag = false;
};
#endif /* defined(__OrbitalVector__serializationSystem__) */
