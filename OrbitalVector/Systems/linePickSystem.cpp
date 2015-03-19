//
//  linePickSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/15/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "linePickSystem.h"
#include "ecs.h"
#include "componentTypes.h"
#include "events.h"

using namespace entityx;

LinePickSystem::LinePickSystem()
{
    
}


void LinePickSystem::update(EntityManager & entities,
                            EventManager &events,
                            double dt,
                            GLFWwindow *pWindow,
                            Camera *pCamera)
{
    //construct mouse/cursor casted ray
    double mouseX, mouseY;
    int screenWidth, screenHeight;
    glfwGetCursorPos(pWindow, &mouseX, &mouseY);
    glfwGetWindowSize(pWindow, &screenWidth, &screenHeight);
    
    //convert coordinate system from [0,1] to [-1, 1] for both x and y
    mouseY = screenHeight - mouseY; //for some reason, mouseY is flipped from tutorial
    auto mouseX_NDC = ((float)mouseX/(float)screenWidth  - 0.5f) * 2.0f;
    auto mouseY_NDC = ((float)mouseY/(float)screenHeight - 0.5f) * 2.0f;
    auto mouse_NDC = glm::vec2(mouseX_NDC * screenWidth, mouseY_NDC * screenHeight);

    //for each entity, check if it is within the threshold of clickability,
    //pick the closest entity as a selectable entity
    //emits an event when there is a selectable entity,
    //and on state change from valid to invalid selection
    Position::Handle position;
    float shortestDistance = INFINITY;  //distance between entity and camera
    Entity selectableEntity;
    for (Entity entity : entities.entities_with_components(position))
    {
        
        //convert entity position from 3D to screen NDC space
        auto posNDC = pCamera->matrix() * world * glm::vec4(position->pos, 1.0);
        posNDC /= posNDC.w;
        auto screenPosNDC = glm::vec2(posNDC.x * screenWidth, posNDC.y * screenHeight);
        auto onScreenDistance = glm::length(mouse_NDC - screenPosNDC);
        
        auto distanceFromCamera = glm::length(position->pos - pCamera->position);
        
        //FIXME: threshold also depends on aparent size of object
        const int thresholdInPixels = 40;

        //pick a clickable entity that is also the closest
        if (onScreenDistance < thresholdInPixels
            && distanceFromCamera < shortestDistance) {
            shortestDistance = distanceFromCamera;
            selectableEntity = entity;
        }
    }
    
    //emit event for clickability
    if (selectableEntity.valid()) {
        std::cout << "can select entity " << selectableEntity.id() << std::endl;
        events.emit<PotentialSelectEvent>(selectableEntity);
        previousSelectableEntity = selectableEntity;
    } else if (previousSelectableEntity.valid()){
        events.emit<PotentialSelectEvent>(selectableEntity);
        previousSelectableEntity = selectableEntity;
    }
    
}