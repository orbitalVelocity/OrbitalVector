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
    paths.resize(1);
    paths[0].reserve(360);
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


void Orbit::calcTrajectory(int &pathSteps)
{
    //FIXME for some reason sending copies of ks/sys to orbitDelta
    //does not work
    //must copy original and move it back after for now
    auto ks2 = ks;
    auto &sys2 = sys;
    
    auto sys3 = sys;
    
    
    float dt = 1;
    int j = 1; //for testing
    for (auto &p : paths)
        p.clear();
    paths.clear();
    paths.resize(sys.size()-1);//# of grav wells, maybe not even that!
    paths[0].reserve(pathSteps);
    int count = 0;
    for (auto &path : paths)
    {
        path.resize(3);
        path[0] = sys2[j+count].sn.pos.x;
        path[1] = sys2[j+count].sn.pos.y;
        path[2] = sys2[j+count].sn.pos.z;
        count++;
    }
    
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
        orbitDelta(dt, ks2, sys2, true);
        //check dist to parent
        distance = glm::length(sys2[j].sn.pos - sys2[0].sn.pos);
        if (last2Distance > lastDistance && lastDistance < distance)
        {
            periFound = true;
            peri      = distance;
            periPos   = sys2[j].sn.pos;
            
//            cout << " periapsis found! ";
        }
        if (last2Distance < lastDistance && lastDistance > distance
            && last2Distance != 0 && lastDistance != 0)
        {
            apoFound = true;
            apo      = distance;
            apoPos   = sys2[j].sn.pos;
//            cout << " apoapsis found! "
//                 << last2Distance
//                 << ", " << lastDistance;
        }
//        cout << "\tdistance: " << distance << "\n";
        
        if ((last2Distance == lastDistance || lastDistance == distance)
            && last2Distance != 0.0f && lastDistance != 0.0f)
        {
            cout << "same radius across 2 time points! gotta solve this\n";
        }
   
        last2Distance = lastDistance;
        lastDistance = distance;
        int offset = 1; //grav wells
        for (int k=0; k < sys2.size()-offset; k++) {
            paths[k].push_back(sys2[k+offset].sn.pos.x);
            paths[k].push_back(sys2[k+offset].sn.pos.y);
            paths[k].push_back(sys2[k+offset].sn.pos.z);
            if (!loopCond())
                continue;
            paths[k].push_back(sys2[k+offset].sn.pos.x);
            paths[k].push_back(sys2[k+offset].sn.pos.y);
            paths[k].push_back(sys2[k+offset].sn.pos.z);
        }
    }
    sys = std::move(sys3);
 
    pathSteps = paths[0].size();
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
    static int count = 0;
    int vecSize = 3;
    int pathSteps = 980;

    float  *pathGL;
    drawCount = pathSteps/vecSize;
    calcTrajectory(pathSteps);
    int pathSize = sizeof(float) * pathSteps;
    int totalPathSize = (int)(paths.size()*paths[0].size() * sizeof(float));

//    return; //FIXME so nextMesh is called from draw
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    check_gl_error();
    glBufferData(GL_ARRAY_BUFFER, totalPathSize, nullptr, GL_STREAM_DRAW);
    check_gl_error();
    pathGL = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    int pathOffset = 0;
    for (auto &path : paths)
    {
        memcpy(&pathGL[pathOffset], path.data(), pathSize);
        pathOffset += path.size();
    }
    drawCount = (int)(paths.size() * paths[0].size());
//    memcpy(pathGL, paths[0].data(), totalPathSize);)
    check_gl_error();
    glUnmapBuffer(GL_ARRAY_BUFFER);
    check_gl_error();
    count++;
}

bool Orbit::nextMesh()
{
    static int count = 0;
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    check_gl_error();
    auto pathSize = paths[count].size();
    glBufferData(GL_ARRAY_BUFFER, pathSize, nullptr, GL_STREAM_DRAW);
    check_gl_error();
    float *pathGL = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(pathGL, paths[count].data(), pathSize);
    check_gl_error();
    glUnmapBuffer(GL_ARRAY_BUFFER);
    check_gl_error();
    count = (count + 1) % paths.size() - 1; //gravwell offset
    
    return count != 0;
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
//    while (nextMesh()) {
//        glDrawArrays(drawType, 0, drawCount);
//    }
    check_gl_error();
    
}