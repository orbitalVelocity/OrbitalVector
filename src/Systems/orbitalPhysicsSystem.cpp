//
//  orbitalPhysicsSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/18/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "orbitalPhysicsSystem.h"
#include "componentTypes.h"

#include "entityx/Entity.h"

#include "vectord.h"
#include "integration.h"
#include "twobody.h"

#include "orbitalelements.h"
#include "oeconvert.h"

#include "includes.h"

using namespace entityx;

void drawEllipse(int segments, std::vector<float> &path, GLdouble semiMajor, GLdouble semiMinor)
{
    float DEG2RAD = M_PI/180.0f;
    
    //each line segment requires its own vertices
    for(int i=0;i<segments;i++)
    {
        GLdouble rad = i*DEG2RAD;
        path.push_back(cos(rad)*semiMajor);
        path.push_back(sin(rad)*semiMinor);
        path.push_back(0);
        path.push_back(cos(rad)*semiMajor);
        path.push_back(sin(rad)*semiMinor);
        path.push_back(0);
    }
    //pull the first vertex out and stuff it in the back
    for(int i=0; i<3; ++i)
    {
        path.push_back(path.front());
        path.erase(path.begin());
    }
    
    //add center for debugging
    for(int i=0; i<3; ++i)
        path.push_back(path[i]);
    for(int i=0; i<6; ++i)
        path.push_back(0);
    path.push_back(0);
    path.push_back(0);
    path.push_back(1e3);
}

void drawOrbitalPath(int segments, std::vector<float> &path, GLdouble a, GLdouble e, GLdouble tra)
{
    path.clear();
    path.reserve(segments * 3 * 2 + 12);
    assert(path.empty());
    
    tra = (tra >= M_PI) ? tra - 2*M_PI : tra;
    for(float theta = (a > 0) ? -M_PI : tra;
        theta < M_PI;
        theta += M_PI*2/segments)
    {
        float denominator = (1.0f+e*cos(theta));
        if (denominator < 0) {
//            continue;
        }
        auto r = a*(1.0f-e*e)/denominator;
        auto x=r*cos(theta);
        auto y=r*sin(theta);
        path.push_back(x);
        path.push_back(y);
        path.push_back(0);
        path.push_back(x);
        path.push_back(y);
        path.push_back(0);
    }
   
    assert(not path.empty());
        //pull the first vertex out and stuff it in the back
    for(int i=0; i<3; ++i)
    {
        if (a < 0) {
            path.erase(path.end()-1);
        } else {
            path.push_back(path.front());
        }
        path.erase(path.begin());
    }
   
//    if (a >= 0)
//    {
//        //add center for debugging
//        for(int i=0; i<3; ++i)
//            path.push_back(path[i]);
//        for(int i=0; i<6; ++i)
//            path.push_back(0);
//        path.push_back(0);
//        path.push_back(0);
//        path.push_back(1e3);
//    }
    assert(not path.empty());
}


void OrbitalPhysicsSystem::update(EntityManager & entities,
                             EventManager &events,
                             double dt)
{
    /**
     *  Refactor goal: update only based on events
     *      such as new entity event,
     *          detection event, 
     *          velocity change events,
     *          periodic events
     */
    /*
     *  All of this just to compute orbital paths using this system
     */
    OrbitPath::Handle orbit;
    Position::Handle position;
    Velocity::Handle velocity;
   
    for (Entity entity: entities.entities_with_components(orbit, position, velocity))
    {
        auto parentEntityID = entity.component<Parent>()->parent;
        auto parentEntity = entities.get(parentEntityID);
        auto parentPosition = parentEntity.component<Position>();
        auto parentVelocity = parentEntity.component<Velocity>();
        auto parentGM = parentEntity.component<GM>();
        auto &orbitPath = orbit->path;
        auto orbitPathSteps = 360;

        
        //draw a flat elipse
        std::vector<double> posVel = toPosVelVector(position->pos, velocity->vel);
        auto oe = rv2oe(parentGM->gm, posVel);
//        assert(oe.lan == oe.lan);
//        assert(oe.aop == oe.aop);
        
        drawOrbitalPath(orbitPathSteps, orbitPath, oe.sma, oe.ecc, oe.tra);
        
        auto aop = glm::rotate(glm::mat4(), (float)(oe.aop), glm::vec3(0, 0, 1));
        auto inc = glm::rotate(glm::mat4(), (float)(oe.inc), glm::vec3(1, 0, 0));
        auto lan = glm::rotate(glm::mat4(), (float)(oe.lan), glm::vec3(0, 0, 1));
        orbit->transform  = lan * inc * aop;
        
        if (entity.has_component<Exempt>())// and entity.component<Exempt>()->linearDynamics)
        {
            continue;
        }
        //calculate next position/velocity for this object
        VectorD params = convertToParams(parentPosition->pos, parentGM->gm, velocity->getAccel());
        
        t_integrator integrator = &rungeKutta4;
        t_dynamics dynamics = &twobody_perturbed;
        posVel = integrator(dynamics, 0, dt, posVel, params);
        position->pos = glm::vec3(posVel[0],
                                  posVel[1],
                                  posVel[2]);
        velocity->vel = glm::vec3(posVel[3],
                                  posVel[4],
                                  posVel[5]);
        assert(not orbit->path.empty());
        
    }
////////DEBUG/////////////////////////////////////

    //print stuff to screen
    auto printOE = [&](std::string name, float element, entityx::Entity entity)
    {
        auto orbitParamString = name + to_string_with_precision(element);
        events.emit<GUITextEvent>(entity, 15.0f, orbitParamString);
    };
    
    auto UITextSetup = [&](){
        Ship::Handle ship;
        Missile::Handle missile;
        Position::Handle position;
        Velocity::Handle velocity;
        OrbitPath::Handle orbit;
        
        
        for (auto entity : entities.entities_with_components(ship, position, velocity, orbit))
        {
            assert(entity.has_component<Position>());
            events.emit<GUITextEvent>(entity,
                                      15.0f,
                                      ship->debugName);
            
            //print out orbital elements
            auto parentEntityID = entity.component<Parent>()->parent;
            auto parentEntity = entities.get(parentEntityID);
            //            auto parentPosition = parentEntity.component<Position>();
            auto parentGM = parentEntity.component<GM>();
            auto posVel = toPosVelVector(position->pos, velocity->vel);
            auto oe = rv2oe(parentGM->gm, posVel);
            
            printOE("sma: ", oe.sma, entity);
            printOE("ecc: ", oe.ecc, entity);
            printOE("inc: ", oe.inc, entity);
            printOE("aop: ", oe.aop, entity);
            printOE("lan: ", oe.lan, entity);
            printOE("tra: ", oe.tra, entity);
            
        }
        for (auto entity : entities.entities_with_components(missile, position, orbit))
        {
            (void) entity;
            events.emit<GUITextEvent>(entity,
                                      15.0f,
                                      missile->debugName);
        }
    };
    UITextSetup();
////////DEBUG/////////////////////////////////////
        
    
}
