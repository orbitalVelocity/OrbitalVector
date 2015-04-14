//
//  menuCircle.h
//  OrbitalVector
//
//  Created by Si Li on 4/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__menuCircle__
#define __OrbitalVector__menuCircle__

#include <stdio.h>
#include "menuBase.h"
#include "entityx/Entity.h"
#include <iostream>
#include <set>
#include "includes.h"

class MenuCircle : public MenuBase
{
public:
    MenuCircle(GLenum _drawType);
    
    void init() override;
    void update(entityx::EntityManager &, entityx::EventManager &, float dt);
    void mouseUpdate(glm::vec2 mb, bool lmb, bool rmb, entityx::Entity s);
    void draw(glm::mat4, entityx::EntityManager &);
};
#endif /* defined(__OrbitalVector__menuCircle__) */
