
//  main.cpp
//  GLFW3_test
//
//  Created by Si Li on 3/18/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//



#define GLFW_INCLUDE_GLCOREARB
//#include <OpenGL/glu.h>
#include "includes.h" 
#include "rk547m.h"
#include "ogl.h"
#include <GLFW/glfw3.h>

#include "camera.h"
#include "scene.h"
#include "orbit.h"
#include "tiny_obj_loader.h"
#include "spatial.h"
#include "text.h"
#include "gameLogic.h"
#include "renderer.h"
#include "EntityManager.h"
#include "GPUTimer.h"
#define CUSTOM_VSYNC 2
#define VSYNC 1

using namespace std;

#define QUERY_BUFFERS 2
#define QUERY_COUNT 1


GPUtimer gpuTimer;

void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

GLFWwindow* initGraphics(int width, int height)
{
    GLFWwindow* window;
    /* Initialize the library */
    assert(glfwInit());

	glfwSetErrorCallback(errorcb);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//	glfwWindowHint(GLFW_SAMPLES, 8);
	
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello world", NULL, NULL);
//    window = glfwCreateWindow(width, height, "Hello world", glfwGetPrimaryMonitor(), NULL);
    if (!window)
    {
        glfwTerminate();
        assert(false);
    }
   	
	glfwSetKeyCallback(window, UserInput::key_callback_dispatch);
    glfwSetMouseButtonCallback(window, UserInput::mb_callback_dispatch);
    glfwSetScrollCallback(window, UserInput::scroll_callback_dispatch);
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    // set to 1 for v-sync
    glfwSwapInterval(0);
    
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		assert(false);
	}
    glGetError(); //throws away benign error from glewInit on mac
 
    initGPUTimer(&gpuTimer);
    return window;
}



static void PrintInfo(const std::vector<tinyobj::shape_t>& shapes, const std::vector<tinyobj::material_t>& materials)
{
  std::cout << "# of shapes    : " << shapes.size() << std::endl;
  std::cout << "# of materials : " << materials.size() << std::endl;

  for (size_t i = 0; i < shapes.size(); i++) {
    printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
    printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
    printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
    assert((shapes[i].mesh.indices.size() % 3) == 0);
    for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
      printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f,
             shapes[i].mesh.indices[3*f+0],
             shapes[i].mesh.indices[3*f+1],
             shapes[i].mesh.indices[3*f+2],
             shapes[i].mesh.material_ids[f]);
    }

    printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
    assert((shapes[i].mesh.positions.size() % 3) == 0);
    for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
      printf("  v[%ld] = (%f, %f, %f)\n", v,
        shapes[i].mesh.positions[3*v+0],
        shapes[i].mesh.positions[3*v+1],
        shapes[i].mesh.positions[3*v+2]);
    }
    printf("shape[%ld].normals: %ld\n", i, shapes[i].mesh.normals.size());
    for (size_t v = 0; v < shapes[i].mesh.normals.size() / 3; v++) {
      printf("  n[%ld] = (%f, %f, %f)\n", v,
        shapes[i].mesh.normals[3*v+0],
        shapes[i].mesh.normals[3*v+1],
        shapes[i].mesh.normals[3*v+2]);
    }
  }

}

int main(int argc, const char * argv[])
{
//    int width = 1280, height = 720;
    int width = 960, height = 540;
//    int width = 1440*2, height = 900*2;
//    int width = 1440*1, height = 900*1;
//    int width = 1920, height = 1080;
    GLFWwindow* window = initGraphics(width, height);
    int winWidth, winHeight;
    int fbWidth, fbHeight;
    
    glfwGetWindowSize(window, &winWidth, &winHeight);
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    
    initFontStash();
    UserInput inputObject;
    Scene scene;
    scene.init(fbWidth, fbHeight);  //THIS THRASHES scene.orbit.entityManager!!
    GameLogic gameLogic(window, &scene, &inputObject);
    Renderer renderer(&scene, &gameLogic, &inputObject);
    renderer.init(fbWidth, fbHeight);
        check_gl_error();
    
    // performance measurement
    glfwSetTime(0);
    auto prevt = glfwGetTime();
    float renderTime = 0.0f;
    auto size = 120;
    RingBuffer<float> fps(size), renderTimes(size), gpuRenderTimes(size);
    vector<float> gpuTimes;
    gpuTimes.resize(3);
    
    // Calculate pixel ratio for hi-dpi devices.
    auto pxRatio = (float)fbWidth / (float)width;
    
    // creating vector of string for printing to screen
    TextRenderer textObj(pxRatio, fbWidth, fbHeight);
    auto getText = [&]()
    {
        stringstream textOut;
        textObj.debugTexts.clear();
        textOut << "Frame: " << std::fixed << std::setprecision(1)
                << 1000.0/fps.average()
                << "ms max: " << 1000.0/fps.min()
                << "ms min: " << 1000.0/fps.max() << "ms";
        textObj.pushBackDebug(textOut);
        textOut << "cpu time: " << std::fixed << std::setprecision(2)
                << renderTimes.average() << "ms max: " << renderTimes.max()
                << "ms min: " << renderTimes.min();
        textObj.pushBackDebug(textOut);
        textOut << "gpu time: " << std::fixed << std::setprecision(2)
                << gpuRenderTimes.average() << "ms max: " << gpuRenderTimes.max()
                << "ms min: " << gpuRenderTimes.min();
//        << std::accumulate(gpuTimes.begin(), gpuTimes.end(), 0) << "ms";
        textObj.pushBackDebug(textOut);
        textOut << "win " << winWidth << " x " << winHeight
                << " fb size " << fbWidth << " x " << fbHeight;
        textObj.pushBackDebug(textOut);
//        textOut << "planet (" << printVec3(sys[0].sn.pos) << ")";
//        textObj.pushBackDebug(textOut);
//        textOut << "planet v: " << glm::length(sys[0].sn.vel);
//        textObj.pushBackDebug(textOut);
//        textOut << "ship vel: " << glm::length(sys[1].sn.vel);
//        textObj.pushBackDebug(textOut);
//        textOut << "projectiles: " << sys.size() - 2;
//        textObj.pushBackDebug(textOut);
        textOut << "paths: " << scene.orbit.paths.size() << " | ";
        for ( auto & path : scene.orbit.paths) {
            textOut << path.size()/3 << " ";
        }
        textObj.pushBackDebug(textOut);
        textOut << "path size: " << scene.orbit.drawCount;
        textObj.pushBackDebug(textOut);
#if OLDWAY
        textOut << "grav offset: " << sysIndexOffset[Family::GRAV]
        << " size: " << numBodyPerType[Family::GRAV];
        textObj.pushBackDebug(textOut);
        textOut << "ship offset: " << sysIndexOffset[Family::SHIP]
        << " size: " << numBodyPerType[Family::SHIP];
        textObj.pushBackDebug(textOut);
        textOut << "miss offset: " << sysIndexOffset[Family::MISSILE]
        << " size: " << numBodyPerType[Family::MISSILE];
        textObj.pushBackDebug(textOut);
        textOut << "proj offset: " << sysIndexOffset[Family::PROJECTILE]
        << " size: " << numBodyPerType[Family::PROJECTILE];
        textObj.pushBackDebug(textOut);
#endif
        //mouse debug
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        textOut << "mouse: x " << mx << " y " << my;
        textObj.pushBackDebug(textOut);
        
        vector<float> dist;
        int obj = -1;
        gameLogic.linePick(dist, obj);
        for (int i=0; i < dist.size(); i++)
        {
            textOut << "obj: " << i << " dist: " << dist[i];
            textObj.pushBackDebug(textOut);
        }
        textOut << "selected: " << gameLogic.selected;
        textObj.pushBackDebug(textOut);
        textOut << "mouseOver: " << gameLogic.mouseHover;
        textObj.pushBackDebug(textOut);
//        textOut << "ray start: " << std::fixed
//                << std::setprecision(2)
//                << printVec3(scene.rayStart);
//        textObj.pushBackDebug(textOut);
//        textOut << "ray end: " <<std::fixed
//                << std::setprecision(2)
//                <<printVec3(scene.rayEnd);
//        textObj.pushBackDebug(textOut);
//        textOut << "planet camera coord: " << std::fixed
//                << std::setprecision(2)
//                << printVec3(scene.obj);
//        textObj.pushBackDebug(textOut);
        
        
        
        return;
        textOut << "shadow pass: 1   " << ((renderStage & stage1) ? "On" : " ");
        textObj.pushBackDebug(textOut);
        textOut << "forward pass: 2  " << ((renderStage & stage2) ? "On" : " ");
        textObj.pushBackDebug(textOut);
        textOut << "highpass: 3           " << ((renderStage & stage3) ? "On" : " ");
        textObj.pushBackDebug(textOut);
        textOut << "blur1 pass: 4         " << ((renderStage & stage4) ? "On" : " ");
        textObj.pushBackDebug(textOut);
        textOut << "blur2 pass: 5         " << ((renderStage & stage5) ? "On" : " ");
        textObj.pushBackDebug(textOut);
        textOut << "composite pass: 6 " << ((renderStage & stage6) ? "On" : " ");
        textObj.pushBackDebug(textOut);
        textOut << "fxaa pass: 7            " << ((renderStage & stage7) ? "On" : " ");
        textObj.pushBackDebug(textOut);
    };
    
    //init GUI text
    textObj.guiText.push_back(Text(glm::vec2(.5, .4), 10.0f, "planet"));
    textObj.guiText.push_back(Text(glm::vec2(.5, .4), 10.0f, std::to_string((float)scene.orbit.apo)));
    textObj.guiText.push_back(Text(glm::vec2(.5, .4), 10.0f, std::to_string((float)scene.orbit.peri)));
    prevt = glfwGetTime();
    
    
    while (!glfwWindowShouldClose(window))
    {
        /* performance measurement setup */
        auto t = glfwGetTime();
        float dt = t - prevt;
        prevt = t;
        fps.push(1.0f/dt);
        renderTimes.push(renderTime*1000.0f);
		startGPUTimer(&gpuTimer);
    
        /* get window size */
   		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        
		glViewport(0, 0, fbWidth, fbHeight);
        
        // Calculate pixel ratio for hi-dpi devices.
        auto pxRatio = (float)fbWidth / (float)winWidth;
        textObj.updateSettings(pxRatio, fbWidth, fbHeight);
		
        /* Set up a blank screen */
//        glClearColor(0.0,0.0,0.0,1);
//        glClearColor(0.5,0.5,0.5,1);
//        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        check_gl_error();
        
        /* physics, handle user input, ai, game states, 
         * it's a lot here 
         * Refactor Goal: 
         *      receive user input
         *      update states as a result of physics
         *      generate actions
         *          from user, from AI, from entities
         *      process actions via gameLogic.processActionList()
         *      update states
         *      render
         */
        gameLogic.processActionList(inputObject.actionList);
        gameLogic.update(dt);
        renderer.update();

        /* setup text  */
        getText();
        
        //GUI setup
        auto vp = scene.camera.matrix() * world;
//        textObj.guiText[0].pos = vec2();
//        textObj.guiText[0].pos = getVec2(vp, sys[0].sn.pos);
        textObj.guiText[0].pos = getVec2(vp, scene.orbit.apoPos);
        textObj.guiText[1].pos = getVec2(vp, scene.orbit.apoPos);
        textObj.guiText[2].pos = getVec2(vp, scene.orbit.periPos);

        textObj.guiText[1].text = std::to_string(scene.orbit.apo);
        textObj.guiText[2].text = std::to_string(scene.orbit.peri);
       
        /* render everything else */
		glEnable(GL_DEPTH_TEST);
        check_gl_error();

        renderer.render();
        
		glDisable(GL_DEPTH_TEST);
        textObj.render();
       
//        glFinish(); //16ms w/o vsync, 32ms w/ vsync for some reason
        renderTime = glfwGetTime() - t;
		stopGPUTimer(&gpuTimer, gpuTimes.data(), 3);
        gpuRenderTimes.push(gpuTimes[0]*1000.0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        renderer.postFrame();
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}