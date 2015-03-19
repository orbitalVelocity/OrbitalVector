//
//  linePickSystem.h
//  OrbitalVector
//
//  Created by Si Li on 3/15/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__linePickSystem__
#define __OrbitalVector__linePickSystem__

#include <stdio.h>
//get rid of these two ASAP!
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "entityx/System.h"
#include "camera.h"

/**
 * Finds closest entity the cursor is over
 * Emmits a "potentially selected" event
 * Up to userInputSystem to determine what to do with this
 */
class LinePickSystem : public entityx::System<LinePickSystem>
{
private:
    entityx::Entity previousSelectableEntity;
public:
    
    LinePickSystem();
    
    //consider a wrapper around *window to store
    //cursor position, screen dimensions, framebuffer dimensions
    void update(entityx::EntityManager & entities,
                entityx::EventManager & events,
                double dt,
                GLFWwindow *w, Camera *c);

    void update(entityx::EntityManager & entities,
                entityx::EventManager & events,
                double dt) override {}
};
#endif /* defined(__OrbitalVector__linePickSystem__) */
