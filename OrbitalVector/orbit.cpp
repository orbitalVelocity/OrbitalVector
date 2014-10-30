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
    auto &ks2 = ks;
    auto sys2 = sys;
    
    
    float dt = 2.0;
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
    
    int offset = 1; //grav wells
    int numTrajectories = sys2.size() - offset;
    vector<bool> objectCrashed;
    objectCrashed.resize(numTrajectories, false);
    
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
        }
        if (last2Distance < lastDistance && lastDistance > distance
            && last2Distance != 0 && lastDistance != 0)
        {
            apoFound = true;
            apo      = distance;
            apoPos   = sys2[j].sn.pos;
        }
        
        if ((last2Distance == lastDistance || lastDistance == distance)
            && last2Distance != 0.0f && lastDistance != 0.0f)
        {
            cout << "same radius across 2 time points! gotta solve this\n";
        }
        
        //collision detection
        {
            vector<body> sys2_copy;
#if 1
            vector<bool> removeElem(sys2.size(), false);
            for (int i=0; i < sys2.size(); i++)
            {
                for (int j = 0; j < sys2.size(); j++)
                {
                    if (i == j) {
                        continue;
                    }
                    if (glm::length(sys2[i].sn.pos - sys2[j].sn.pos) <= 10) {
                        removeElem[j] = true;
                        objectCrashed[j] = true;
                    }
                }
            }
#else
            auto matchingBody = [&](const body &b)
            {
                auto match = [&](const body &bb)
                {
                    return glm::length(b.sn.pos - bb.sn.pos) > 10.0;
                };
                return std::all_of(sys2.begin(), sys2.end(), match);
            };
            vector<body> sys2_copy;
            std::copy_if(sys2.begin(), sys2.end(), sys2_copy.begin(), matchingBody);
#endif
            
            sys2.clear();
            sys2 = std::move(sys2_copy);
            for (auto &k : ks2)
            {
                k.resize(sys2.size());
            }
        }
   
        last2Distance = lastDistance;
        lastDistance = distance;
        for (int k=0; k < numTrajectories; k++) {
            //FIXME could be optimized
            
            if (false){//objectCrashed[k]) {
                paths[k].push_back(0.0f);
                paths[k].push_back(0.0f);
                paths[k].push_back(0.0f);
                if (!loopCond()) {
                    continue;
                }
                paths[k].push_back(0.0f);
                paths[k].push_back(0.0f);
                paths[k].push_back(0.0f);
            } else {
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
    }
//    sys = std::move(sys3);
 
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

    check_gl_error();
    
}