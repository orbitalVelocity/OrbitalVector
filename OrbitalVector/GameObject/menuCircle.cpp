//
//  menuCircle.cpp
//  OrbitalVector
//
//  Created by Si Li on 4/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "menuCircle.h"
#include "componentTypes.h"
#include "scene.h"

using namespace std;

MenuCircle::MenuCircle(GLenum _drawType) : OGLShader(_drawType)
{
    
}

void MenuCircle::init()
{
    auto shipIdx = 1;
    loadShaders("spriteVertex.glsl", "spriteFragment.glsl", true);
    loadAttribute("position", shapes[shipIdx].mesh.positions, GL_STATIC_DRAW);
    check_gl_error();
    loadAttribute("normal", shapes[shipIdx].mesh.normals, GL_STATIC_DRAW);
    setupBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, shapes[shipIdx].mesh.indices);
}
void MenuCircle::mouseUpdate(glm::vec2 mb, bool lmb, bool rmb, entityx::Entity s)
{
    mbPosition = mb;
    
}
void MenuCircle::update(entityx::EntityManager &entities,
                        entityx::EventManager &events,
                        float dt)
{
    GUICircleMenu::Handle circle;
    //get cursor screen position
    for (auto entity : entities.entities_with_components(circle))
    {
        auto positionHandle = circle->target.component<Position>();
        //animate
        //state transition
        //animate based on state and time elapsed in state
        //manipulate transform
        //check if cursor is over an interactive element
        //change state if so
        //emit event when player has triggered an action
        
    }
}
void MenuCircle::MenuCircle::draw(glm::mat4 camera, entityx::EntityManager &entities)
{
    glm::vec3 color(1,.8,.8);
    GUICircleMenu::Handle circle;
    //draw each element w/ the associated transform
    glUseProgram(shaderProgram);
    for (auto entity : entities.entities_with_components(circle))
    {
        auto positionHandle = circle->target.component<Position>();
        auto centralPos = glm::vec3(world * glm::vec4(positionHandle->pos, 1.0));
        auto loc = glGetUniformLocation(shaderProgram, "centralPos");
        glUniform3fv(loc, 1, glm::value_ptr(centralPos));
        drawIndexed(camera, color);
    }
}