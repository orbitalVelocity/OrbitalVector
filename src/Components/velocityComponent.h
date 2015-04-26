//
//  velocityComponent.h
//  OrbitalVector
//
//  Created by Si Li on 4/26/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_velocityComponent_h
#define OrbitalVector_velocityComponent_h
#include "entityx/Entity.h"
#include "componentTypes.h"

COMPONENT(Velocity)
{
    Velocity() {}
    Velocity(glm::vec3 v) : vel(v) {}
    
    /**
     * returns acceleration and clears it as well
     * FIXME: consider better semantics in function name
     */
    glm::vec3 getAccel()
    {
        auto temp = accel;
        accel = glm::vec3(0);
        return temp;
    }
    void setAccel(glm::vec3 a)
    {
        accel = a;
    }
    
    glm::vec3 vel;
private:
    glm::vec3 accel;
};
#endif
