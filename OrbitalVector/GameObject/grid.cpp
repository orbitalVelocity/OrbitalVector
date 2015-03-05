//
//  grid.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/2/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "grid.h"

void RenderableGrid::init()
{
    //load shaders
    string vertFilename = "lineVertex.glsl";
    string fragFilename = "lineFragment.glsl";
    loadShaders(vertFilename, fragFilename);
    
    loadAsset();
}

void RenderableGrid::loadAsset()
{
    //generate positions
    std::vector<float> path;
    int vecSize = 3;
    
    float gridSize = 1024;
    for (int i=0; i < gridSize; i+=16) {
        path.push_back(i-gridSize/2);
        path.push_back(0.0);
        path.push_back(gridSize/2);
        path.push_back(i-gridSize/2);
        path.push_back(0.0);
        path.push_back(-gridSize/2);
    }
    for (int i=0; i < gridSize; i+=16) {
        path.push_back(gridSize/2);
        path.push_back(0.0);
        path.push_back(i-gridSize/2);
        path.push_back(-gridSize/2);
        path.push_back(0.0);
        path.push_back(i-gridSize/2);
    }
    
    drawType = GL_LINES;
    drawCount = (int)(path.size()/vecSize);
    loadAttribute("position", path, GL_DYNAMIC_DRAW);
}