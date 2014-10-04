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


OGL::OGL(GLenum _drawType) : drawType(_drawType), vboIdx(0) {
    glGenVertexArrays(1, &vao);
}

void OGL::init()
{
    if (drawType == GL_TRIANGLES)
        loadIco();
    else
        loadGrid();
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

void OGL::loadShaders(string vs, string fs)
{
    map<GLuint, string> shaders;
    auto vShader = get_file_contents(vs);
    auto fragmentSource = get_file_contents(fs);
    shaders.insert({GL_VERTEX_SHADER, vShader});
    shaders.insert({GL_FRAGMENT_SHADER, fragmentSource});
    newProgram(shaders);
}

void OGL::loadIco() {
    //load shaders
    string vertFilename = "planetVertex.glsl";
    string fragFilename = "planetFragment.glsl";
    loadShaders(vertFilename, fragFilename);
    
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

    int depth = 2;
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
    
    //setup GL states
    drawCount = (int)(vertexData.size()/3);
    drawType = GL_TRIANGLES;
    loadAttrib("position", vertexData, GL_STATIC_DRAW);
}


void OGL::loadGrid()
{
    //load shaders
    string vertFilename = "lineVertex.glsl";
    string fragFilename = "lineFragment.glsl";
    loadShaders(vertFilename, fragFilename);
 
    
    //generate positions
    std::vector<float> path;
    int vecSize = 3;
    
    float gridSize = 1024;
    for (int i=0; i < gridSize; i+=16) {
        path.push_back(i-gridSize/2);
        path.push_back(0.0);
        path.push_back(gridSize/2);
        path.push_back(i-gridSize/2);
        path.push_back(0.0);
        path.push_back(-gridSize/2);
    }
    for (int i=0; i < gridSize; i+=16) {
        path.push_back(gridSize/2);
        path.push_back(0.0);
        path.push_back(i-gridSize/2);
        path.push_back(-gridSize/2);
        path.push_back(0.0);
        path.push_back(i-gridSize/2);
    }
    
    drawType = GL_LINES;
    drawCount = (int)(path.size()/vecSize);
    loadAttrib("position", path, GL_DYNAMIC_DRAW);
}

//single data type per attrib
void OGL::loadAttrib(string attribName, vector<float> &path, GLuint drawHint, GLuint bufferType)
{
    //transfer position data
    vbo.resize(vboIdx+1);
    glGenBuffers(1, &vbo[vboIdx]);
    
    glBindVertexArray(vao);
    glBindBuffer(bufferType, vbo[vboIdx]);
        check_gl_error();
    
    glBufferData(bufferType, sizeof(float)*path.size(), path.data(), drawHint);
        check_gl_error();
    
    //set position attribute
    GLint posAttrib = glGetAttribLocation(shaderProgram, attribName.c_str());
        check_gl_error();
    glEnableVertexAttribArray(posAttrib);
        check_gl_error();
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), NULL);
        check_gl_error();
    
//    glBindBuffer(bufferType, 0);
    glBindVertexArray(0);
    vboIdx++;
}


void OGL::update()
{
//call back function?
}



void OGL::drawIndexed(Camera &_camera, glm::vec3 lightPos, glm::mat4 &model,
                      glm::vec3 color, GLuint *indices)
{
    GLint uniformID;
    uniformID = glGetUniformLocation(shaderProgram, "color");
    glUniform3fv(uniformID, 1, glm::value_ptr(color));
    check_gl_error();
    
    uniformID = glGetUniformLocation(shaderProgram, "cameraPos");
    glUniform3fv(uniformID, 1, glm::value_ptr(_camera.position));
    check_gl_error();
    
    uniformID = glGetUniformLocation(shaderProgram, "lightPos");
    glUniform3fv(uniformID, 1, glm::value_ptr(lightPos));
    check_gl_error();
    
    uniformID = glGetUniformLocation(shaderProgram, "camera");
    glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(_camera.matrix()));
    check_gl_error();
    
    uniformID = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(vao);
    check_gl_error();
    
    glDrawElements(drawType, drawCount, GL_UNSIGNED_INT, (void*)0);
    check_gl_error();
    
}

void OGL::draw(glm::mat4 &mvp, glm::vec3 color)
{
    GLint uColor = glGetUniformLocation(shaderProgram, "color");
        check_gl_error();
    glUniform3fv(uColor, 1, glm::value_ptr(color));
        check_gl_error();
    GLint uTransform = glGetUniformLocation(shaderProgram, "transform");
//    glm::mat4 mvp = camera * world * position * size * orientation;
    glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(mvp));
        check_gl_error();
    glBindVertexArray(vao);
        check_gl_error();
    glDrawArrays(drawType, 0, drawCount);
        check_gl_error();
 
}