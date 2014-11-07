//
//  renderer.h
//  OrbitalVector
//
//  Created by Si Li on 11/6/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__renderer__
#define __OrbitalVector__renderer__


#include <iostream>
#include "includes.h"
#include "ogl.h"
#include "orbit.h"
#include "camera.h"
#include "gameLogic.h"
#include "scene.h"

enum SN {
    shadowMap,
    forward,
    highPass,
    blur1,
    blur2,
    comp,
    fxaa,
    //don't need to worry about this last pass blit,
    maxStages
};
using namespace std;

class RenderTarget {
public:
    RenderTarget() : useMipMap(false), useHDR(true)
    {
    }
    void init(int, int, bool depthTexture=false);
public:
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint FramebufferName;
	GLuint renderedTexture;
    GLuint depthrenderbuffer;
    bool useMipMap;
    bool useHDR;
};

//Renderer graph handles managements of all (?) objects
//invokes game logic
class Renderer {
public:
    Renderer(Scene &s, GameLogic &g, UserInput &i)
    : scene(s), gameLogic(g),
    userInput(i),
    orbit(GL_LINES),
    globe(GL_TRIANGLES),
    grid(GL_LINEAR_ATTENUATION),
    ship(GL_TRIANGLES),
    sprite(GL_TRIANGLES),
    hdr(GL_TRIANGLES),
    highPass(GL_TRIANGLES),
    composite(GL_TRIANGLES),
    fxaa(GL_TRIANGLES),
    shadowMap(GL_TRIANGLES),
    blit(GL_TRIANGLES),
    downSizeFactor(1.0),
    debug(false)
    {
        rt.resize(maxStages);
        selected = -1;
        mouseHover = -1;
    };
    void init(int, int);
    void render();
    void forwardRender();
    void update();
    void postFrame();
    
public:
    //    vector<OGL> renderables;
    Orbit orbit;
    OGL globe, grid, ship, sprite;
    glm::vec3 lightPos;
    Camera camera;
    friend class GameLogic;
    friend class UserInput;
    GameLogic &gameLogic;
    UserInput &userInput;
    Scene &scene;
    
    //    RenderTarget rt, rtBloom, rtBloomV, rtShadowMap;
    vector<RenderTarget> rt;
    OGL hdr, highPass, shadowMap,
    blit, composite, fxaa;
    GLuint quad_vertexbuffer;
    GLuint quad_vertexPosition_modelspace;
    GLuint texID, timeID, coefficientID;
    int fbWidth, fbHeight;
    
    bool debug;
    float downSizeFactor;
    glm::vec3 rayStart, rayEnd;
    glm::vec3 obj;
    int selected, mouseHover;
};



#endif /* defined(__OrbitalVector__renderer__) */
