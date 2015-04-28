//
//  componentTypes.cpp
//  OrbitalVector
//
//  Created by Si Li on 4/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include <stdio.h>
#include "componentTypes.h"
#include "camera.h"

tag_t Tag::previous = 0;


int OrbitPath::instanceCount(0);


glm::mat4 world;

glm::mat4 getViewProjection(entityx::EntityManager &entities)
{
    auto ch = Camera::getCamera(entities);
    auto vp = ch->matrix() * world;
    return vp;
}