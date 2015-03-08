//
//  ecs.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include <stdio.h>
#include "ecs.h"


vec3 getShipPos(int index)
{
#if OLDECS
    return sys[index+SHIPOFFSET].sn.pos;
#else
    return entityx::entity.get<POSITION>(index);
#endif
}