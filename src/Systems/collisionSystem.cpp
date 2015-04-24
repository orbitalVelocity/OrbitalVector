//
//  collisionSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/16/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "collisionSystem.h"
#include "componentTypes.h"
#include "log.h"

using namespace entityx;

CollisionSystem::CollisionSystem()
{

}

void CollisionSystem::update(EntityManager & entities,
                           EventManager &events,
                           double dt)
{
    currentTime += dt;
    
    //FIXME: need a continuous collision detection method
    Position::Handle left_position, right_position;
    Radius::Handle left_radius, right_radius;
    Velocity::Handle left_velocity, right_velocity;
    
    for (Entity left_entity : entities.entities_with_components(left_position, left_radius, left_velocity)) {
    
        for (Entity right_entity : entities.entities_with_components(right_position, right_radius, right_velocity)) {
            if (right_entity.id() == left_entity.id()) {
                continue;
            }
            
            //find the minimum distance between the two entities over the last dt
            auto dr = left_position->pos - right_position->pos;
            auto dv = left_velocity->vel - right_velocity->vel;
            auto time = glm::length(dv) != 0.0 ? -dot(dr,dv)/dot(dv,dv) : 0.0;
            auto distance = -dt < time && time < 0.0 ? glm::length(dr+time*dv) : fmin(glm::length(dr),glm::length(dr-dv*dt));
            auto minimumDistance = left_radius->radius + right_radius->radius;
            
            //emit collision event if this is closer than the sum of their radii
            if (distance < minimumDistance)
            {
                std::stringstream text;
                text << "collision: distance: " << distance << " r1: "
                    << left_entity.component<OrbitalBodyType>()->orbitalBodyType << " : " << left_radius->radius
                    << " r2: " << right_entity.component<OrbitalBodyType>()->orbitalBodyType << " : " << right_radius->radius << std::endl;
//                events.emit<DebugEvent>(text.str());
                std::cout << text.str() << std::endl;
                
                if (-dt < time && time < 0.0) log(LOG_DEBUG,"A collision was detected between frames.");
                else log(LOG_DEBUG,"A collision was detected at the end of a frame.");
                addCollision(left_entity);
                addCollision(right_entity);
            }
        }
    }
 
    processCollisions(entities);
}

//FIXME: handle self/target collision relationships
//if entity is a planet or some other non destructable?
//priority queue: what happens when a puff of smoke collides with a ship?
//puff goes away, ship stays!
//events.emit<puffCollision>(puffEntity, shipEntity);
//events.emit<shipCollision>(shipEntity, puffEntity);
//????
//FIXME: should have a destructability component
void CollisionSystem::addCollision(Entity entity){
    auto hasNotBeenAdded = [&](Entity::Id id)
    {
        auto ret = collidedSet.insert(id);
        return ret.second;
    };
    if (entity.component<OrbitalBodyType>()->orbitalBodyType not_eq BodyType::GRAV
        and hasNotBeenAdded(entity.id())
        and not entity.has_component<PlayerControl>()) {
        
        //set collision to occur in the past so it's processed immediately
        MyPair temp(currentTime, entity);
        collided.push(temp);//{currentTime, entity});
    }
}
void CollisionSystem::processCollisions(EntityManager & entities)
{
    //process all past and present collisions
    while (not collided.empty() && collided.top().time <= currentTime)
    {
        auto myPair = collided.top();
        assert(myPair.entity.valid());
        //check if a menu is attached to entity
        GUICircleMenu::Handle menuCircle;
        for (auto entity : entities.entities_with_components(menuCircle))
        {
            if (menuCircle->target.id() == myPair.entity.id()) {
                entity.destroy();
            }
        }
        myPair.entity.destroy();
        //clear this pair from collided map
        collided.pop();
    }
}
