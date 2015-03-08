//
//  ecs.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include <stdio.h>
#include "ecs.h"


glm::vec3 getShipPos(int index)
{
#if OLDECS
    return sys[index+SHIPOFFSET].sn.pos;
#else
    //convert index to ship index or something
    //keep another vector to store just ship?
    int shipIndex = convertindextoshipindex(index);
    return getEntityPosition(shipIndex);
#endif
}

glm::vec3 getMissilePos(int index)
{
#if OLDECS
    return sys[index+MISSILEOFFSET].sn.pos;
#else
    //convert index to missile index or something
    //keep another vector to store just missile?
    int missileIndex = convertindextomissileindex(index);
    return getEntityPosition(missileIndex);
#endif
}

glm::vec3 getEntityPosition(int index)
{
#if OLDECS
    return sys[index].sn.pos;
#else
    return ecs.get<Position>(index);
#endif
}

int getNumberOfEntities()
{
#if OLDECS
    return sys.size();
#else
    return ecs.size();
#endif
}