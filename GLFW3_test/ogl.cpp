//
//  ogl.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/7/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "ogl.h"

using namespace std;

// Shader sources
const GLchar* vertexSource =
"#version 150 core\n"
"in vec3 position;"
"out vec3 fragColor;"
"uniform mat4 transform;"
#if TRANSFORM
"uniform mat4 view;"
"uniform mat4 proj;"
"void main() {"
"   gl_Position = proj * view transform * vec4(position, 0.0, 1.0);"
#else
"void main() {"
/*
"   vec3 surfacePos = vec3(transform * vec4(position, 1.0));"
"    vec3 surfaceToLight = normalize(vec3(0.0, -10,10) - surfacePos);"
"    vec3 normal = normalize(transpose(inverse(mat3(transform))) * position);"
"    float diffuseCoefficient = max(0.0, dot(normal, -surfaceToLight));"
 */
"   fragColor = position;"//diffuseCoefficient * vec3(1.0,1.0,1.0);"
"   gl_Position = transform * vec4(position, 1.0);"
#endif
"}";
const GLchar* fragmentSource =
"#version 150 core\n"
"in vec3 fragColor;"
"out vec4 outColor;"
//"uniform mat4 transform;"
"void main() {"
"   outColor = vec4(fragColor, 1.0);"
"}";

OGL::OGL(glm::mat4 v, glm::mat4 p) : x(0), y(90) {
    init();
}

void OGL::init()
{
#if 1
    loadIco();
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

    int depth = 0;
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
void OGL::draw(glm::mat4 &camera)
{
    GLint uTransform = glGetUniformLocation(shaderProgram, "transform");
    glm::mat4 mvp = camera * position * orientation;
    glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(mvp));
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, drawCount);
 
}