//
//  userInputSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/15/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "userInputSystem.h"
#include "componentTypes.h"

using namespace entityx;

UserInputSystem::UserInputSystem(GLFWwindow *w, UserInput *ui) : pWindow(w), legacyUserInput(ui)
{
    
}

void UserInputSystem::configure(EventManager& eventManager)
{
    eventManager.subscribe<PotentialSelectEvent>(*this);
}

/**
 *  only gets called when a mouse hovers over an entity
 */
void UserInputSystem::receive(const PotentialSelectEvent& e)
{
    potentiallySelectedEntity = e.entity;
    assert(potentiallySelectedEntity.valid());
    
    //assuming select one thing at a time
    auto cameraRotateMode = legacyUserInput->rmbPressed;
    auto selectionMode = legacyUserInput->lmbPressed and not cameraRotateMode;
    
    auto enableMultiSelection = false;
    if (not enableMultiSelection) {
        if (selectionMode) {
            selectedEntities.clear();
        }
        mouseOverEntities.clear();
    }
    
    if (selectionMode) {
        selectedEntities.emplace_back(potentiallySelectedEntity);
        std::cout << "selected: " << potentiallySelectedEntity.id() << std::endl;
        
    } else { //else hovermode
        mouseOverEntities.emplace_back(potentiallySelectedEntity);
        std::cout << "hovered: " << potentiallySelectedEntity.id() << std::endl;
    }

    //what happens when the cursor moves? is that called in a callback??
}

void UserInputSystem::init(UserInput *ui)
{
    legacyUserInput = ui;
}

void UserInputSystem::update(entityx::EntityManager &entities,
                             entityx::EventManager &events,
                             double dt)
{
 
}
