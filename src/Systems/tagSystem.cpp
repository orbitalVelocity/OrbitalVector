#include "tagSystem.h"
#include "componentTypes.h"
#include "includes.h"
#include "log.h"

using namespace entityx;

TagSystem::TagSystem()
{}



void TagSystem::update(EntityManager & entities,
                            EventManager &events,
                            double dt)
{
    Tag::Handle atag;
    Parent::Handle parent;
    MissileLogic::Handle missileLogic;

    for (entityx::Entity entity_linked : entities.entities_with_components(atag))
    {
        for (entityx::Entity entity : entities.entities_with_components(parent))
        {
            if(parent->parent.id() == 0 && parent->parenttag == atag->tag)
            {
                parent->parent = entity_linked.id();
                log(LOG_DEBUG,"Linked reference based on tag.");
            }
        }
        for (entityx::Entity entity : entities.entities_with_components(missileLogic))
        {
            if(!missileLogic->parent.valid() && missileLogic->parenttag == atag->tag)
            {
                missileLogic->parent = entity_linked;
                log(LOG_DEBUG,"Linked reference based on tag.");
            }
            if(!missileLogic->target.valid() && missileLogic->targettag == atag->tag)
            {
                missileLogic->target = entity_linked;
                log(LOG_DEBUG,"Linked reference based on tag.");
            }
        }
        entity_linked.remove<Tag>();
    }
}
