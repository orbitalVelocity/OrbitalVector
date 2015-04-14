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

class MenuBase : public OGLShader
{
    std::vector<GLuint> vaos;
    std::vector<int> drawCounts;
public:
    MenuBase(GLenum _drawType);
    
    void init() override;
    void update(entityx::EntityManager &, entityx::EventManager &, float dt);
    void mouseUpdate(glm::vec2 mb, bool lmb, bool rmb, entityx::Entity s);
    void draw(glm::mat4, entityx::EntityManager &);
};
#endif /* defined(__OrbitalVector__MenuBase__) */
