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
    loadShaders("spriteVertex.glsl", "spriteFragment.glsl", true);
   
    //FIXME: need a central factility to map mesh names to mesh vector,
    // and mesh names to vao/vbo id

    auto shipIdx = 4;
    auto numMeshes = 2;
    vaos.resize(numMeshes);
    drawCounts.resize(numMeshes);
    glGenVertexArrays(numMeshes, &vaos[0]);
    auto genGLNames = [&](int i)
    {
        loadAttribute(vaos[i], "position", shapes[shipIdx].mesh.positions, GL_STATIC_DRAW);
        check_gl_error();
        loadAttribute(vaos[i],"normal", shapes[shipIdx].mesh.normals, GL_STATIC_DRAW);
        setupBuffer(vaos[i], GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, shapes[shipIdx].mesh.indices);
        drawCounts[i] = shapes[shipIdx].mesh.indices.size();
    };
    
    genGLNames(0);
        shipIdx++;
    genGLNames(1);
}
void MenuCircle::mouseUpdate(glm::vec2 mb, bool lmb, bool rmb, entityx::Entity s)
{
    mbPosition = mb;
    
}

template<typename T>
T lerp(T a, T b, float t)
{
    return (1-t)*a + t*b;
}

float spring(float factor, float progress)
{
    auto x = progress;
    progress = pow(2, -10*x)
            * sin((x - factor / 4) * (2 * M_PI)/factor)
            + 1.0;
    return progress;
}
void MenuCircle::update(entityx::EntityManager &entities,
                        entityx::EventManager &events,
                        float dt)
{
    GUICircleMenu::Handle circle;
    //get cursor screen position
    float scaleBegin(0.05);
    float scaleEnd(0.1);
    
    float rotateBegin = 0;
    float rotateEnd = M_PI/4;
    auto animate = [&](UIElement &element)
    {
        auto &elapsedTime = element.time.elapsedTime;
        auto &totalTime = element.time.totalTime;
        if (elapsedTime >= totalTime) {
            element.state = AnimationState::end;
            return;
        }
        
        auto progress = elapsedTime/totalTime;
        auto progress1 = spring(0.4, progress);
        glm::vec2 screenAspectRatio(1, 1.6);
        element.scale2d = screenAspectRatio * lerp(scaleBegin, scaleEnd, progress1);
        
        auto progress2 = spring(0.8, progress);
        element.rotateByRadian = element.initialRotation + lerp(rotateBegin, rotateEnd, progress2);

        elapsedTime += dt;
    };
    
    //count until leaf elements also start
    //NOTE: leaf delayStartTime must be less than center.time.totalTime!
    //also delayedStart time is sorted increasing order
    auto checkDelayedStart = [&](float elapsedTime, GUICircleMenu::Handle circle)
    {
        for (auto &leaf : circle->leafMenus)
        {
            if (AnimationState::invalid == leaf.state
                and elapsedTime > leaf.time.delayedStartTime) {
                leaf.state = AnimationState::start;
            }
        }
    };
    
    for (auto entity : entities.entities_with_components(circle))
    {
        auto positionHandle = circle->target.component<Position>();
        //animate
        //state 0: expand center element
        auto &center = circle->centerElement;
        auto &elapsedTime = center.time.elapsedTime;
        auto &totalTime = center.time.totalTime;
        if (AnimationState::start == center.state)
        {
            animate(center);
        }
        
        checkDelayedStart(elapsedTime, circle);
        
        for (auto& leaf: circle->leafMenus)
        {
            if (AnimationState::start == leaf.state)
            {
                animate(leaf);
            }
        }
        
        //state 1: fling out leaf elements
        //do what the center thing did
        
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