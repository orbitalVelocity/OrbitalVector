//
//  orbitalPhysicsSystem.h
//  OrbitalVector
//
//  Created by Si Li on 3/18/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__orbitalPhysicsSystem__
#define __OrbitalVector__orbitalPhysicsSystem__

#include <stdio.h>
#include "entityx/System.h"
#include "events.h"
#include "cameraComponent.h"

class OrbitalPhysicsSystem : public entityx::System<OrbitalPhysicsSystem>
//public entityx::Receiver<OrbitalChangeEvent>
{
public:
    OrbitalPhysicsSystem() {}
    
    void  update(entityx::EntityManager & entities,
                entityx::EventManager & events,
                double dt) override;
};
#endif /* defined(__OrbitalVector__orbitalPhysicsSystem__) */
