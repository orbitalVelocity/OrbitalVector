//
//  main.cpp
//  GLFW3_test
//
//  Created by Si Li on 3/18/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//



#define GLFW_INCLUDE_GLCOREARB
//#include <OpenGL/glu.h>
#include "includes.h"
#include "OGLShader.h"
#include <GLFW/glfw3.h>

#include "ecs.h"
#include "cameraComponent.h"
#include "scene.h"
#include "orbit.h"
#include "tiny_obj_loader.h"
#include "text.h"
#include "renderer.h"
#include "GPUTimer.h"
#include "log.h"
#define CUSTOM_VSYNC 2
#define VSYNC 1

using namespace std;

#define QUERY_BUFFERS 2
#define QUERY_COUNT 1


GPUtimer gpuTimer;
/*
 * error call back function
 */
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


/*
 * prints to console all vertices, indices, and normals of a mesh
 * @Refactor Goal   move to mesh class, or at least file
 */
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

const int ftSize = 120;

/**
 Measures frame rate, frame latency, GPU and CPU timing
 
 Includes debugging functionality such as
 @code
    - frame rate limiting for breakpoint while debugging
    - other debugging functionalities?
 */
class PerfMon {
public:
    PerfMon() : fps(ftSize), renderTimes(ftSize),
                gpuRenderTimes(ftSize), gpuTimes(3),
                tPrevFrame(0), renderTime(0.0f), dt(0.0f)
    {
    }
    void update(double thisTime)
    {
        tFrameStart = thisTime;
        dt = tFrameStart - tPrevFrame;
        tPrevFrame = tFrameStart;
        fps.push(1.0f/dt);
        renderTimes.push(renderTime*1000.0f);
        
        frameRateLimitForDebugging();
        
        startGPUTimer(&gpuTimer);
    }
    /**
     Prevents artificially large frame time when paused at
     break point during debugging
     */
    void frameRateLimitForDebugging()
    {
        //for debugging: framelock to 1/30 second
        const auto frameTimeLimit = 1.0f/30.0f;
        dt = (dt > frameTimeLimit) ? frameTimeLimit : dt;
    }
    void frameEnd(double thisTime)
    {
        renderTime = thisTime - tFrameStart;
        stopGPUTimer(&gpuTimer, gpuTimes.data(), 3);
        gpuRenderTimes.push(gpuTimes[0]*1000.0);
    }
    
public:
    double tPrevFrame, tFrameStart;
    float renderTime;
    RingBuffer<float> fps, renderTimes, gpuRenderTimes;
//    (120), renderTimes(120), gpuRenderTimes(120);
    vector<float> gpuTimes;
    float dt;
};

/**
 Convenience POD for holding GLFW window states
 */
struct WindowStates {
    GLFWwindow* pWindow;
    int fbWidth, fbHeight;
    int winWidth, winHeight;
};

/**
 get and change window/framebuffer dimension to support 
 mouse dragging of window border
 @param ws    a reference to a convenience struct for holding window related states
 */
void updateWindowSize(WindowStates &ws)
{
    glfwGetWindowSize(ws.pWindow, &ws.winWidth, &ws.winHeight);
    glfwGetFramebufferSize(ws.pWindow, &ws.fbWidth, &ws.fbHeight);
    glViewport(0, 0, ws.fbWidth, ws.fbHeight);
}
//auto getText = [&]()
void getText(TextRenderer &textObj, PerfMon &perfMon, WindowStates &ws)
{
    auto fbWidth = ws.fbWidth;
    auto fbHeight = ws.fbHeight;
    auto winWidth = ws.winWidth;
    auto winHeight = ws.winHeight;
    
    stringstream textOut;
    textOut << "Frame: " << std::fixed << std::setprecision(1)
    << 1000.0/perfMon.fps.average()
    << "ms max: " << 1000.0/perfMon.fps.min()
    << "ms min: " << 1000.0/perfMon.fps.max() << "ms";
    textObj.pushBackDebug(textOut);
    textOut << "cpu time: " << std::fixed << std::setprecision(2)
    << perfMon.renderTimes.average() << "ms max: " << perfMon.renderTimes.max()
    << "ms min: " << perfMon.renderTimes.min();
    textObj.pushBackDebug(textOut);
    textOut << "gpu time: " << std::fixed << std::setprecision(2)
    << perfMon.gpuRenderTimes.average() << "ms max: " << perfMon.gpuRenderTimes.max()
    << "ms min: " << perfMon.gpuRenderTimes.min();
    //        << std::accumulate(gpuTimes.begin(), gpuTimes.end(), 0) << "ms";
    textObj.pushBackDebug(textOut);
    textOut << "win " << winWidth << " x " << winHeight
    << " fb size " << fbWidth << " x " << fbHeight;
    textObj.pushBackDebug(textOut);
    
    
    //the following code should go into UserInput and GameLogic, not out here

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
}

int main(int argc, const char * argv[])
{
    log_init(stdout);

    log(LOG_DEBUG,"Creating window.");

    int width = 960, height = 540;

    WindowStates ws;
    ws.pWindow = initGraphics(width, height);
    glfwGetWindowSize(ws.pWindow, &ws.winWidth, &ws.winHeight);
    glfwGetFramebufferSize(ws.pWindow, &ws.fbWidth, &ws.fbHeight);
    glViewport(0, 0, ws.fbWidth, ws.fbHeight);

    log(LOG_DEBUG,"Creating GameSingleton.");

    GameSingleton myGameSingleton("testMap");
    myGameSingleton.load("test", width, height);
    assert(myGameSingleton.myShip.valid());

    initFontStash();
    myGameSingleton.renderer.init(ws.fbWidth, ws.fbHeight);
        check_gl_error();
    
    // performance measurement
    glfwSetTime(0);
    PerfMon perfMon;
    perfMon.tPrevFrame = glfwGetTime();
    
    myGameSingleton.pWindow = ws.pWindow;

    log(LOG_DEBUG,"Entering game loop.");

    while (!glfwWindowShouldClose(ws.pWindow))
    {
        perfMon.update(glfwGetTime());
        auto dt = perfMon.dt;
    
        updateWindowSize(ws);
        
        // Calculate pixel ratio for hi-dpi devices.
        auto pxRatio = (float)ws.fbWidth / (float)ws.winWidth;
        myGameSingleton.textObj.updateSettings(pxRatio, ws.fbWidth, ws.fbHeight);
		
        myGameSingleton.textObj.debugTexts.clear();
        getText(myGameSingleton.textObj, perfMon, ws);
       
        myGameSingleton.update(dt);
        
        
        myGameSingleton.renderer.update();

        myGameSingleton.renderer.render(myGameSingleton.entities);
        myGameSingleton.textObj.render();
       
        perfMon.frameEnd(glfwGetTime());

        glfwSwapBuffers(ws.pWindow);
        myGameSingleton.renderer.postFrame();
        
        /* Poll for events (kb, mouse, joystick) and process them via glfw callbacks*/
        glfwPollEvents();
    }
    
    glfwTerminate();

    log(LOG_DEBUG,"Terminating.");
    log_free();

    return 0;
}
