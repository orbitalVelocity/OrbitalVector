//
//  globe.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/2/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "globe.h"

void RenderableGlobe::init()
{
    std::string vertFilename = "planetVertex.glsl";
    std::string fragFilename = "planetFragment.glsl";
    loadShaders(vertFilename, fragFilename);
    
    loadAsset();
}


void _tesselate(int depth, GLfloat *tri0, GLfloat *tri1, GLfloat *tri2, std::vector<GLfloat> &buffer)
{
    GLfloat *tris[3] = {tri0, tri1, tri2};
    if (depth==0)
    {
        for (int i=0; i<3; ++i)
            for (int j=0; j<3; ++j)
                buffer.push_back(tris[i][j]);
    } else
    {
        GLfloat midPoint[3][3];
        //find midpoints, call _tesselate for each of 4 things
        for (int j=0; j<3; j++)
        {
            //calculate intermediate points
            //take 2 points, find middle point
            GLfloat *p1=tris[j];
            GLfloat *p2=tris[(j+1)%3];
            for (int k=0; k<3; k++) {
                midPoint[j][k] = (p1[k]+p2[k])/2;
            }
            
            //make it 1 unit from origin
            GLfloat curDist = std::sqrt(midPoint[j][0]*midPoint[j][0]+midPoint[j][1]*midPoint[j][1]+midPoint[j][2]*midPoint[j][2]);
            GLfloat scaleFactor = 1 / curDist;
            
            for(int k=0; k<3; k++)
            {
                midPoint[j][k] *= scaleFactor;
            }
        }
        //render the 4 triangles
        _tesselate(depth-1, tri0, midPoint[0], midPoint[2], buffer);
        _tesselate(depth-1, midPoint[0], tri1, midPoint[1], buffer);
        _tesselate(depth-1, midPoint[1], tri2, midPoint[2], buffer);
        _tesselate(depth-1, midPoint[0], midPoint[1], midPoint[2], buffer);
        
    }
}

void RenderableGlobe::loadAsset()
{
    // Make a sphere
    std::vector<GLfloat> vertexData;
    GLfloat X = .525731112119133606;
    GLfloat Z = .850650808352039932;
    static GLfloat vdata[12][3] = {
        {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
        {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
        {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
    };
    
    static GLint tindices[20][3] = {
        {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
        {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
        {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
        {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };
    
    int depth = 2;
    
    //tesselate each face of the icosahedron
    for(auto i=0; i<20; ++i)
    {
        _tesselate(depth,
                   vdata[tindices[i][0]],
                   vdata[tindices[i][1]],
                   vdata[tindices[i][2]],
                   vertexData
                   );
    }
    
    //setup GL states
    drawCount = (int)(vertexData.size()/3);
    drawType = GL_TRIANGLES;
    loadAttribute("position", vertexData, GL_STATIC_DRAW);
}
