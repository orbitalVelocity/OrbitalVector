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
    glm::vec3 initPos = sys2[0].sn.pos;

    int j = 1; //for testing
    
    vector<int> ids(sys2.size());       //ids index into paths
    iota(ids.begin(), ids.end(), -1);
    
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
        if (true)
        {
            //mark elements that needs to be removed
            //TODO: due to collision, too far/escape velocity
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
        }
   
        last2Distance = lastDistance;
        lastDistance = distance;
        for (auto i=1; i < sys2.size(); i++) {
            auto j = ids[i];
            
            int offset = 0;
            auto tmp = sys2[i+offset].sn.pos - sys2[0].sn.pos + initPos;
            paths[j].push_back(tmp.x);
            paths[j].push_back(tmp.y);
            paths[j].push_back(tmp.z);
            
            paths[j].push_back(tmp.x);
            paths[j].push_back(tmp.y);
            paths[j].push_back(tmp.z);
        }
    }
    
    //delete the last three floats from each path
    for (auto &path : paths) {
        path.erase(path.end()-1);
        path.erase(path.end()-1);
        path.erase(path.end()-1);
    }
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
    int pathSteps = 980;

    float  *pathGL;
    calcTrajectory(pathSteps);
    
    pathSteps = 0;
    for (auto &p : paths) {
        pathSteps += p.size();
    }
    int totalPathSize = (int)(pathSteps * sizeof(float));

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    check_gl_error();
    glBufferData(GL_ARRAY_BUFFER, totalPathSize, nullptr, GL_STREAM_DRAW);
    check_gl_error();
    pathGL = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    
    int pathOffset = 0;
    for (auto &path : paths)
    {
        memcpy(&pathGL[pathOffset], path.data(), sizeof(float)*path.size());
        pathOffset += path.size();
    }
    drawCount = (int)(pathSteps)/3;
 
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