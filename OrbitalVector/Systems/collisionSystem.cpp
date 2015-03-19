//
//  collisionSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/16/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "collisionSystem.h"
#include "componentTypes.h"

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
    ComponentHandle<Position> left_position, right_position;
    ComponentHandle<Radius> left_radius, right_radius;
    
    for (Entity left_entity : entities.entities_with_components(left_position, left_radius)) {
    
        for (Entity right_entity : entities.entities_with_components(right_position, right_radius)) {
            if (right_entity.id() == left_entity.id()) {
                continue;
            }
            
            //find distance between two position
            auto distance = glm::length(left_position->pos -  right_position->pos);
            auto minimumDistance = left_radius->radius + right_radius->radius;
            
            //emit collision event if this is closer than the sum of their radii
            if (distance < minimumDistance)
            {
                std::stringstream text;
                text << "collision: distance: " << distance << " r1: "
                    << left_entity.component<OrbitalBodyType>()->orbitalBodyType << " : " << left_radius->radius
                    << " r2: " << right_entity.component<OrbitalBodyType>()->orbitalBodyType << " : " << right_radius->radius << std::endl;
                events.emit<DebugEvent>(text.str());
                
                addCollision(left_entity);
                addCollision(right_entity);
            }
        }
    }
 
    processCollisions();
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
    if (entity.component<OrbitalBodyType>()->orbitalBodyType not_eq BodyType::GRAV) {
        
        //set collision to occur in the past so it's processed immediately
        MyPair temp(currentTime, entity);
        collided.push(temp);//{currentTime, entity});
    }
}
void CollisionSystem::processCollisions()
{
    //process all past and present collisions
    while (not collided.empty() && collided.top().time <= currentTime)
    {
        auto myPair = collided.top();
        myPair.entity.destroy();
        //clear this pair from collided map
        collided.pop();
    }
}