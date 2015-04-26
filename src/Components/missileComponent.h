//
//  missileComponent.h
//  OrbitalVector
//
//  Created by Si Li on 4/26/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__missileComponent__
#define __OrbitalVector__missileComponent__

#include <stdio.h>
#include <vector>
#include "entityx/Entity.h"
#include "componentTypes.h"

COMPONENT(MissileLogic)
{
    MissileLogic() {}
    MissileLogic(entityx::Entity p, entityx::Entity t)
    : parent(p), target(t) {}
    
    tag_t targettag;
    entityx::Entity target;
    tag_t parenttag;
    entityx::Entity parent;
    bool done = false;
    
};
COMPONENT(Missile)
{
    Missile()
    {
        debugName = "missile" + std::to_string(instanceCount);
        instanceCount++;
    }
    int meshID;
    std::string debugName;
    static int instanceCount;
};

#endif /* defined(__OrbitalVector__missileComponent__) */
