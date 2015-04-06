//
//  orbitalPhysicsSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/18/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "orbitalPhysicsSystem.h"
#include "componentTypes.h"

#include "ecs.h" //FIXME: get rid of ASAP!
#include "vectord.h"
#include "integration.h"
#include "twobody.h"

#include "orbitalelements.h"
#include "oeconvert.h"

using namespace entityx;

void drawEllipse(int segments, vector<float> &path, GLdouble semiMajor, GLdouble semiMinor)
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

void drawOrbitalPath(int segments, vector<float> &path, GLdouble a, GLdouble e, GLdouble tra)
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
            continue;
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
    ComponentHandle<OrbitPath> orbit;
    ComponentHandle<Position> position;
    ComponentHandle<Velocity> velocity;
   
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
        
        auto aop = glm::rotate(glm::mat4(), (float)(oe.aop*180/M_PI), glm::vec3(0, 0, 1));
        auto inc = glm::rotate(glm::mat4(), (float)((oe.inc*180)/M_PI), glm::vec3(1, 0, 0));
        auto lan = glm::rotate(glm::mat4(), (float)(oe.lan*180/M_PI), glm::vec3(0, 0, 1));
        orbit->transform  = lan * inc * aop;
        
        //calculate next position/velocity for this object
        VectorD params = convertToParams(parentPosition->pos, parentGM->gm);
        
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
        
////////DEBUG/////////////////////////////////////
        string message;
#if 1
//        message = "r: " + to_string(position->pos.x)
//        + " " + to_string(position->pos.y)
//        + " " + to_string(position->pos.z);
//        events.emit(DebugEvent(message));
//        
//        message = "v: " + to_string(velocity->vel.x)
//        + " " + to_string(velocity->vel.y)
//        + " " + to_string(velocity->vel.z);
//        events.emit(DebugEvent(message));
        
        message = "sma: " + to_string(oe.sma);
        events.emit(DebugEvent(message));
//        message = "ecc: " + to_string(oe.ecc);
//        events.emit(DebugEvent(message));
//        message = "inc: " + to_string(oe.inc);
//        events.emit(DebugEvent(message));
//        message = "lan: " + to_string(oe.lan);
//        events.emit(DebugEvent(message));
//        message = "aop: " + to_string(oe.aop);
//        events.emit(DebugEvent(message));
        message = "tra: " + to_string(oe.tra);
        events.emit(DebugEvent(message));
#else
        message = "orbits: " + to_string(orbitCount);
        events.emit(DebugEvent(message));
#endif
////////DEBUG/////////////////////////////////////
        
    }
    
//    for (Entity entity: entities.entities_with_components(position, velocity))
//    {
//        auto parentEntityID = entity.component<Parent>()->parent;
//        auto parentEntity = entities.get(parentEntityID);
//        auto parentPosition = parentEntity.component<Position>();
//        auto parentVelocity = parentEntity.component<Velocity>();
//        auto parentGM = parentEntity.component<GM>();
//        auto &orbitPath = orbit->path;
//        auto orbitPathSteps = 360;
//        
//        //TODO: move the integration from the loop above to here (so all elements with position/velocity can still move about, only those w/ orbit components need to recompute orbits
//    }
}