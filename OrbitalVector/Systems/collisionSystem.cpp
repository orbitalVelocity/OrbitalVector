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

//void CollisionSystem::configure(EventManager& eventManager)
//{
//    eventManager.subscribe<CollisionEvent>(*this);
//}

//void CollisionSystem::receive(const CollisionEvent &e)
//{
//    //remove all components in even
//    //TODO: need to trigger an animation event, explode, update UI, signal to player entity destroyed
//    //and THEN destroy the entity
//    //basically, replace this function with a DeathSystem
//    auto entity1 = e.entity1;
//    auto entity2 = e.entity2;
//    
//}


void CollisionSystem::update(EntityManager & entities,
                           EventManager &events,
                           double dt)
{
    currentTime += dt;
    
    //FIXME: need a continuous collision detection method
    ComponentHandle<Position> left_position, right_position;
    ComponentHandle<Radius> left_radius, right_radius;
    
    //FIXME: add a collidable component? are there anything that's not collidable? particles?
    for (Entity left_entity : entities.entities_with_components(left_position, left_radius)) {
    
        for (Entity right_entity : entities.entities_with_components(right_position, right_radius)) {
            if (right_entity.id() == left_entity.id()) {
                continue;
            }
            //FIXME: handle self/target collision relationships
            //if entity is a planet or some other non destructable?
            //priority queue: what happens when a puff of smoke collides with a ship?
                //puff goes away, ship stays!
                //events.emit<puffCollision>(puffEntity, shipEntity);
                //events.emit<shipCollision>(shipEntity, puffEntity);
            //????
            
            //find distance between two position
            auto distance = glm::length(left_position->pos -  right_position->pos);
            auto minimumDistance = left_radius->radius + right_radius->radius;
            
            //emit collision event if this is closer than the sum of their radii
            if (distance < minimumDistance)
            {
                std::stringstream text;
                text << "collision: distance: " << distance << " r1: " << left_radius->radius
                    << " r2: " << right_radius->radius << std::endl;
                events.emit<DebugEvent>(text.str());
                
                
                addCollision(left_entity);
                addCollision(right_entity);
            }
        }
    }
 
    processCollisions();
}

//FIXME: should have a destructability component
void CollisionSystem::addCollision(Entity entity){
    if (entity.component<OrbitalBodyType>() not_eq BodyType::GRAV) {
        
        //set collision to occur in the past so it's processed immediately
        MyPair temp(currentTime, entity);
        collided.push(temp);//{currentTime, entity});
    }
}
void CollisionSystem::processCollisions()
{
    //process all past and present collisions
    while (not collided.empty() && collided.top().time < currentTime)
    {
        auto myPair = collided.top();
        myPair.entity.destroy();
        //clear this pair from collided map
        collided.pop();
    }
}