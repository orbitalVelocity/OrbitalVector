//
//  scene.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/14/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "scene.h"
#include "glm/gtx/closest_point.hpp"
#include "ecs.h"


std::vector<glm::vec3> barycenters;
std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;

static bool
TestLoadObj(
            std::string filename,
            const char* basepath = NULL)
{
    std::cout << "Loading " << filename << std::endl;
    
    std::string err = tinyobj::LoadObj(shapes, materials, filename.c_str(), basepath);
    
    if (!err.empty()) {
        std::cerr << err << std::endl;
        return false;
    }

    return true;
}

void initCamera(Camera & camera, int width, int height) {
    camera.setPosition(glm::vec3(0, 0, 10.0f));
    camera.setFocus(glm::vec3(0,3.0f,0));
    camera.setClip(0.01f, 2000.0f);
    camera.setFOV(45.0f);
    camera.setAspectRatio((float)width/(float)height);
}


void writeBinObject(string _fileName)
{
    int shipIdx = (int)shapes.size()-1; //last obj read
    auto fileName = _fileName + ".obj";
    assert(true == TestLoadObj(fileName.c_str()));

    //write to binary
    std::vector<int> vecSizes(3);
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
    
    //write the actual arrays
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
    {
        auto &iArray = shapes[shipIdx].mesh.indices;
        iArray.resize(vecSizes[2]);
        fread((void *)iArray.data(), sizeof(int), vecSizes[2], pFile);
    }
    
    {
        barycenters.resize(shapes.size());
        glm::vec3 barycenter;
        //find barycenter of mesh
        for (auto i = 0; i < shapes[shipIdx].mesh.positions.size(); i += 3)
        {
            barycenter.x += shapes[shipIdx].mesh.positions[i];
            barycenter.y += shapes[shipIdx].mesh.positions[i+1];
            barycenter.z += shapes[shipIdx].mesh.positions[i+2];
        }
        barycenters[shipIdx] = barycenter / 3.0/shapes[shipIdx].mesh.positions.size();
        auto petalCenter = barycenter;
        std::cout << "petal barycenter: "
                  << petalCenter.x << ", "
                  << petalCenter.y << ", "
                  << petalCenter.z << "\n";
    }
}


void Scene::init()
{

    /* mesh loading */
#define SETUP true
#if SETUP
    std::vector<std::string> meshNames;
    meshNames.push_back("terran_corvette_small");
    meshNames.push_back("square_bracket2");
    meshNames.push_back("missile1");
    meshNames.push_back("circleMenu");
    meshNames.push_back("circleMenu_inner_circle");
    meshNames.push_back("circleMenu_leaf_top");
    
    for (auto name : meshNames)
    {
        assert(true == TestLoadObj(name + ".obj"));
        writeBinObject(name);
    }
#endif

    shapes.clear();
    for (auto name : meshNames)
    {
        readBinObject(name);
    }
    

}










