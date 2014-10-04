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
#include "ogl.h"
#include <GLFW/glfw3.h>

#include "camera.h"
#include "scene.h"
#include "orbit.h"
#include "tiny_obj_loader.h"
#include "spatial.h"
#include "text.h"
#include "gameLogic.h"

#define CUSTOM_VSYNC 2
#define VSYNC 1

using namespace std;


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
	glfwWindowHint(GLFW_SAMPLES, 4);
	
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello world", NULL, NULL);
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
    
    glfwSwapInterval(1);
    
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		assert(false);
	}
    glGetError(); //throws away benign error from glewInit on mac
 
    return window;
}

void initPhysics()
{
    //hardcoded, need to go into object creation code
    double m = 7e12;
    double G = 6.673e-11;
    double gm = m * G;
    sys.push_back(body(state(glm::vec3(), glm::vec3(0, 0, 0)),
                       gm,
                       100,
                       nullptr,
                       objType::PLANET
                       )
                  );
    glm::vec3 rad(110, 0, 0);
    glm::vec3 vel(0, 0, 1.3);
    m = 1e5;
    gm = m * G;
    sys.push_back(body(state(rad, vel),
                       gm,
                       1,
                       nullptr,
                       objType::SHIP
                       )
                  );

    const int numTerms = 8;
    ks.resize(numTerms);
    for (auto &k : ks)
        k.resize(sys.size());
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
    int width = 1280, height = 720;
    GLFWwindow* window = initGraphics(width, height);
    
    initFontStash();
    initPhysics();
    UserInput inputObject;
    GameLogic gameLogic;
    Scene scene(&gameLogic, &inputObject);
    scene.window = window;
    scene.init(width, height);
    
    // performance measurement
    glfwSetTime(0);
    auto prevt = glfwGetTime();
    float renderTime = 0.0f;
    auto size = 60;
    RingBuffer<float> fps(size), renderTimes(size);
    
    static float x=0, y=0;
    int winWidth, winHeight;
    int fbWidth, fbHeight;
    
    // creating vector of string
    auto pushClear = [](vector<string> &vec, stringstream & ss)
    {
        vec.push_back(ss.str());
        ss.str(string());
        ss.clear();
    };
    auto getText = [&](vector<string> & textOuts)
    {
        stringstream textOut;
        textOut << "FPS: " << std::setprecision(4) << fps.average() << "";
        pushClear(textOuts, textOut);
        textOut << "render time: " << std::setprecision(4)
                << renderTimes.average() << "ms";
        pushClear(textOuts, textOut);
        textOut << "mouse x: " << x << " y: " << y << " window size " << fbWidth << " x " << fbHeight;
        pushClear(textOuts, textOut);
        textOut << "ship (" << printVec3(sys[1].sn.pos) << ")";
        pushClear(textOuts, textOut);
        textOut << "ship dist: " << glm::length(sys[1].sn.pos);
        pushClear(textOuts, textOut);
        textOut << "ship vel: " << glm::length(sys[1].sn.vel);
        pushClear(textOuts, textOut);
        textOut << "projectiles: " << sys.size() - 2;
        pushClear(textOuts, textOut);
        textOut << "path size: " << scene.orbit.drawCount;
        pushClear(textOuts, textOut);
    };
    
    //init GUI text
    guiText.push_back(Text(glm::vec2(.5, .4), 10.0f, "planet"));
    guiText.push_back(Text(glm::vec2(.5, .4), 10.0f, to_string(scene.orbit.apo)));
    guiText.push_back(Text(glm::vec2(.5, .4), 10.0f, to_string(scene.orbit.peri)));
    
    while (!glfwWindowShouldClose(window))
    {
        /* performance measurement setup */
        auto t = glfwGetTime();
        float dt = t - prevt;
        prevt = t;
        fps.push(1.0f/dt);
        renderTimes.push(renderTime*1000.0f);
    
        /* get window size */
   		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		glViewport(0, 0, fbWidth, fbHeight);
        
        // Calculate pixel ratio for hi-dpi devices.
        auto pxRatio = (float)fbWidth / (float)winWidth;
		
        /* Set up a blank screen */
//        glClearColor(0.1,0.1,0.1,1);
        glClearColor(0.5,0.5,0.5,1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        check_gl_error();
        
        /* physics, handle user input, ai, game states, 
         * it's a lot here 
         */
        gameLogic.processActionList(inputObject.actionList);
        gameLogic.update(dt);
        scene.update();

        
        /* render text  */
        vector<string> textOuts;
        getText(textOuts);
        
        //GUI setup
#if 1
        auto get2d = [&](glm::vec3 _pos)
        {
            glm::vec4 pos = scene.camera.matrix() * world * glm::vec4(_pos, 1.0f);
            pos.x /= pos.z;
            pos.y /= pos.z;
            pos.x = ( pos.x+1.0f) / 2;
            pos.y = (-pos.y+1.0f) / 2; //FIXME: why y has to be negative?
            return glm::vec2(pos.x, pos.y);
        };
        guiText[0].pos = get2d(sys[0].sn.pos);
        guiText[1].pos = get2d(scene.orbit.apoPos);
        guiText[2].pos = get2d(scene.orbit.periPos);
        
        guiText[1].text = to_string(scene.orbit.apo);
        guiText[2].text = to_string(scene.orbit.peri);
#endif
        
        printText(textOuts, pxRatio, fbWidth, fbHeight);
 
        /* render everything else */
		glEnable(GL_DEPTH_TEST);
        check_gl_error();

        scene.render();
        
        renderTime = glfwGetTime() - t;
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}