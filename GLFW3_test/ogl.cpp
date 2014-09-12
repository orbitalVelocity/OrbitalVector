//
//  ogl.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/7/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "ogl.h"
#include "rk547m.h"

using namespace std;

// Shader sources
const GLchar* vertexSource =
"#version 150 core\n"
"in vec3 position;"
//"out vec3 fragColor;"
"uniform mat4 transform;"
"void main() {"
/*
"   vec3 surfacePos = vec3(transform * vec4(position, 1.0));"
"    vec3 surfaceToLight = normalize(vec3(0.0, -10,10) - surfacePos);"
"    vec3 normal = normalize(transpose(inverse(mat3(transform))) * position);"
"    float diffuseCoefficient = max(0.0, dot(normal, -surfaceToLight));"
//*/
//"   fragColor = position;"//diffuseCoefficient * vec3(1.0,1.0,1.0);"
"   gl_Position = transform * vec4(position, 1.0);"
"}";
const GLchar* fragmentSource =
"#version 150 core\n"
//"in vec3 fragColor;"
"out vec4 outColor;"
"uniform vec3 color;"
"void main() {"
"   outColor = vec4(color, 1.0);"
"}";

OGL::OGL(GLenum _drawType) : drawType(_drawType), x(0), y(90) {
    init();
}

void OGL::init()
{
#if 1
    if (drawType == GL_TRIANGLES)
        loadIco();
    else if (drawType == GL_LINES)
        loadPath();
    else
        loadGrid();
#else
    // Create Vertex Array Object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // Create a Vertex Buffer Object and copy the vertex data to it
    GLfloat vertices[] = {
        0.0f, 0.5f,
        0.5f, -0.5f,
        -0.5f, -0.5f
    };
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    check_gl_error();
    
    map<GLuint, string> shaders;
    shaders.insert({GL_VERTEX_SHADER, vertexSource});
    shaders.insert({GL_FRAGMENT_SHADER, fragmentSource});
    newProgram(shaders);
#endif
}

void OGL::newProgram(map<GLuint, string> &shaders)
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

void _tesselate(int depth, GLfloat *tri0, GLfloat *tri1, GLfloat *tri2, vector<GLfloat> &buffer)
{
    GLfloat *tris[3] = {tri0, tri1, tri2};
    if (depth==0)
    {
        for (int i=0; i<3; ++i)
            for (int j=0; j<3; ++j)
                buffer.push_back(tris[i][j]);
    } else
    {
        GLfloat midPoint[3][3];
        //find midpoints, call _tesselate for each of 4 things
        for (int j=0; j<3; j++)
        {
            //calculate intermediate points
            //take 2 points, find middle point
            GLfloat *p1=tris[j];
            GLfloat *p2=tris[(j+1)%3];
            for (int k=0; k<3; k++) {
                midPoint[j][k] = (p1[k]+p2[k])/2;
            }
            
            //make it 1 unit from origin
            GLfloat curDist = sqrt(midPoint[j][0]*midPoint[j][0]+midPoint[j][1]*midPoint[j][1]+midPoint[j][2]*midPoint[j][2]);
            GLfloat scaleFactor = 1 / curDist;
            
            for(int k=0; k<3; k++)
            {
                midPoint[j][k] *= scaleFactor;
            }
        }
        //render the 4 triangles
        _tesselate(depth-1, tri0, midPoint[0], midPoint[2], buffer);
        _tesselate(depth-1, midPoint[0], tri1, midPoint[1], buffer);
        _tesselate(depth-1, midPoint[1], tri2, midPoint[2], buffer);
        _tesselate(depth-1, midPoint[0], midPoint[1], midPoint[2], buffer);
        
    }
}

void OGL::loadIco() {
    //load shaders
    string vertFilename = "planetvs.txt";
    string fragFilename = "planetfs.txt";
    map<GLuint, string> shaders;
    shaders.insert({GL_VERTEX_SHADER, vertexSource});
    shaders.insert({GL_FRAGMENT_SHADER, fragmentSource});
    newProgram(shaders);
    
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    // Make a sphere
    vector<GLfloat> vertexData;
    GLfloat X = .525731112119133606;
    GLfloat Z = .850650808352039932;
    static GLfloat vdata[12][3] = {
        {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
        {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
        {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
    };
    
    static GLint tindices[20][3] = {
        {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
        {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
        {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
        {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

    int depth = 1;
    for(auto i=0; i<20; ++i)    //each face
    {
        //call tesselate with 3 vertices
        _tesselate(depth,
                   vdata[tindices[i][0]],
                   vdata[tindices[i][1]],
                   vdata[tindices[i][2]],
                   vertexData
                   );
    }
    glBufferData(GL_ARRAY_BUFFER, vertexData.size()*sizeof(float), vertexData.data(), GL_STATIC_DRAW);
    
    drawCount = (int)(vertexData.size()/3);
#if 0
    
    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    check_gl_error();
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    check_gl_error();
#endif
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    check_gl_error();
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), NULL);
    check_gl_error();
    
    glBindVertexArray(0);
    
}
string printVec3(glm::vec3 v)
    {
        stringstream out;
        out << v.x << ", " << v.y << ", " << v.z;
        return out.str();
    }
void printks(vector<vector<state>> &ks)
{
    
    for (auto &k : ks)
        for (auto &s : k)
        {
            cout << printVec3(s.pos) << " v: " << printVec3(s.vel) << "\n";
        }
}

void printsys(vector<body> &sys)
{
    for (auto &body : sys)
    {
        cout << "p: " << printVec3(body.sn.pos) << " v: " << printVec3(body.sn.vel) << "\n";
    }
}
#define MAPBUFFER 1
#if MAPBUFFER
void calcTrajectory(float *path, int pathSteps)
{
    //FIXME for some reason sending copies of ks/sys to orbitDelta
    //does not work
    //must copy original and move it back after for now
    auto &ks2 = ks;
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
#else
void calcTrajectory(vector<float> &path)
{
    //FIXME for some reason sending copies of ks/sys to orbitDelta
    //does not work
    //must copy original and move it back after for now
    auto &ks2 = ks;
    auto &sys2 = sys;
    auto ks3 = ks;
    auto sys3 = sys;
    
    const int vecSize = 6;
    float dt = 1;
    int j = 1; //for testing
    path.at(0) = sys2[j].sn.pos.x;
    path.at(1) = sys2[j].sn.pos.y;
    path.at(2) = sys2[j].sn.pos.z;
    for(int i=0; i<path.size()/vecSize; i++)
    {
        orbitDelta(dt, ks2, sys2, true);
        
        //for (int j=0; j < sys2.size(); j++)
        {
            //                int bodyIndex = j*pathSteps*vecSize;
            int pathIndex = i*vecSize + 3;
            path.at(pathIndex+0) = sys2[j].sn.pos.x;
            path.at(pathIndex+1) = sys2[j].sn.pos.y;
            path.at(pathIndex+2) = sys2[j].sn.pos.z;
            if (path.size() <= pathIndex+3)
                break;
            path.at(pathIndex+3) = sys2[j].sn.pos.x;
            path.at(pathIndex+4) = sys2[j].sn.pos.y;
            path.at(pathIndex+5) = sys2[j].sn.pos.z;
            //printks(ks2);
            //printsys(sys2);
        }
    }
    for (int i=0; i<3; i++) {
        path.pop_back();
    }
    sys = std::move(sys3);
    ks = std::move(ks3);

}
#endif

void OGL::loadGrid()
{
    //load shaders
    string vertFilename = "planetvs.txt";
    string fragFilename = "planetfs.txt";
    map<GLuint, string> shaders;
    shaders.insert({GL_VERTEX_SHADER, vertexSource});
    shaders.insert({GL_FRAGMENT_SHADER, fragmentSource});
    newProgram(shaders);
    
    std::vector<float> path;
    int vecSize = 3;
    
    float gridSize = 1024;
    for (int i=0; i < gridSize; i+=16) {
        path.push_back(i-gridSize/2);
        path.push_back(gridSize/2);
        path.push_back(0.0);
        path.push_back(i-gridSize/2);
        path.push_back(-gridSize/2);
        path.push_back(0.0);
    }
    for (int i=0; i < gridSize; i+=16) {
        path.push_back(gridSize/2);
        path.push_back(i-gridSize/2);
        path.push_back(0.0);
        path.push_back(-gridSize/2);
        path.push_back(i-gridSize/2);
        path.push_back(0.0);
    }
    
    drawType = GL_LINES;
    drawCount = (int)(path.size()/vecSize);
    
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
        check_gl_error();
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*path.size(), path.data(), GL_DYNAMIC_DRAW);
        check_gl_error();
    
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
        check_gl_error();
    glEnableVertexAttribArray(posAttrib);
        check_gl_error();
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), NULL);
        check_gl_error();
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
}
void OGL::loadPath()
{
    //load shaders
    string vertFilename = "planetvs.txt";
    string fragFilename = "planetfs.txt";
    map<GLuint, string> shaders;
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
    
}
void OGL::update()
{
    int vecSize = 3;
    int pathSteps = 360;
#if MAPBUFFER
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
 
#else
    std::vector<float> path;
    path.resize(pathSteps);
    
    calcTrajectory(path);
   
    drawCount = (int)(path.size()/vecSize);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
        check_gl_error();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*path.size(), nullptr, GL_DYNAMIC_DRAW);
        check_gl_error();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*path.size(), path.data(), GL_DYNAMIC_DRAW);
        check_gl_error();
#endif
    
}

void OGL::scale(const glm::vec3 s)
{
    size = glm::scale(glm::mat4(), s);
}
void OGL::move(glm::vec3 m)
{
    position = glm::translate(glm::mat4(), m);
}
void OGL::update(float dx, float dy)
{
    y += dy;
    x += dx;
    x = (x > 360) ? x - 360 : x;
    y = (y > 180) ? 180 : y;
    y = (y < 0) ? 0 : y;
#if TRANSFORM
        //transform triangle
        //set uniform
        GLint uView = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(view));
        GLint uProj = glGetUniformLocation(shaderProgram, "proj");
        glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj));
#endif
    orientation = glm::rotate(glm::mat4(), -y, glm::vec3(1.0f, 0.0f, 0.0f));
    orientation = glm::rotate(orientation, -x, glm::vec3(0.0f, 1.0f, 0.0f));
}

void OGL::draw(glm::mat4 &camera, glm::vec3 color)
{
    GLint uColor = glGetUniformLocation(shaderProgram, "color");
    glUniform3fv(uColor, 1, glm::value_ptr(color));
    GLint uTransform = glGetUniformLocation(shaderProgram, "transform");
    glm::mat4 mvp = camera * position * size * orientation;
    glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(mvp));
    glBindVertexArray(vao);
    glDrawArrays(drawType, 0, drawCount);
 
}