//
//  text.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/25/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "text.h"

using namespace std;

Text::Text(glm::vec2 _p, float size, string _s)
{
    pos = _p;
    fontSize = size;
    text = _s;
}