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

class UserInputSystem : public entityx::System<UserInputSystem>,
                        public entityx::Receiver<PotentialSelectEvent>
{
public:
    UserInputSystem(GLFWwindow *w, UserInput *ui);
    void configure(entityx::EventManager& eventManager);
    
    void init(UserInput *ui);
    void update(entityx::EntityManager &entities,
                entityx::EventManager &events,
                double dt);
    
    void receive(const PotentialSelectEvent &e);
    
private:
    GLFWwindow *pWindow;
    entityx::Entity potentiallySelectedEntity;
    entityx::Entity hoverOverEntity;
    UserInput* legacyUserInput = nullptr;   //get rid of this when userInput is absorbed into this class
    
    std::vector<entityx::Entity> mouseOverEntities;
    std::vector<entityx::Entity> selectedEntities;
};
#endif /* defined(__OrbitalVector__userInputSystem__) */
