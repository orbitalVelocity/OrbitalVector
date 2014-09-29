//
//  text.h
//  GLFW3_test
//
//  Created by Si Li on 9/25/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __GLFW3_test__text__
#define __GLFW3_test__text__

#include <iostream>
#include "includes.h"

using namespace std;

class Text {
public:
    Text() : x(0.0f), y(0.0f), fontSize(20)
    {}
    Text(glm::vec2, float, string);
public:
    float x, y; //[0,1]
    glm::vec2 pos;
    float fontSize;
    string text;
};
#endif /* defined(__GLFW3_test__text__) */
