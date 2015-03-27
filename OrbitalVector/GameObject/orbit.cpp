//
//  orbit.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/16/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "orbit.h"
#include "OGLShader.h"
#include "rk547m.h"
#include "ecs.h"
#include "componentTypes.h"

using namespace std;


RenderableOrbit::RenderableOrbit(GLenum _drawType) : OGLShader(_drawType) {
}

void RenderableOrbit::init()
{
    //load shaders
    string vertFilename = "lineVertex.glsl";
    string fragFilename = "lineFragment.glsl";
    loadShaders(vertFilename, fragFilename);
    
    generateVertexBuffer(GL_ARRAY_BUFFER);
    
    setAttribute("position");
    
    //unbinds vao, prevents subsequent GL calls from modifying this object
    glBindVertexArray(0);
    //    vboIdx++; //FIXME: doesn't work w/ multiple vbo's!
}



void RenderableOrbit::update()
{
    float  *pathGL;
    using namespace entityx;
    int pathSteps2 = 0;
    ComponentHandle<OrbitPath> orbit;
    for (Entity entity : myGameSingleton.entities.entities_with_components(orbit))
    {
        pathSteps2 += (int) orbit->path.size();
    }
    int totalPathSize2 = (int)(pathSteps2 * sizeof(float));
    
//    std::cout << "path steps: " << pathSteps2 << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIdx]);
    check_gl_error();
    glBufferData(GL_ARRAY_BUFFER, totalPathSize2, nullptr, GL_STREAM_DRAW);
    check_gl_error();
    pathGL = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    check_gl_error();
       
    int pathOffset = 0;
    for (Entity entity : myGameSingleton.entities.entities_with_components(orbit))
    {
        auto &path = orbit->path;
        memcpy(&pathGL[pathOffset], path.data(), sizeof(float)*path.size());
        pathOffset += path.size();
    }
    
    drawCount = (int)(pathSteps2)/3;
 
    check_gl_error();
    glUnmapBuffer(GL_ARRAY_BUFFER);
    check_gl_error();
}



void RenderableOrbit::draw(glm::mat4 &mvp, glm::vec3 color)
{
    GLint uColor = glGetUniformLocation(shaderProgram, "color");
    check_gl_error();
    glUniform3fv(uColor, 1, glm::value_ptr(color));
    check_gl_error();
    GLint uTransform = glGetUniformLocation(shaderProgram, "model");
    //glm::mat4 mvp = camera * world;// * position * size * orientation;
    glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(mvp));
    check_gl_error();
    glBindVertexArray(vao);
    check_gl_error();
    glDrawArrays(drawType, 0, drawCount);

    check_gl_error();
    
}


/*
//legacy function for finding apoapsis/periapsis of orbit paths

//calculate future positions in while loop
//find max/min distances at each iteration of loop
auto origin = sys2[j].sn.pos;
bool apoFound = false;
bool periFound = false;
float lastDistance = 0;
float last2Distance = 0;
float distance = 100;

auto loopCond = [&]() {
    return (!apoFound or !periFound) and paths[0].size() < 1000;
};

while (loopCond())
{
    orbitPhysicsUpdate(dt, ks2, sys2, true);
    
    //check if apoapsis and periapsis has been reached
    distance = glm::length(sys2[j].sn.pos - sys2[0].sn.pos);
    if ((last2Distance > lastDistance && lastDistance < distance)
        || distance < 15.0)
    {
        periFound = true;
        peri      = distance;
        periPos   = sys2[j].sn.pos - sys2[0].sn.pos + initPos;
    }
    if ((last2Distance < lastDistance && lastDistance > distance
         && last2Distance != 0 && lastDistance != 0)
        || distance > 400)
    {
        apoFound = true;
        apo      = distance;
        apoPos   = sys2[j].sn.pos - sys2[0].sn.pos + initPos;
    }
    
    if ((last2Distance == lastDistance || lastDistance == distance)
        && last2Distance != 0.0f && lastDistance != 0.0f)
    {
        cout << "same radius across 2 time points! gotta solve this\n";
        break;
        }
        
        //collision detection
        vector<bool> markedForRemoval(sys2.size(), false);
        markForDeletion(sys2, markedForRemoval);
        
        //remove all marked elements
        //FIXME: only works because first element never designed to be removed
        auto it = --sys2.end();
        auto itt = --ids.end();
        for (int i = (int)sys2.size()-1; i >= 0; --i, --it, --itt)
        {
            if (markedForRemoval[i]) {      //TODO: wrap sys2 and ids into 1 object
                sys2.erase(it);
                ids.erase(itt);
            }
        }
        
        last2Distance = lastDistance;
        lastDistance = distance;
        
        }
        }
*/
