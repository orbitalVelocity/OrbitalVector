//
//  VertexArrayObject.h
//  OrbitalVector
//
//  Created by Si Li on 3/4/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_VertexArrayObject_h
#define OrbitalVector_VertexArrayObject_h

#include "includes.h"


class VertexArrayObject
{
public:
    VertexArrayObject()
    {
        glGenVertexArrays(1, &vao);
        vboIdx = 0;
    }
    
    void generateBuffer(GLuint bufferType)
    {
        vbo.resize(vbo.size()+1);           //vbo only used in this funciton

        //must bind VAO first, else VBO won't be linked to VAO
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo[vboIdx]);
        glBindBuffer(bufferType, vbo[vboIdx]);
        check_gl_error();
    }
    
    template<typename T>
    void setupBuffer(GLuint bufferType, GLuint drawHint, vector<T> &array)
    {
        //must bind VAO first, else VBO won't be linked to VAO
        glBindVertexArray(vao);
        generateBuffer(bufferType);
        glBufferData(bufferType,
                     array.size() * sizeof(T),
                     array.data(),
                     drawHint);
        check_gl_error();
        drawCount = (int)array.size();
    }
    
public:
    GLuint vao, elementBuffer;
    vector<GLuint> vbo;
    int vboIdx;
    GLenum drawType;
    int drawCount;
};

#endif
