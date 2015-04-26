//
//  playerControlComponent.h
//  OrbitalVector
//
//  Created by Si Li on 4/26/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__playerControlComponent__
#define __OrbitalVector__playerControlComponent__

#include <stdio.h>
#include "entityx/Entity.h"
#include "componentTypes.h"


COMPONENT(PlayerControl)
{
    PlayerControl() {}
    
    void newFocus(entityx::Entity newFocus)
    {
        lastEntityFocused = focusOnEntity;
        focusOnEntity = newFocus;
        switchedFocus = true;
        currentTime = 0;
    }
    float getProgress(float dt)
    {
        currentTime += dt;
        if (currentTime > animationTime)
            switchedFocus = false;
        
        auto t = currentTime/animationTime;
        auto factor = 5.0;
        return (1.0 - pow((1.0 - t), 2 * factor));
    }
    
    float animationTime = .8;
    float currentTime = 0;
    bool switchedFocus;
    entityx::Entity focusOnEntity, lastEntityFocused;
    std::vector<entityx::Entity> selectedEntities;
    std::vector<entityx::Entity> mouseOverEntities;
    
    entityx::Entity shadowEntity; //for orbit planning
};
#endif /* defined(__OrbitalVector__playerControlComponent__) */
