//
//  entityxHelpers.h
//  OrbitalVector
//
//  Created by Si Li on 4/27/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__entityxHelpers__
#define __OrbitalVector__entityxHelpers__

#include <stdio.h>
#include "entityx/Entity.h"


void printOE(std::string name, entityx::Entity entity, entityx::EntityManager &entities, entityx::EventManager &events);

void printOE(std::string name, float element, entityx::Entity entity, entityx::EntityManager &entities, entityx::EventManager &events);

#endif /* defined(__OrbitalVector__entityxHelpers__) */
