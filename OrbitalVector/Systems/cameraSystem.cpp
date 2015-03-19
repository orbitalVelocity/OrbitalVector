//
//  cameraSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/19/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "cameraSystem.h"
#include "componentTypes.h"
using namespace entityx;

void CameraSystem::update(EntityManager & entities,
                                  EventManager &events,
                                  double dt)
{
    ComponentHandle<CameraComponent> camera;
    
    for (Entity entity: entities.entities_with_components(camera))
    {
        //call a function to get stuff
        //update position/velocity of self, ignore effects on parent?
        
        //update orbital elements?
        //errors?
    }
}


//COMPONENT(Camera)
//{
//    Camera () {}
//    
//    glm::vec3 position;
//    glm::vec3 focus;
//    
//    float fov;
//    float hAngle, vAngle;
//    float nearPlane, farPlane;
//    float ratio;
//};