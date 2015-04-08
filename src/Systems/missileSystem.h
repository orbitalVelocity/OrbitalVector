//
//  missileSystem.h
//  OrbitalVector
//
//  Created by Si Li on 3/16/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__missileSystem__
#define __OrbitalVector__missileSystem__

#include <stdio.h>
#include "entityx/System.h"

/**
 * determines how to steer missile towards target (or not!)
 */
class MissileSystem: public entityx::System<MissileSystem>
{
public:
    
    MissileSystem();
    
    void update(entityx::EntityManager & entities,
                entityx::EventManager & events,
                double dt);
};
#endif /* defined(__OrbitalVector__missileSystem__) */
