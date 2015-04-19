//
//  orbit.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/16/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "orbit.h"
#include "OGLShader.h"
#include "ecs.h"
#include "componentTypes.h"

using namespace std;


RenderableOrbit::RenderableOrbit(GLenum _drawType) : OGLShader(_drawType) {
}

void RenderableOrbit::init()
{
    string vertFilename = "lineVertex.glsl";
    string fragFilename = "lineFragment.glsl";
    loadShaders(vertFilename, fragFilename);
}



void RenderableOrbit::update(entityx::EntityManager &entities)
{
    float  *pathGL;
    using namespace entityx;
    int pathSteps = 0;
    OrbitPath::Handle orbit;
    
    auto count = 0;
    for (Entity entity : entities.entities_with_components(orbit))
    {
        auto &path = orbit->path;
        if (path.empty())
        {
            std::cout << "empty orbit path\n";
            continue;
        }
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