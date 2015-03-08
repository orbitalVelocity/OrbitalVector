//
//  GLError.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/8/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "GLError.h"
#include <cassert>

#ifdef WIN32
#  include <GL/glew.h>
#elif __APPLE__
#  include <OpenGL/gl3.h>
#else
#  include <GL3/gl3.h>
#endif
#include <string>
#include <iostream>

void _check_gl_error(const char *file, int line) {
    GLenum err (glGetError());
    
    bool errFlag = false;
    while(err!=GL_NO_ERROR) {
        std::string error;
        
        switch(err) {
            case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
            case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
            case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
            case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
        }
        
        std::cerr << "GL_" << error.c_str() <<" - "<<file<<":"<<line<<std::endl;
        err=glGetError();
        errFlag = true;
    }
    if (errFlag) {
        assert(false);
    }
}