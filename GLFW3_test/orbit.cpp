//
//  orbit.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/16/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "orbit.h"
#include "ogl.h"
#include "rk547m.h"

using namespace std;


Orbit::Orbit(GLenum _drawType) : drawType(_drawType), x(0), y(90) {
}

void Orbit::init()
{
        loadPath();
}

void Orbit::newProgram(map<GLuint, string> &shaders)
{
    vector<GLuint> shaderIDs(shaders.size());
    
    // Create and compile the vertex shader
    int i = 0;
    for (auto &shader: shaders) {
        auto shaderType = shader.first;
        auto shaderSource = shader.second;
        const char* src = shaderSource.c_str();
        shaderIDs[i] = glCreateShader(shaderType);
        
        glShaderSource(shaderIDs[i], 1, &src, NULL);
        check_gl_error();
        glCompileShader(shaderIDs[i]);
        check_gl_error();
        i++;
    }
    
    // Link the vertex and fragment shader into a shader program
    shaderProgram = glCreateProgram();
    for (auto &shaderID : shaderIDs)
    {
        glAttachShader(shaderProgram, shaderID);
        check_gl_error();
    }
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    check_gl_error();
    glLinkProgram(shaderProgram);
    check_gl_error();
    glUseProgram(shaderProgram);
    check_gl_error();
    
    
    
}


void calcTrajectory(float *path, int pathSteps)
{
    //FIXME for some reason sending copies of ks/sys to orbitDelta
    //does not work
    //must copy original and move it back after for now
    auto ks2 = ks;
    auto &sys2 = sys;
    auto ks3 = ks;
    auto sys3 = sys;
    

    const int vecSize = 6;
    float dt = 1;
    int j = 1; //for testing
    path[0] = sys2[j].sn.pos.x;
    path[1] = sys2[j].sn.pos.y;
    path[2] = sys2[j].sn.pos.z;
    for(int i=0; i<pathSteps/vecSize; i++)
    {
        orbitDelta(dt, ks2, sys2, true);
        
        //for (int j=0; j < sys2.size(); j++)
        {
            //                int bodyIndex = j*pathSteps*vecSize;
            int pathIndex = i*vecSize + 3;
            path[pathIndex+0] = sys2[j].sn.pos.x;
            path[pathIndex+1] = sys2[j].sn.pos.y;
            path[pathIndex+2] = sys2[j].sn.pos.z;
            if (pathSteps <= pathIndex+3)
                break;
            path[pathIndex+3] = sys2[j].sn.pos.x;
            path[pathIndex+4] = sys2[j].sn.pos.y;
            path[pathIndex+5] = sys2[j].sn.pos.z;
            //printks(ks2);
            //printsys(sys2);
        }
    }
    sys = std::move(sys3);
    ks = std::move(ks3);
    
}

void Orbit::loadPath()
{
    //load shaders
    string vertFilename = "lineVertex.glsl";
    string fragFilename = "lineFragment.glsl";
    map<GLuint, string> shaders;
    
    auto vertexSource = get_file_contents(vertFilename);
    auto fragmentSource = get_file_contents(fragFilename);
    shaders.insert({GL_VERTEX_SHADER, vertexSource});
    shaders.insert({GL_FRAGMENT_SHADER, fragmentSource});
    newProgram(shaders);
    
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    check_gl_error();
    
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    check_gl_error();
    glEnableVertexAttribArray(posAttrib);
    check_gl_error();
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), NULL);
    check_gl_error();
    update();
    glBindVertexArray(0);
    
}

void Orbit::update()
{
    int vecSize = 3;
    int pathSteps = 360;
    
    float *path;
    drawCount = pathSteps/vecSize;
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    check_gl_error();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*pathSteps, nullptr, GL_STREAM_DRAW);
    check_gl_error();
    path = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    check_gl_error();
    calcTrajectory(path, pathSteps);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    check_gl_error();
}



void Orbit::draw(glm::mat4 &mvp, glm::vec3 color)
{
    GLint uColor = glGetUniformLocation(shaderProgram, "color");
    check_gl_error();
    glUniform3fv(uColor, 1, glm::value_ptr(color));
    check_gl_error();
    GLint uTransform = glGetUniformLocation(shaderProgram, "transform");
    //glm::mat4 mvp = camera * world;// * position * size * orientation;
    glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(mvp));
    check_gl_error();
    glBindVertexArray(vao);
    check_gl_error();
    glDrawArrays(drawType, 0, drawCount);
    check_gl_error();
    
}