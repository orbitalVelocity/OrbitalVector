//
//  userInputSystem.h
//  OrbitalVector
//
//  Created by Si Li on 3/15/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__userInputSystem__
#define __OrbitalVector__userInputSystem__

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "entityx/System.h"
#include "events.h"
#include "input.h" //legacy

/**
 * Grabs user input from the UserInput class 
 *      FIXME: (UserInput is legacy, needs to move that into this system... maybe?)
 *      on the other hand, this helps keep a separation of system and state 
 *          (although... I still need to keep state, so n/m)
 * Anyway, this turns raw user inputs into actionable commands and emmits those commands into 
 * a commandProcessorSystem
 *      TODO: implement commandProcessorSystem; for now, keep that in this class as AI doesn't exist yet
 */
class UserInputSystem : public entityx::System<UserInputSystem>,
                        public entityx::Receiver<PotentialSelectEvent>
{
public:
    UserInputSystem(UserInput *ui, std::vector<entityx::Entity> &selected);
    void configure(entityx::EventManager& eventManager);
    
    void update(entityx::EntityManager &entities,
                entityx::EventManager &events,
                double dt,
                UserInput *ui,
                entityx::Entity e);
    
    void update(entityx::EntityManager & entities,
                entityx::EventManager & events,
                double dt) override {}
    
    void receive(const PotentialSelectEvent &e);
    
private:
    GLFWwindow *pWindow;
    UserInput* legacyUserInput = nullptr;   //get rid of this when userInput is absorbed into this class
    entityx::Entity myShip;
    
    entityx::Entity potentiallySelectedEntity;
    entityx::Entity hoverOverEntity;
    
    std::vector<entityx::Entity> mouseOverEntities;
    std::vector<entityx::Entity> &selectedEntities;
};
#endif /* defined(__OrbitalVector__userInputSystem__) */
