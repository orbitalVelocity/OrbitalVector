//
//  scene.h
//  GLFW3_test
//
//  Created by Si Li on 9/14/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __GLFW3_test__scene__
#define __GLFW3_test__scene__

#include <iostream>
#include "includes.h"
#include "ogl.h"
#include "orbit.h"
#include "camera.h"
#include "gameLogic.h"
#include "input.h"

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

//Scene graph handles managements of all (?) objects
//invokes game logic
class Scene {
public:
    Scene(GameLogic *_gl, UserInput *_ui)
        : orbit(GL_LINES),
          globe(GL_TRIANGLES),
          grid(GL_LINEAR_ATTENUATION),
          ship(GL_TRIANGLES),
          hdr(GL_TRIANGLES),
          highPass(GL_TRIANGLES),
          composite(GL_TRIANGLES),
          fxaa(GL_TRIANGLES),
          shadowMap(GL_TRIANGLES),
          blit(GL_TRIANGLES),
          _gameLogic(_gl),
          _userInput(_ui),
          downSizeFactor(1.0),
          debug(false)
        {
            rt.resize(maxStages);
        };
    void init(int, int);
    void render();
    void forwardRender();
    void linePick(vector<float> &dist, int &obj);
    void update();
    void postFrame();
    void setActiveShip(int s) { shipIdx = s; }
    
public:
    int shipIdx;
    GLFWwindow *window;
//    vector<OGL> renderables;
    Orbit orbit;
    OGL globe, grid, ship;
    glm::vec3 lightPos;
    Camera camera;
    friend class GameLogic;
    GameLogic *_gameLogic;
    friend class UserInput;
    UserInput *_userInput;
    
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
};



#endif /* defined(__GLFW3_test__scene__) */
