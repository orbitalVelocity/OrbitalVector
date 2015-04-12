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
void MenuCircle::update(entityx::EntityManager &entities,
                        entityx::EventManager &events,
                        float dt)
{
    GUICircleMenu::Handle circle;
    //get cursor screen position
    glm::vec2 animateBegin(0.05);
    glm::vec2 animateEnd(0.1);
    for (auto entity : entities.entities_with_components(circle))
    {
        auto positionHandle = circle->target.component<Position>();
        //animate
        //state 0: expand center element
        if (0 == circle->animationState)
        {
            auto &elapsedTime = circle->centerElement.time.elapsedTime;
            auto &totalTime = circle->centerElement.time.totalTime;
            if (elapsedTime >= totalTime) {
                circle->animationState++;
//                elapsedTime = 0;
                break;
            }
            
            auto progress = elapsedTime/totalTime;
//            progress = pow(progress, 4);
//            progress = sqrt(progress);
            auto x = progress;
            auto factor = 0.5;
            progress = pow(2, -10*x)
                     * sin((x - factor / 4) * (2 * M_PI)/factor)
                     + 1.0;
            circle->scale2d = glm::vec2(1,1.6) * glm::lerp(animateBegin, animateEnd, progress);
            elapsedTime += dt;
//            std::cout << totalTime << "," << elapsedTime
//                      << "," << x << ","
//                      << progress << "\n";
            
           
        }
        //state 1: fling out leaf elements
        
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
        glUniform2fv(loc, 1, glm::value_ptr(circle->offset2d));
        loc = glGetUniformLocation(shaderProgram, "scale2d");
        glUniform2fv(loc, 1, glm::value_ptr(circle->scale2d));
        drawIndexed(vaos[0], drawCounts[0], camera, color);
                    //extend leaves
        
        auto rotate2d = [](float theta)
        {
            float array[] = {cos(theta), -sin(theta), sin(theta), cos(theta)};
            glm::mat2 temp = glm::make_mat2(array);
            return temp;
//            return glm::tmat2x2(cos(theta), -sin(theta), sin(theta), cos(theta));
        };
        
        for (auto leafRotation : circle->leafMenus)
        {
            std::cout << "rotation: " << leafRotation;
//            auto leafOffset = rotate2d(leafRotation) * circle->offset2d;
            auto rotation = rotate2d(leafRotation);
            auto leafOffset = rotation * circle->offset2d;
            std::cout << "rotate offset: " << leafOffset.x << " " << leafOffset.y
            << "\n";
            loc = glGetUniformLocation(shaderProgram, "offset2d");
            glUniform2fv(loc, 1, glm::value_ptr(leafOffset));
            drawIndexed(vaos[1], drawCounts[1], camera, color);
        }
        
    }
}