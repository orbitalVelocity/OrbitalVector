//
//  ogl.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/7/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "ogl.h"

// Shader sources
const GLchar* vertexSource =
"#version 150 core\n"
"in vec2 position;"
"void main() {"
"   gl_Position = vec4(position, 0.0, 1.0);"
"}";
const GLchar* fragmentSource =
"#version 150 core\n"
"out vec4 outColor;"
"void main() {"
"   outColor = vec4(1.0, 1.0, 1.0, 1.0);"
"}";

OGL::OGL() {
    
    //transform = glm::mat4();
    // Create Vertex Array Object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &vbo);
    
    GLfloat vertices[] = {
        0.0f, 0.5f,
        0.5f, -0.5f,
        -0.5f, -0.5f
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Create and compile the vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    
    // Create and compile the fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    
    // Link the vertex and fragment shader into a shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    
    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    

}