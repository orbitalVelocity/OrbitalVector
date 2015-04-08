//
//  collisionSystem.h
//  OrbitalVector
//
//  Created by Si Li on 3/16/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__collisionSystem__
#define __OrbitalVector__collisionSystem__

#include <stdio.h>
#include <queue>
#include <tuple>
#include "entityx/System.h"
#include "events.h"

class MyPair {
public:
    double time;
    entityx::Entity entity;
    
    MyPair(double t, entityx::Entity e) : time(t), entity(e) {}
    bool operator>(const MyPair &rhs) const
    {
        return time > rhs.time;
    }
    
};


class CollisionSystem: public entityx::System<CollisionSystem>
//                        public entityx::Receiver<CollisionEvent>
{
public:
    
    CollisionSystem();
//    void configure(entityx::EventManager& eventManager);
    
//    void receive(const CollisionEvent &e);
    
    /**
     * detects collisoins
     */
    void update(entityx::EntityManager & entities,
                entityx::EventManager & events,
                double dt);
    
    void processCollisions();
    
    void addCollision(entityx::Entity entity);
    
private:
    double currentTime = 0.0;
//    std::map<double, entityx::Entity> collided;
    
    std::priority_queue<MyPair,
                        std::deque<MyPair>,
                        std::greater<MyPair> >
        collided;
    std::set<entityx::Entity::Id> collidedSet;
};
#endif /* defined(__OrbitalVector__collisionSystem__) */
