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
    
    generateVertexBuffer(vao, GL_ARRAY_BUFFER);
    
    //    setAttribute("position");
    
    //unbinds vao, prevents subsequent GL calls from modifying this object
    glBindVertexArray(0);
    //    vboIdx++; //FIXME: doesn't work w/ multiple vbo's!
}



void RenderableOrbit::update(entityx::EntityManager &entities)
{
    float  *pathGL;
    using namespace entityx;
    int pathSteps = 0;
    OrbitPath::Handle orbit;
    
#if 1
    auto count = 0;
    for (Entity entity : entities.entities_with_components(orbit))
    {
        auto &path = orbit->path;
        glBindVertexArray(orbit->vao);
        vao = orbit->vao;
        glBindBuffer(GL_ARRAY_BUFFER, orbit->vbo);
        setAttribute("position");
        check_gl_error();

        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*path.size(), nullptr, GL_STREAM_DRAW);
        check_gl_error();
       
        pathGL = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        check_gl_error();
        
        memcpy(pathGL, path.data(), sizeof(float)*path.size());
        transform = orbit->transform;
        
        drawCount = (int)(path.size())/3;
        
        check_gl_error();
        glUnmapBuffer(GL_ARRAY_BUFFER);
        check_gl_error();
        count++;
    }
    
#else    
    for (Entity entity : entities.entities_with_components(orbit))
    {
        pathSteps += (int) orbit->path.size();
    }
    int totalPathSize = (int)(pathSteps * sizeof(float));
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[vboIdx]);
    setAttribute("position");
    check_gl_error();
    glBufferData(GL_ARRAY_BUFFER, totalPathSize, nullptr, GL_STREAM_DRAW);
    check_gl_error();
    pathGL = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    check_gl_error();
    
    int pathOffset = 0;
    for (Entity entity : entities.entities_with_components(orbit))
    {
        auto &path = orbit->path;
        memcpy(&pathGL[pathOffset], path.data(), sizeof(float)*path.size());
        pathOffset += path.size();
        transform = orbit->transform;
    }
    
    drawCount = (int)(pathSteps)/3;
    
    check_gl_error();
    glUnmapBuffer(GL_ARRAY_BUFFER);
    check_gl_error();
#endif
}


void RenderableOrbit::draw(glm::mat4 &mvp, glm::vec3 color)
{
    auto newTransform = mvp * transform;
    draw(vao, drawCount, newTransform, color);
}

void RenderableOrbit::draw(GLuint vao, int drawCount, glm::mat4 &mvp, glm::vec3 color)
{
    auto newTransform = mvp;
    GLint uColor = glGetUniformLocation(shaderProgram, "color");
    check_gl_error();
    glUniform3fv(uColor, 1, glm::value_ptr(color));
    check_gl_error();
    GLint uTransform = glGetUniformLocation(shaderProgram, "model");
    //glm::mat4 mvp = camera * world;// * position * size * orientation;
    glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(newTransform));
    check_gl_error();
    glBindVertexArray(vao);
    check_gl_error();
    glDrawArrays(drawType, 0, drawCount);
    
    check_gl_error();
    
}