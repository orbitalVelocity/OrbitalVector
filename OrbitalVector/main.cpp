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
#include "rk547m.h"
#include "OGLShader.h"
#include <GLFW/glfw3.h>

#include "camera.h"
#include "scene.h"
#include "orbit.h"
#include "tiny_obj_loader.h"
#include "spatial.h"
#include "text.h"
#include "gameLogic.h"
#include "renderer.h"
#include "GPUTimer.h"
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

/**
 * initializes entities in the scene
 * Refactor Goal: move to scene class, but data driven (from save file, preferably)
 */
void initPhysics()
{
    //hardcoded, need to go into object creation code
    double m = 7e12;
    double G = 6.673e-11;
    double gm = m * G;
    
#define oldway 0
#if oldway
    sys.push_back(body(state(glm::vec3(), glm::vec3(0, 0, -.1)),
                       gm,
                       10,
                       nullptr,
                       BodyType::GRAV
                       )
                  );
#else
    auto planet = body(state(glm::vec3(), glm::vec3(0, 0, -.1)),
                       gm,
                       10,
                       nullptr,
                       BodyType::GRAV
                       );
    InsertToSys(planet, BodyType::GRAV);
#endif
    glm::vec3 rad(110, 0, 0);
    glm::vec3 vel(0, 0, 2.3);
    m = 1e5;
    gm = m * G;

#if oldway
    sys.push_back(body(state(rad, vel),
                       gm,
                       1,
                       nullptr,
                       BodyType::SHIP
                       )
                  );
#else
    auto ship = body(state(rad, vel),
                       gm,
                       1,
                       nullptr,
                       BodyType::SHIP
                     );
    InsertToSys(ship, BodyType::SHIP);
#endif
    const int numTerms = 8;
    ks.resize(numTerms);
    for (auto &k : ks)
        k.resize(sys.size());
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
//    textOut << "planet (" << printVec3(sys[0].sn.pos) << ")";
//    textObj.pushBackDebug(textOut);
//    textOut << "planet v: " << glm::length(sys[0].sn.vel);
//    textObj.pushBackDebug(textOut);
//    textOut << "ship vel: " << glm::length(sys[1].sn.vel);
//    textObj.pushBackDebug(textOut);
//    textOut << "projectiles: " << sys.size() - 2;
//    textObj.pushBackDebug(textOut);
//    textOut << "paths: " << scene.orbit.paths.size() << " | ";
//    for ( auto & path : scene.orbit.paths) {
//        textOut << path.size()/3 << " ";
//    }
//    textObj.pushBackDebug(textOut);
//    textOut << "path size: " << scene.orbit.drawCount;
//    textObj.pushBackDebug(textOut);
//    textOut << "grav offset: " << sysIndexOffset[BodyType::GRAV]
//    << " size: " << numBodyPerType[BodyType::GRAV];
//    textObj.pushBackDebug(textOut);
//    textOut << "ship offset: " << sysIndexOffset[BodyType::SHIP]
//    << " size: " << numBodyPerType[BodyType::SHIP];
//    textObj.pushBackDebug(textOut);
//    textOut << "miss offset: " << sysIndexOffset[BodyType::MISSILE]
//    << " size: " << numBodyPerType[BodyType::MISSILE];
//    textObj.pushBackDebug(textOut);
//    textOut << "proj offset: " << sysIndexOffset[BodyType::MISSILE]
//    << " size: " << numBodyPerType[BodyType::MISSILE];
//    textObj.pushBackDebug(textOut);
   
//    //mouse debug
//    double mx, my;
//    glfwGetCursorPos(window, &mx, &my);
//    textOut << "mouse: x " << mx << " y " << my;
//    textObj.pushBackDebug(textOut);
//    

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
    
    
    
//    return;
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

void getLinePick(TextRenderer &textObj, GameLogic &gameLogic)
{
    stringstream textOut;
    for (int i=0; i < gameLogic.shortestDist.size(); i++)
    {
        textOut << "obj: " << i << " dist: " << gameLogic.shortestDist[i];
        textObj.pushBackDebug(textOut);
    }
    textOut << "selected: " << gameLogic.selected;
    textObj.pushBackDebug(textOut);
    textOut << "mouseOver: " << gameLogic.mouseHover;
    textObj.pushBackDebug(textOut);
}

int main(int argc, const char * argv[])
{
//    int width = 1280, height = 720;
    int width = 960, height = 540;
//    int width = 1440*2, height = 900*2;
//    int width = 1440*1, height = 900*1;
//    int width = 1920, height = 1080;
//    GLFWwindow* window = initGraphics(width, height);
    
    WindowStates ws;
    ws.pWindow = initGraphics(width, height);
    glfwGetWindowSize(ws.pWindow, &ws.winWidth, &ws.winHeight);
    glfwGetFramebufferSize(ws.pWindow, &ws.fbWidth, &ws.fbHeight);
    glViewport(0, 0, ws.fbWidth, ws.fbHeight);
   
    
    // Calculate pixel ratio for hi-dpi devices.
    auto pxRatio = (float)ws.fbWidth / (float)ws.winWidth;
    
    initFontStash();
    initPhysics();
    UserInput inputObject;
    Scene scene;
    GameLogic gameLogic(ws.pWindow, scene, inputObject);
    scene.init(ws.fbWidth, ws.fbHeight);
    Renderer renderer(scene, gameLogic, inputObject);
    renderer.init(ws.fbWidth, ws.fbHeight);
        check_gl_error();
    
    // creating vector of string
    TextRenderer textObj(pxRatio, ws.fbWidth, ws.fbHeight);
    textObj.guiText.push_back(Text(glm::vec2(.5, .4), 10.0f, "planet"));
    textObj.guiText.push_back(Text(glm::vec2(.5, .4), 10.0f, std::to_string(scene.orbit.apo)));
    textObj.guiText.push_back(Text(glm::vec2(.5, .4), 10.0f, std::to_string(scene.orbit.peri)));

    auto UITextSetup = [&](){
        auto vp = scene.camera.matrix() * world;
        textObj.guiText[0].pos = getVec2(vp, sys[0].sn.pos);
        textObj.guiText[1].pos = getVec2(vp, scene.orbit.apoPos);
        textObj.guiText[2].pos = getVec2(vp, scene.orbit.periPos);
        
        textObj.guiText[1].text = std::to_string(scene.orbit.apo);
        textObj.guiText[2].text = std::to_string(scene.orbit.peri);
    };
    
    // performance measurement
    glfwSetTime(0);
    PerfMon perfMon;
    perfMon.tPrevFrame = glfwGetTime();
    
    //FIXME: for refactoring only
    myGameSingleton.pWindow = ws.pWindow;
    myGameSingleton.pCamera = &scene.camera;
    myGameSingleton.init(&inputObject, &textObj);
    
    while (!glfwWindowShouldClose(ws.pWindow))
    {
        perfMon.update(glfwGetTime());
        auto dt = perfMon.dt;
    
        updateWindowSize(ws);
        
        // Calculate pixel ratio for hi-dpi devices.
        auto pxRatio = (float)ws.fbWidth / (float)ws.winWidth;
        textObj.updateSettings(pxRatio, ws.fbWidth, ws.fbHeight);
		
        textObj.debugTexts.clear();
        gameLogic.processActionList(inputObject.actionList);
        gameLogic.update(dt);
        myGameSingleton.update(dt);
        renderer.update();

        getText(textObj, perfMon, ws);
        getLinePick(textObj, gameLogic);
        UITextSetup();
       
        renderer.render();
        textObj.render();
       
        perfMon.frameEnd(glfwGetTime());

        glfwSwapBuffers(ws.pWindow);
        renderer.postFrame();
        
        /* Poll for events (kb, mouse, joystick) and process them via glfw callbacks*/
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}