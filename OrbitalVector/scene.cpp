//
//  scene.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/14/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "scene.h"
#include "glm/gtx/closest_point.hpp"

using namespace glm;

std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;

static bool
TestLoadObj(
            const char* filename,
            const char* basepath = NULL)
{
    std::cout << "Loading " << filename << std::endl;
    
    std::string err = tinyobj::LoadObj(shapes, materials, filename, basepath);
    
    if (!err.empty()) {
        std::cerr << err << std::endl;
        return false;
    }
    vec3 com;
    auto &pos = shapes[0].mesh.positions;
    for (int i=0; i< pos.size(); i+=3)
    {
        com += vec3(pos[i+0],
                         pos[i+1],
                         pos[i+2]);
    }
    com /= pos.size();
    for (int i=0; i< pos.size(); i+=3)
    {
        pos[i+0] -= com.x;
        pos[i+1] -= 3*com.y;
        pos[i+2] -= com.z;
    }
    cout << "center of mass in local coord is " << printVec3(com);
    //PrintInfo(shapes, materials);
    return true;
}

void initCamera(Camera & camera, int width, int height) {
    camera.setPosition(vec3(0, 0, 10.0f));
    camera.setFocus(vec3(0,3.0f,0));
    camera.setClip(0.01f, 2000.0f);
    camera.setFOV(45.0f);
    camera.setAspectRatio((float)width/(float)height);
}


void writeBinObject(string _fileName)
{
    auto fileName = _fileName + ".obj";
    assert(true == TestLoadObj(fileName.c_str()));
    //write to binary
    vector<int> vecSizes(3);
    int shipIdx = 0;
    fileName = _fileName + ".bin";
    FILE* pFile = fopen(fileName.c_str(), "wb");
    //write size of each vector
    vecSizes[0] = (float)shapes[shipIdx].mesh.positions.size();
    vecSizes[1] = (float)shapes[shipIdx].mesh.normals.size();
    vecSizes[2] = (float)shapes[shipIdx].mesh.indices.size();
    cout << "actual sizes of vectors: " << endl
    << "positions: " << vecSizes[0] << endl
    << "normals:   " << vecSizes[1] << endl
    << "indices:   " << vecSizes[2] << endl;
    fwrite(vecSizes.data(), sizeof(float), 3, pFile);
    
    auto &array = shapes[shipIdx].mesh.positions;
    fwrite(array.data(), 1, array.size()*sizeof(float), pFile);
    auto &array2 = shapes[shipIdx].mesh.normals;
    fwrite(array2.data(), 1, array2.size()*sizeof(float), pFile);
    auto & arrayIdx = shapes[shipIdx].mesh.indices;
    fwrite(arrayIdx.data(), 1, arrayIdx.size()*sizeof(unsigned int), pFile);
    fclose(pFile);
}

void readBinObject(string _fileName)
{
    auto shipIdx = shapes.size();
    shapes.resize(shapes.size()+1);
    //read back and compare
    vector<int> vecSizes(3);
    auto fileName = _fileName + ".bin";
    FILE* pFile = fopen(fileName.c_str(), "r");
    fread((void *)vecSizes.data(), sizeof(int), vecSizes.size(), pFile);
    cout << "sizes of vectors: " << endl
    << "positions: " << vecSizes[0] << endl
    << "normals:   " << vecSizes[1] << endl
    << "indices:   " << vecSizes[2] << endl;
    
    vector<float> fArray1, fArray2;
    vector<unsigned int> iArray1;
    {
        
        auto &fArray = shapes[shipIdx].mesh.positions;
        fArray.resize(vecSizes[0]);
        fread((void *)fArray.data(), sizeof(float), vecSizes[0], pFile);
    }
    {
        auto &fArray = shapes[shipIdx].mesh.normals;
        fArray.resize(vecSizes[1]);
        fread((void *)fArray.data(), sizeof(float), vecSizes[1], pFile);
    }
    auto &iArray = shapes[shipIdx].mesh.indices;
    iArray.resize(vecSizes[2]);
    fread((void *)iArray.data(), sizeof(int), vecSizes[2], pFile);
}


void Scene::init(int width, int height)
{
    fbWidth = width;
    fbHeight = height;
    initCamera(camera, width, height);
    
    lightPos = vec3(0, 0, -1000);
    

    
    /* mesh loading */
//    assert(true == TestLoadObj("cornell_box.obj"));
//    assert(true == TestLoadObj("suzanne.obj"));
//    assert(true == TestLoadObj("olympus_1mesh.obj"));
//    assert(true == TestLoadObj("terran_corvette.obj"));
//    assert(true == TestLoadObj("square_bracket2.obj"));

    char fileName[] = "terran_corvette";
    char fileName2[] = "square_bracket2";
//    char fileName[] = "olympus";
//    if (1) writeBinObject(fileName);
    if (1) readBinObject(fileName);
    if (1) readBinObject(fileName2);
  
}










