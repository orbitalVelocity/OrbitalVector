#include "uuidSystem.h"
#include "componentTypes.h"
#include "includes.h"
#include "log.h"

using namespace entityx;

UUIDSystem::UUIDSystem()
{}



void UUIDSystem::update(EntityManager & entities,
                            EventManager &events,
                            double dt)
{
    UUID::Handle atag;
    Parent::Handle parent;
    MissileLogic::Handle missileLogic;

    for (entityx::Entity entity_linked : entities.entities_with_components(atag))
    {
        for (entityx::Entity entity : entities.entities_with_components(parent))
        {
            if(parent->parent.id() == 0 && parent->parentuuid == atag->tag)
            {
                parent->parent = entity_linked.id();
                log(LOG_DEBUG,"Linked reference based on UUID.");
            }
        }
        for (entityx::Entity entity : entities.entities_with_components(missileLogic))
        {
            if(!missileLogic->parent.valid() && missileLogic->parentuuid == atag->tag)
            {
                missileLogic->parent = entity_linked;
                log(LOG_DEBUG,"Linked reference based on UUID.");
            }
            if(!missileLogic->target.valid() && missileLogic->targetuuid == atag->tag)
            {
                missileLogic->target = entity_linked;
                log(LOG_DEBUG,"Linked reference based on UUID.");
            }
        }
    }
}
