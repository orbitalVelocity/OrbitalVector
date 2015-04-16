#ifndef __OrbitalVector__tagSystem__
#define __OrbitalVector__tagSystem__

#include <stdio.h>
#include "entityx/System.h"

/**
 * Fixes broken references between recently unserialized entities.
 */
class TagSystem: public entityx::System<TagSystem>
{
public:
    
    TagSystem();
    
    void update(entityx::EntityManager & entities,
                entityx::EventManager & events,
                double dt);
};
#endif /* defined(__OrbitalVector__tagSystem__) */
