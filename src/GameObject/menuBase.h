//
//  MenuBase.h
//  OrbitalVector
//
//  Created by Si Li on 4/14/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__menuBase__
#define __OrbitalVector__menuBase__

#include <stdio.h>
#include <stdio.h>
#include "OGLShader.h"
#include "entityx/Entity.h"
#include <iostream>
#include <set>
#include "includes.h"
#include "componentTypes.h"

class MenuBase : public OGLShader
{
public:
    std::vector<GLuint> vaos;
    std::vector<int> drawCounts;
public:
    MenuBase(GLenum _drawType);
    
    void loadGUIMesh(int meshID, int GUIElementID);
    virtual void init(){}
    
    void animate(UIElement &, float dt);
    void triggerDelayedStart(float elapsedTime, GUICircleMenu::Handle);
    virtual void update(entityx::EntityManager &, entityx::EventManager &, float dt){}
    virtual void draw(glm::mat4, entityx::EntityManager &) {}
};
#endif /* defined(__OrbitalVector__MenuBase__) */
