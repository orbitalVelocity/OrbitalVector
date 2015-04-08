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
#include "camera.h" //legacy

/**
 * Grabs user input from the UserInput class 
 *      FIXME: (UserInput is legacy, needs to move that into this system... maybe?)
 *      on the other hand, this helps keep a separation of system and state 
 *          (although... I still need to keep state, so n/m)
 * Anyway, this turns raw user inputs into actionable commands and emmits those commands into 
 * a commandProcessorSystem
 *      TODO: implement commandProcessorSystem; for now, keep that in this class as AI doesn't exist yet
 */
class UserInputSystem : public entityx::System<UserInputSystem>
//                        public entityx::Receiver<PotentialSelectEvent>
{
public:
    UserInputSystem(UserInput *ui);
    
    void configure(entityx::EventManager& eventManager);
//    void receive(const PotentialSelectEvent &e);
    
    void update(entityx::EntityManager &entities,
                entityx::EventManager &events,
                double dt,
                UserInput *ui,
                entityx::Entity e,
                GLFWwindow *w, Camera &);
    
    void  update(entityx::EntityManager & entities,
                entityx::EventManager & events,
                double dt) override {}

    entityx::Entity linePick(entityx::EntityManager & entities,
                  GLFWwindow *w, Camera &c);

void updateMouseSelection(entityx::EntityManager &entities, entityx::Entity selectableEntity);
    
    void updateCamera();

    void processAction(entityx::EntityManager &entities, entityx::Entity myShip);

private:
    GLFWwindow *pWindow;
    UserInput* legacyUserInput = nullptr;   //get rid of this when userInput is absorbed into this class
};
#endif /* defined(__OrbitalVector__userInputSystem__) */
