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

MenuCircle::MenuCircle(GLenum _drawType) : MenuBase(_drawType)
{
    
}

void MenuCircle::init()
{
    auto numMeshes = 2;
    vaos.resize(numMeshes);
    drawCounts.resize(numMeshes);
    glGenVertexArrays(numMeshes, &vaos[0]);
    //translate mesh name to meshID using a mesh manager singleton
    loadGUIMesh(4, 0);
    loadGUIMesh(5, 1);
}



void MenuCircle::update(entityx::EntityManager &entities,
                        entityx::EventManager &events,
                        float dt)
{
    GUICircleMenu::Handle circle;
    
    for (auto entity : entities.entities_with_components(circle))
    {
        assert(circle->target.valid());
        auto positionHandle = circle->target.component<Position>();
        //animate
        //state 0: expand center element
        auto &center = circle->centerElement;
        auto &elapsedTime = center.time.elapsedTime;
        auto &totalTime = center.time.totalTime;
        if (AnimationState::start == center.state)
        {
            animate(center, dt);
        }
        
        triggerDelayedStart(elapsedTime, circle);
        
        for (auto& leaf: circle->leafMenus)
        {
            if (AnimationState::start == leaf.state)
            {
                animate(leaf, dt);
            }
        }
        
        //always: hit test: change color if one of them is hit
        //based on state
        
        //animate based on state and time elapsed in state
        //manipulate transform
        //check if cursor is over an interactive element
        //change state if so
        //emit event when player has triggered an action
        
    }
}
void MenuCircle::MenuCircle::draw(glm::mat4 camera, entityx::EntityManager &entities)
{
    glm::vec3 color(1,.1,.1);
    GUICircleMenu::Handle circle;
    //draw each element w/ the associated transform
    glUseProgram(shaderProgram);
    for (auto entity : entities.entities_with_components(circle))
    {
        auto positionHandle = circle->target.component<Position>();
        auto centralPos = glm::vec3(world * glm::vec4(positionHandle->pos, 1.0));
        
        //for each component
        //draw center element + leaf elements
        auto loc = glGetUniformLocation(shaderProgram, "centralPos");
        glUniform3fv(loc, 1, glm::value_ptr(centralPos));
        loc = glGetUniformLocation(shaderProgram, "offset2d");
        glUniform2fv(loc, 1, glm::value_ptr(circle->centerElement.offset2d));
        loc = glGetUniformLocation(shaderProgram, "scale2d");
        glUniform2fv(loc, 1, glm::value_ptr(circle->centerElement.scale2d));
        loc = glGetUniformLocation(shaderProgram, "rotate2d");
        glUniformMatrix2fv(loc, 1, false, glm::value_ptr(glm::mat2()));
        drawIndexed(vaos[0], drawCounts[0], camera, color);
                    //extend leaves
        
        auto rotate2d = [](float theta)
        {
            float array[] = {cos(theta), -sin(theta), sin(theta), cos(theta)};
            glm::mat2 temp = glm::make_mat2(array);
            return temp;
        };
        
        for (auto leaf: circle->leafMenus)
        {
            loc = glGetUniformLocation(shaderProgram, "scale2d");
            glUniform2fv(loc, 1, glm::value_ptr(leaf.scale2d));

            auto rotation = rotate2d(leaf.rotateByRadian);
            loc = glGetUniformLocation(shaderProgram, "rotate2d");
            glUniformMatrix2fv(loc, 1, false, glm::value_ptr(rotation));
            
            drawIndexed(vaos[1], drawCounts[1], camera, color);
        }
        
    }
}