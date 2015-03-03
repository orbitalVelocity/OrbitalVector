//
//  renderableType.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/7/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "renderableType.h"

using namespace std;


RenderableType::RenderableType(GLenum _drawType) : drawType(_drawType),
                             vboIdx(0),
                             depthTexture(false)
{
    glGenVertexArrays(1, &vao);
}

RenderableType::~RenderableType()
{
    glDeleteProgram(shaderProgram);
    for (auto &shaderID : shaderIDs)
        glDeleteShader(shaderID);
    
    for (auto &v : vbo)
        glDeleteBuffers(1, &v);
    glDeleteBuffers(1, &elementBuffer);
    
    glDeleteVertexArrays(1, &vao);
}



void RenderableType::newProgram(map<GLuint, string> &shaders, bool useProg)
{
    GLint Result = GL_FALSE;
    int InfoLogLength;
    auto programInfo = [&](GLuint infoType)
    {
        // Check the program
        glGetProgramiv(shaderProgram, infoType, &Result);
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);
        
        if ( InfoLogLength > 0 ){
            std::vector<char> ProgramErrorMessage(InfoLogLength+1);
            glGetProgramInfoLog(shaderProgram, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            printf("%s\n", &ProgramErrorMessage[0]);
        }
    };
    
    auto shaderInfo = [&](GLuint shaderID, GLuint infoType)
    {
        glGetShaderiv(shaderID, infoType, &Result);
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
            std::vector<char> ShaderErrorMessage(InfoLogLength+1);
            glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &ShaderErrorMessage[0]);
            printf("%s\n", &ShaderErrorMessage[0]);
        }
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    // Compile shaders, attach shaders, create programs, link program
    ////////////////////////////////////////////////////////////////////////////////
    shaderIDs.resize(shaders.size());
    
    // Create and compile the vertex shader
    int i = 0;
    for (auto &shader: shaders)
    {
        shaderIDs[i] = glCreateShader(shader.first);
        const char* src = shader.second.c_str();
        
        glShaderSource(shaderIDs[i], 1, &src, NULL);
        glCompileShader(shaderIDs[i]);
        
        shaderInfo(shaderIDs[i], GL_COMPILE_STATUS);
        i++;
    }
    
    // Link the vertex and fragment shader into a shader program
    shaderProgram = glCreateProgram();
    __glewProgramParameteri(shaderProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);
    for (auto &shaderID : shaderIDs)
    {
        glAttachShader(shaderProgram, shaderID);
        check_gl_error();
    }
    if (not depthTexture) {
        glBindFragDataLocation(shaderProgram, 0, "outColor");
    }
    check_gl_error();
    glLinkProgram(shaderProgram);
    programInfo(GL_LINK_STATUS);
//    glValidateProgram(shaderProgram);
//    programInfo(GL_VALIDATE_STATUS);
    
    // Clean up
    for (auto &shaderID : shaderIDs)
    {
        glDetachShader(shaderProgram, shaderID);
        check_gl_error();
    }
    
    if (useProg) {
        glUseProgram(shaderProgram);
        check_gl_error();
    }

    cout << "compiled a new shader!\n";
}

void RenderableType::loadShaders(string vs, string fs, bool useProg)
{
    map<GLuint, string> shaders;
    auto vShader = get_file_contents(vs);
    auto fShader= get_file_contents(fs);
    
    shaders.insert({GL_VERTEX_SHADER, vShader});
    shaders.insert({GL_FRAGMENT_SHADER, fShader});
    newProgram(shaders, useProg);
}


//single data type per attrib
void RenderableType::loadAttrib(string attribName, vector<float> &path, GLuint drawHint, GLuint bufferType)
{
    //transfer position data
    vbo.resize(vboIdx+1);           //vbo only used in this funciton
    glGenBuffers(1, &vbo[vboIdx]);
    
    glBindVertexArray(vao);
    glBindBuffer(bufferType, vbo[vboIdx]);
        check_gl_error();
    
    glBufferData(bufferType, sizeof(float)*path.size(), path.data(), drawHint);
        check_gl_error();
    
    //set position attribute
    GLint posAttrib = glGetAttribLocation(shaderProgram, attribName.c_str());
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), NULL);
        check_gl_error();
    
    glBindVertexArray(0);
    vboIdx++;
}


void RenderableType::update()
{
//call back function?
}


void RenderableType::drawIndexed(glm::mat4 &world, Camera &_camera, glm::vec3 lightPos, glm::mat4 &model,
                      glm::vec3 color, GLuint *indices)
{
    GLint uniformID;
    uniformID = glGetUniformLocation(shaderProgram, "color");
    glUniform3fv(uniformID, 1, glm::value_ptr(color));
    check_gl_error();
    
    uniformID = glGetUniformLocation(shaderProgram, "cameraPos");
    glUniform3fv(uniformID, 1, glm::value_ptr(_camera.getPosition()));
    check_gl_error();
    
    uniformID = glGetUniformLocation(shaderProgram, "lightPos");
    glUniform3fv(uniformID, 1, glm::value_ptr(lightPos));
    check_gl_error();
    
    drawIndexed(world, _camera, model, indices);
    
}

void RenderableType::drawIndexed(glm::mat4 &world, Camera &_camera, glm::mat4 &model, GLuint *indices)
{
    auto uniformID = glGetUniformLocation(shaderProgram, "camera");
    glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(_camera.matrix()));
    check_gl_error();
    
    uniformID = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(model));
    
    uniformID = glGetUniformLocation(shaderProgram, "world");
    glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(world));
    
    glBindVertexArray(vao);
    check_gl_error();
    
    glDrawElements(drawType, drawCount, GL_UNSIGNED_INT, (void*)0);
    check_gl_error();
    
}

void RenderableType::drawIndexed(glm::mat4 &model, glm::vec3 &color, GLuint *indices)
{
    auto uniformID = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(model));

    GLint uColor = glGetUniformLocation(shaderProgram, "color");
    check_gl_error();
    glUniform3fv(uColor, 1, glm::value_ptr(color));
    check_gl_error();
    
    glBindVertexArray(vao);
    check_gl_error();
    
    glDrawElements(drawType, drawCount, GL_UNSIGNED_INT, (void*)0);
    check_gl_error();
    
}

void RenderableType::draw(glm::mat4 &mvp)
{
    
    GLint uTransform = glGetUniformLocation(shaderProgram, "model");
    //    glm::mat4 mvp = camera * world * position * size * orientation;
    glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(mvp));
    check_gl_error();
    glBindVertexArray(vao);
    check_gl_error();
    glDrawArrays(drawType, 0, drawCount);
    check_gl_error();
    
}
void RenderableType::draw(glm::mat4 &mvp, glm::vec3 color)
{
    GLint uColor = glGetUniformLocation(shaderProgram, "color");
    check_gl_error();
    glUniform3fv(uColor, 1, glm::value_ptr(color));
    check_gl_error();
    
    draw(mvp);
}