//
//  includes.h
//  GLFW3_test
//
//  Created by Si Li on 9/7/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef GLFW3_test_includes_h
#define GLFW3_test_includes_h

#include <iostream>
#include <sstream>
#include <string>
#include <numeric>
#include <vector>
#include <chrono>
#include <cassert>
#include <iomanip>
#include <fstream>
#include <map>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "GLError.h"

#include "rk547m.h"
#define TRANSFORM 0
using namespace std;

string get_file_contents(string filename);
string printVec3(glm::vec3 v);

class Obj {
public:
    Obj(){}
public:
    vector<glm::vec3> triangles, vertices, normals;
    vector<unsigned> normCount, normIndex;
};

void readObj(char *filename, Obj &obj);
#endif
