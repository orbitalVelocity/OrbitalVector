//
//  GLError.h
//  GLFW3_test
//
//  Created by Si Li on 9/8/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef GLERROR_H
#define GLERROR_H

void _check_gl_error(const char *file, int line);

///
/// Usage
/// [... some opengl calls]
/// glCheckError();
///
#define check_gl_error()// _check_gl_error(__FILE__,__LINE__)

#endif // GLERROR_H
