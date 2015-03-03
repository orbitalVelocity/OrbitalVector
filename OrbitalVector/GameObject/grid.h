//
//  grid.h
//  OrbitalVector
//
//  Created by Si Li on 3/2/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__grid__
#define __OrbitalVector__grid__

#include <stdio.h>
#include "renderableType.h"
using namespace std;

class RenderableGrid : public RenderableType
{
public:
    RenderableGrid(GLenum drawType) : RenderableType(drawType) {}
    void init() override;
    void loadAsset () override;
};

class gridObject
{
public:
    RenderableGrid renderableGrid;
};
#endif /* defined(__OrbitalVector__grid__) */
