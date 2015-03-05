//
//  globe.h
//  OrbitalVector
//
//  Created by Si Li on 3/2/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__globe__
#define __OrbitalVector__globe__

#include <stdio.h>
#include "OGLShader.h"
using namespace std;

class RenderableGlobe : public OGLShader
{
public:
    RenderableGlobe(GLenum drawType) : OGLShader(drawType) {}
    void init() override;
    void loadAsset () override;
};

class globeType
{
public:
    RenderableGlobe renderableGlobe;
};

#endif /* defined(__OrbitalVector__globe__) */
