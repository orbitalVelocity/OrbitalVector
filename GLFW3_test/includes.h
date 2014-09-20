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


vector<unsigned> normCount, normIndex;
#if 0
class triangle {
  public:
    triangle(unsigned a, unsigned b, unsigned c): idx{a, b, c} {}
    glm::vec3 &get(unsigned i) const { return glm::vec3[idx[i]]; }
    unsigned getIdx(unsigned i) const { return idx[i]; }

    GLuint *ptr() { return idx; }
  private:
    GLuint idx[3];
};

class Obj {
public:
    Obj(){}
public:
    vector<GLuint> triangles;
    vector<glm::vec3> vertices, normals;
    vector<unsigned> normCount, normIndex;
};

void readObj(string filename, Obj &obj);
#endif
#endif
