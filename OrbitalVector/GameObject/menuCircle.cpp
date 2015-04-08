//
//  menuCircle.cpp
//  OrbitalVector
//
//  Created by Si Li on 4/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "menuCircle.h"
#include "scene.h"

using namespace std;

MenuCircle::MenuCircle(GLenum _drawType) : OGLShader(_drawType)
{
    auto shipIdx = 1;
    loadShaders("spriteVertex.glsl", "spriteFragment.glsl", true);
    loadAttribute("position", shapes[shipIdx].mesh.positions, GL_STATIC_DRAW);
    check_gl_error();
    loadAttribute("normal", shapes[shipIdx].mesh.normals, GL_STATIC_DRAW);
    setupBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, shapes[shipIdx].mesh.indices);
}

void MenuCircle::update(entityx::EntityManager &entities,
                        entityx::EventManager &events,
                        float dt)
{
    //get cursor screen position
    //animate
    //state transition
    //animate based on state and time elapsed in state
    //manipulate transform
    //check if cursor is over an interactive element
    //change state if so
    //emit event when player has triggered an action
    
}
void MenuCircle::MenuCircle::draw()
{
    //draw each element w/ the associated transform
}