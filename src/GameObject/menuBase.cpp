//
//  menuBase.cpp
//  OrbitalVector
//
//  Created by Si Li on 4/14/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "menuBase.h"
#include "scene.h"

using namespace std;

MenuBase::MenuBase(GLenum _drawType) : OGLShader(_drawType)
{
    loadShaders("spriteVertex.glsl", "spriteFragment.glsl", true);
    
}

void MenuBase::loadGUIMesh(int meshID, int GUIElementID)
{
    
    //FIXME: need a central factility to map mesh names to mesh vector,
    // and mesh names to vao/vbo id
    loadAttribute(vaos[GUIElementID], "position", shapes[meshID].mesh.positions, GL_STATIC_DRAW);
    check_gl_error();
    loadAttribute(vaos[GUIElementID],"normal", shapes[meshID].mesh.normals, GL_STATIC_DRAW);
    setupBuffer(vaos[GUIElementID], GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, shapes[meshID].mesh.indices);
    drawCounts[GUIElementID] = (int)shapes[meshID].mesh.indices.size();
}


void MenuBase::animate(UIElement &element, float dt)
{
    float scaleBegin(0.05);
    float scaleEnd(0.1);
    
    float rotateBegin = 0;
    float rotateEnd = M_PI/4;
    
    auto &elapsedTime = element.time.elapsedTime;
    auto &totalTime = element.time.totalTime;
    if (elapsedTime >= totalTime) {
        element.state = AnimationState::end;
        return;
    }
    
    auto progress = elapsedTime/totalTime;
    auto progress1 = spring(0.4, progress);
    glm::vec2 screenAspectRatio(1, 1.6); //FIXME: hardcoded
    element.scale2d = screenAspectRatio * lerp(scaleBegin, scaleEnd, progress1);
    
    auto progress2 = spring(0.8, progress);
    element.rotateByRadian = element.initialRotation + lerp(rotateBegin, rotateEnd, progress2);
    
    elapsedTime += dt;
}

//count until leaf elements also start
//NOTE: leaf delayStartTime must be less than center.time.totalTime!
//also delayedStart time is sorted increasing order
void MenuBase::triggerDelayedStart(float elapsedTime, GUICircleMenu::Handle circle)
{
    for (auto &leaf : circle->leafMenus)
    {
        if (AnimationState::invalid == leaf.state
            and elapsedTime > leaf.time.delayedStartTime) {
            leaf.state = AnimationState::start;
        }
    }
}

