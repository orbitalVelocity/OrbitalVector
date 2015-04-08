//
//  cameraSystem.h
//  OrbitalVector
//
//  Created by Si Li on 3/19/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__cameraSystem__
#define __OrbitalVector__cameraSystem__

#include <stdio.h>
#include "includes.h"
#include "entityx/System.h"

class CameraSystem : public entityx::System<CameraSystem>
//public entityx::Receiver<OrbitalChangeEvent>
{
public:
    
public:
    CameraSystem() {}
    void update(entityx::EntityManager &entities,
                entityx::EventManager &events,
                double dt);
};
#endif /* defined(__OrbitalVector__cameraSystem__) */
