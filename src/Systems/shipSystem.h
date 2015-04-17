//
//  shipSystem.h
//  OrbitalVector
//
//  Created by Si Li on 4/16/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__shipSystem__
#define __OrbitalVector__shipSystem__

#include <stdio.h>
#include "entityx/System.h"
#include "events.h"

class ShipSystem : public entityx::System<ShipSystem>,
public entityx::Receiver<ShipSystem>
{
public:
    ShipSystem() {}
    
    void update(entityx::EntityManager &entities,
                entityx::EventManager &events,
                double dt);
};
#endif /* defined(__OrbitalVector__shipSystem__) */
