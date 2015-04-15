#ifndef __OrbitalVector__uuidSystem__
#define __OrbitalVector__uuidSystem__

#include <stdio.h>
#include "entityx/System.h"

/**
 * Fixes broken references between recently unserialized entities.
 */
class UUIDSystem: public entityx::System<UUIDSystem>
{
public:
    
    UUIDSystem();
    
    void update(entityx::EntityManager & entities,
                entityx::EventManager & events,
                double dt);
};
#endif /* defined(__OrbitalVector__uuidSystem__) */
