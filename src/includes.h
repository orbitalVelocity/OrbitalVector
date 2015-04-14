//
//  includes.h
//  root header file for all basic functionality (and maybe more...)
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

//#define GLM_FORCE_RADIANS
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "GLError.h"

//orbital utils related
#include "vectord.h"
#include "orbitalelements.h"
#include "oeconvert.h"
#include "kepler.h"
#include "lambert.h"


void _debug_cout(std::string msg, const char *file, int line);

#define debug_cout(x) _debug_cout(x,__FILE__,__LINE__)

//FIXME: globals for prototyping ONLY
extern bool globalReload;
extern bool globalShowFXAAAAirection;
extern int renderStage;
extern const int stage1, stage2, stage3, stage4, stage5, stage6, stage7, stage8, stage9, stage10;
std::string get_file_contents(std::string filename);
std::string printVec3(glm::vec3 v);
glm::vec2 getVec2(glm::mat4 vp, glm::vec3 _pos);


//template<typename T>
//T lerp(T a, T b, float t);
template<typename T>
T lerp(T a, T b, float t)
{
    return (1-t)*a + t*b;
}
float spring(float factor, float progress);


//orbital stuff
VectorD convertToParams (glm::vec3 pos, double gm);
std::vector<double> toPosVelVector(glm::vec3 pos, glm::vec3 vel);

template<typename T>
T deg2rad(T degree);

template <class T>
class RingBuffer{
public:
    RingBuffer(int size)
    {
        data.resize(size, 0);
        index = 0;
        _average = _min = _max = 0;
    }
    void push(T v)
    {
        data[index++%data.size()] = v;
    }
    float average()
    {
        if (index % data.size() == 0) {
            _average = (float)std::accumulate(data.begin(), data.end(), 0.0f)/(float)data.size();
        }
        return _average;
    }
    float max()
    {
        if (index % data.size() == 0) {
            auto mm = std::minmax_element(data.begin(), data.end());
            _min = *mm.first;
            _max = *mm.second;
        }
        return _max;
    }
    float min()
    {
        if (index % data.size() == 0) {
            auto mm = std::minmax_element(data.begin(), data.end());
            _min = *mm.first;
            _max = *mm.second;
        }
        return _min;
    }
    
    int index = 0;
    float _average, _min, _max;
    std::vector<T> data;
};

#endif
