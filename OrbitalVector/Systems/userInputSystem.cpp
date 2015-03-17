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

/*
void UserInputSystem::update(entityx::EntityManager &entities,
                             entityx::EventManager &events,
                             double dt)
{
    assert(legacyUserInput != nullptr);
    
    if (potentiallySelectedEntity.valid())
    {
        ComponentHandle<UISelection> UISelectComponent = entities.component<UISelection>(potentiallySelectedEntity.id());
        
        auto cameraRotateMode = legacyUserInput->rmbPressed;
        auto selectionMode = legacyUserInput->lmbPressed and not cameraRotateMode;
        auto hoverMode = not selectionMode;

        //semantics
            //only 1 selection at a time
                //if selection changes, previously selected is deselected
            //only 1 hover at a time
                //if hover changes, previous hover is no longer hover
            //selection takes priority over hover
        
        //if selectionMode
            //assign if component not there or set component to selected
                //just call replace, it will check if valid and if not assign
            //unassign all previously selected components
        //if hoverMode
            //set to hover if not selected
            //remove other components that is HOVER
        //optimizations:
            //check if potentiallySelectedEntity already has the intended status, if so, skip
        auto enableMultiSelection = false;
        if (selectionMode and not enableMultiSelection) {
            if (UISelectComponent.valid() and UISelectionType::SELECTED != UISelectComponent->type) {
                for (Entity entity : entities.entities_with_components(UISelectComponent))
                {
                    entities.remove<UISelection>(entity.id());
                }
                potentiallySelectedEntity.replace<UISelection>(UISelectionType::SELECTED); //FIXME: does this actually work for the place, should entity has a pointer to its EntityManager
            }
        } //if (selectionMode and enableMultiSelection) { }
        else if (hoverMode)
        {
            if (UISelectionType::HOVER not_eq UISelectComponent->type) {
                for (Entity entity : entities.entities_with_components(UISelectComponent))
                {
                    if (UISelectionType::HOVER == UISelectComponent->type) {
                        entities.remove<UISelection>(entity.id());
                    }
                }
                if (not UISelectComponent.valid()) {
                    entities.assign<UISelection>(potentiallySelectedEntity.id(), UISelectionType::HOVER);
                }
            }
        }
        
        if (cameraRotateMode) {
            //camera rotations
        }
//        hoverOverEntity = potentiallySelectedEntity;
        potentiallySelectedEntity.invalidate();
    }
//    
//    PlayerControl::Handle playerControl;
//    Velocity::Handle velocity;
//    Weapon::Handle weapon;
//    for (Entity entity : entities.entities_with_components(playerControl, velocity, weapon))
//    {
//        
//    }   
}
 */
