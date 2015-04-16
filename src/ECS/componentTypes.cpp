//
//  componentTypes.cpp
//  OrbitalVector
//
//  Created by Si Li on 4/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include <stdio.h>
#include "componentTypes.h"

tag_t Tag::previous = 0;

int Ship::instanceCount(0);
int Missile::instanceCount(0);


glm::mat4 world;
