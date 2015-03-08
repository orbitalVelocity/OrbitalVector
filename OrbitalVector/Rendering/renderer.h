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
#include "ecs.h"

#include "VertexArrayObject.h"
//shaders
#include "OGLShader.h"
//#include "shadowMap.h"
#include "Highpass.h"
#include "blur.h"
#include "composite.h"
//#include "fxaa.h"

#include "orbit.h"
#include "camera.h"
#include "gameLogic.h"
#include "scene.h"
#include "grid.h"
#include "globe.h"


#define NOSHADER false
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
    globe(GL_TRIANGLES),
    grid(GL_LINEAR_ATTENUATION),
    ship(GL_TRIANGLES),
    sprite(GL_TRIANGLES),
    missile(GL_TRIANGLES),
    hdr(GL_TRIANGLES),
    highPass(GL_TRIANGLES),
    composite(GL_TRIANGLES),
    fxaa(GL_TRIANGLES),
    highpassShader(GL_TRIANGLES),
    blurShader(GL_TRIANGLES),
    compositeShader(GL_TRIANGLES),
//    shadowmapShader(GL_TRIANGLES),
//    fxaaShader(GL_TRIANGLES),
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
    /*
     * perform actions between frames
     * like reloading shaders
     */
    void postFrame();
    
public:
    VertexArrayObject quadVAO;
    //    vector<OGLShader> renderables;
    OGLShader ship, sprite, missile;
    RenderableGrid grid;
    RenderableGlobe globe;
    glm::vec3 lightPos;
    friend class GameLogic;
    friend class UserInput;
    GameLogic &gameLogic;
    UserInput &userInput;
    Scene &scene;
    
    //    RenderTarget rt, rtBloom, rtBloomV, rtShadowMap;
    vector<RenderTarget> rt;
    OGLShader hdr, highPass, shadowMap,
        blit, composite, fxaa;
    
//    ShadowMapShader shadowmapShader;
    HighpassShader highpassShader;
    BlurShader blurShader;
    CompositeShader compositeShader;
//    FXAAShader fxaaShader;
    
    GLuint quad_vertexbuffer;
    GLuint quad_vertexPosition_modelspace;
    GLuint texID, timeID, coefficientID;
    int fbWidth, fbHeight;
    
    bool debug;
    float downSizeFactor;
    int selected, mouseHover;
};



#endif /* defined(__OrbitalVector__renderer__) */
