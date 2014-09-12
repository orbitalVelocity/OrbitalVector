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

#include "../fontstash/fontstash.h"
#include "../fontstash/opengl_fontstashcallbacks.h"
#include "../rendertest/LoadShader.h"
#include "../rendertest/GLPrimInternalData.h"

#define CUSTOM_VSYNC 2
#define VSYNC 1

using namespace std;

static PrimInternalData sData;
glm::mat4 camera;

void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

void loadShader();
unsigned int indexData[6] = {0,1,2,0,2,3};

void loadBufferData(){
    
    Vertex vertexData[4] = {
        { vec4(-0.5, -0.5, 0.0, 1.0 ), vec4( 1.0, 1.0, 1.0, 1.0 ) ,vec2(0.0078125,0.015625)},
        { vec4(-0.5,  0.5, 0.0, 1.0 ), vec4( 1.0, 1.0, 1.0, 1.0 ) ,vec2(0.101562,0.015625)},
        { vec4( 0.5,  0.5, 0.0, 1.0 ), vec4( 1.0, 1.0, 1.0, 1.0 ) ,vec2(0.101562,0.105469)},
        { vec4( 0.5, -0.5, 0.0, 1.0 ), vec4( 1.0, 1.0, 1.0, 1.0 ) ,vec2(0.0078125,0.105469)}
    };
    
    
    
	glGenVertexArrays(1, &sData.m_vertexArrayObject);
    glBindVertexArray(sData.m_vertexArrayObject);
    
    glGenBuffers(1, &sData.m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sData.m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), vertexData, GL_STATIC_DRAW);
    GLuint err = glGetError();
    assert(err==GL_NO_ERROR);
    
    
    
    glGenBuffers(1, &sData.m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sData.m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,6*sizeof(int), indexData,GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(sData.m_positionAttribute);
    glEnableVertexAttribArray(sData.m_colourAttribute);
	err = glGetError();
    assert(err==GL_NO_ERROR);
    
	glEnableVertexAttribArray(sData.m_textureAttribute);
    
    glVertexAttribPointer(sData.m_positionAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);
    glVertexAttribPointer(sData.m_colourAttribute  , 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)sizeof(vec4));
    glVertexAttribPointer(sData.m_textureAttribute , 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)(sizeof(vec4)+sizeof(vec4)));
	err = glGetError();
    assert(err==GL_NO_ERROR);
    
}

void initTestTexture()
{
    //	glEnable(GL_TEXTURE_2D);
	glGenTextures(1,(GLuint*)&sData.m_texturehandle);
	
    GLint err = glGetError();
    assert(err==GL_NO_ERROR);
    
    glBindTexture(GL_TEXTURE_2D,sData.m_texturehandle);
    
    err = glGetError();
    assert(err==GL_NO_ERROR);
    
	int width=256;
	int height=256;
	unsigned char* image = (unsigned char*)malloc(width*height);
	memset(image,0,width*height);
	for (int i=0;i<width;i++)
	{
		for (int j=0;j<height;j++)
		{
			if (i==j)
				image[i+width*j]=0;
			else
				image[i+width*j]=255;
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width,height,0,GL_RED,GL_UNSIGNED_BYTE,image);
	
    err = glGetError();
    assert(err==GL_NO_ERROR);
    
    glGenerateMipmap(GL_TEXTURE_2D);
	
    err = glGetError();
    assert(err==GL_NO_ERROR);
    
    free(image);
    
}


static const char* vertexShader= \
"#version 150   \n"
"\n"
"uniform vec2 p;\n"
"\n"
"in vec4 position;\n"
"in vec4 colour;\n"
"out vec4 colourV;\n"
"\n"
"in vec2 texuv;\n"
"out vec2 texuvV;\n"
"\n"
"\n"
"void main (void)\n"
"{\n"
"    colourV = colour;\n"
"	gl_Position = vec4(p.x+position.x, p.y+position.y,0.f,1.f);\n"
"	texuvV=texuv;\n"
"}\n";

static const char* fragmentShader= \
"#version 150\n"
"\n"
"in vec4 colourV;\n"
"out vec4 fragColour;\n"
"in vec2 texuvV;\n"
"\n"
"uniform sampler2D Diffuse;\n"
"\n"
"void main(void)\n"
"{\n"
"	vec4 texcolorred = texture(Diffuse,texuvV);\n"
"//	vec4 texcolor = vec4(texcolorred.x,texcolorred.x,texcolorred.x,texcolorred.x);\n"
"	vec4 texcolor = vec4(1,1,1,texcolorred.x);\n"
"\n"
"    fragColour = colourV*texcolor;\n"
"}\n";


void loadShader(){
	sData.m_shaderProg= gltLoadShaderPair(vertexShader,fragmentShader);
    
    sData.m_positionUniform = glGetUniformLocation(sData.m_shaderProg, "p");
    if (sData.m_positionUniform < 0) {
		assert(0);
	}
	sData.m_colourAttribute = glGetAttribLocation(sData.m_shaderProg, "colour");
	if (sData.m_colourAttribute < 0) {
        assert(0);
    }
	sData.m_positionAttribute = glGetAttribLocation(sData.m_shaderProg, "position");
	if (sData.m_positionAttribute < 0) {
		assert(0);
  	}
	sData.m_textureAttribute = glGetAttribLocation(sData.m_shaderProg,"texuv");
	if (sData.m_textureAttribute < 0) {
		assert(0);
	}
    
}

void display() {
    
    glUseProgram(sData.m_shaderProg);
    glBindBuffer(GL_ARRAY_BUFFER, sData.m_vertexBuffer);
    glBindVertexArray(sData.m_vertexArrayObject);
    
    GLuint err = glGetError();
    assert(err==GL_NO_ERROR);
    
    vec2 p( 0.f,0.f);//?b?0.5f * sinf(timeValue), 0.5f * cosf(timeValue) );
    glUniform2fv(sData.m_positionUniform, 1, (const GLfloat *)&p);
    
    glEnableVertexAttribArray(sData.m_positionAttribute);
    glEnableVertexAttribArray(sData.m_colourAttribute);
	glEnableVertexAttribArray(sData.m_textureAttribute);
    
    glVertexAttribPointer(sData.m_positionAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);
    glVertexAttribPointer(sData.m_colourAttribute  , 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)sizeof(vec4));
    glVertexAttribPointer(sData.m_textureAttribute , 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)(sizeof(vec4)+sizeof(vec4)));
	err = glGetError();
    assert(err==GL_NO_ERROR);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sData.m_indexBuffer);
    
    err = glGetError();
    assert(err==GL_NO_ERROR);
    
}

struct sth_stash* stash = 0;
	int droidRegular, droidItalic, droidBold, droidJapanese;
void initFontStash()
{
    loadShader();
    loadBufferData();
    initTestTexture();
    
    
	int fontTextureWidth = 512;
	int fontTextureHeight = 512;
	SimpleOpenGL2RenderCallbacks* renderCallbacks = new SimpleOpenGL2RenderCallbacks(&sData);
    
	stash = sth_create(fontTextureWidth,fontTextureHeight,renderCallbacks);
    
	if (!stash)
	{
		fprintf(stderr, "Could not create stash.\n");
		assert(false);
	}
    
	// Load the first truetype font from memory (just because we can).
#ifdef _WIN32
    const char* fontPath = "../../bin/";
#else
    const char* fontPath = "./";
#endif
    
    char fullFontFileName[1024];
    sprintf(fullFontFileName,"%s%s",fontPath,"DroidSerif-Regular.ttf");
    
	droidRegular = sth_add_font(stash,fullFontFileName);
    sprintf(fullFontFileName,"%s%s",fontPath,"DroidSerif-Italic.ttf");
	droidItalic = sth_add_font(stash,fullFontFileName);
    sprintf(fullFontFileName,"%s%s",fontPath,"DroidSerif-Bold.ttf");
	droidBold = sth_add_font(stash,fullFontFileName);
    sprintf(fullFontFileName,"%s%s",fontPath,"DroidSansJapanese.ttf");
    droidJapanese = sth_add_font(stash,fullFontFileName);
    
    //end of font init stuff

}

static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
#if 0
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		blowup = !blowup;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		screenshot = 1;
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		premult = !premult;
#endif
}

bool lmbPressed, rmbPressed;
static void mb(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            rmbPressed = true;
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            lmbPressed = true;
        }
    } else {
        rmbPressed = false;
        lmbPressed = false;
    }
}

float yScroll, xScroll;
float fov = 45;
static void scroll(GLFWwindow* window, double xoffset, double yoffset)
{
    yScroll = yoffset;
    fov -= yScroll;
    fov = (fov < 45 ) ? 45 : fov;
    fov = (fov > 120) ? 120 : fov;
    xScroll = xoffset;
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
   	
	glfwSetKeyCallback(window, key);
    glfwSetMouseButtonCallback(window, mb);
    glfwSetScrollCallback(window, scroll);
    
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

template <class T>
class RingBuffer{
public:
    RingBuffer(int size)
    {
        data.resize(size, 0);
        index = 0;
        _average = 0;
    }
    void push(T v)
    {
        data[index++%data.size()] = v;
    }
    float average()
    {
        if (index % data.size() == 0) {
            _average = (float)std::accumulate(data.begin(), data.end(), 0.0f)/(float)data.size();
        }
        return _average;
    }
    
    int index = 0;
    float _average;
    vector<T> data;
};

void printText(vector<string> texts, int pxRatio, int fbWidth, int fbHeight)
{
	float sx,sy,dx,dy;
   
    sth_begin_draw(stash);
    
    display();
    
    sx = 0; sy = fbHeight;
    dx = sx; dy = sy;
    float size1 = 20.0f * pxRatio;
    float leftMargin = 10 * pxRatio;
    dy = 0;
 
    for (auto &text : texts)
    {
        //sprintf(txt,"%d. The quick brown fox jumper over the lazy dog. 1234567890",i);
        const char *txt = text.c_str();
        sth_draw_text(stash, droidRegular, size1, leftMargin, dy, txt, &dx, fbWidth,fbHeight);
        dy += size1;
    }
#if 0
        const char *txt3 = textOut2.str().c_str();
        sth_draw_text(stash, droidRegular, size1, leftMargin, dy, txt3, &dx, fbWidth,fbHeight);
        dy +=40 * pxRatio;
        const char *txt4 = textOut3.str().c_str();
        sth_draw_text(stash, droidRegular, size1, leftMargin, dy, txt4, &dx, fbWidth,fbHeight);
        dy +=40 * pxRatio;
        const char *txt2 = textFPS.str().c_str();
        sth_draw_text(stash, droidRegular, size2, leftMargin, dy, txt2, &dx, fbWidth,fbHeight);
        dy +=20 * pxRatio;
    }
#endif
    
    //sth_flush_draw(stash);
    
    sth_end_draw(stash);
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
    glm::vec3 vel(0, 1.3, 0);
    m = 1e-10;
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

string vec3String(glm::vec3 v)
{
    stringstream printMe;
    printMe << to_string(v.x) << "," << to_string(v.y) << "," << to_string(v.z);
    return printMe.str();
}
int main(int argc, const char * argv[])
{
    int width = 800, height = 600;
    GLFWwindow* window = initGraphics(width, height);
    
    initFontStash();
    
    //camera
    glm::mat4 view = glm::lookAt(
                       glm::vec3(0.0f, 1.0f, 0.0f),
                       glm::vec3(0.0f, 0.0f, 0.0f),
                       glm::vec3(0.0f, 0.0f, 1.0f)
    );
    glm::mat4 proj = glm::perspective(45.0f, (float)width / (float)height, 0.01f, 2000.0f);
    initPhysics();
    OGL globe(GL_TRIANGLES);
        check_gl_error();
    OGL orbit(GL_LINES);
        check_gl_error();
    
    // performance measurement
    glfwSetTime(0);
    auto prevt = glfwGetTime();
    float renderTime = 0.0f;
    auto size = 10;
    RingBuffer<float> fps(size), renderTimes(size);
    
    static float x=0, y=0;
    int winWidth, winHeight;
    int fbWidth, fbHeight;
    /* Loop until the user closes the window */
    glm::mat4 orientation2;
    while (!glfwWindowShouldClose(window))
    {
        auto t = glfwGetTime();
        float dt = t - prevt;
        prevt = t;
        fps.push(1.0f/dt);
        renderTimes.push(renderTime*1000.0f);
    
        //get window size
   		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		glViewport(0, 0, fbWidth, fbHeight);

        
        //physics
        orbitDelta(dt, ks, sys, false);
        
        /* Set up a blank screen */
        glClearColor(0.5,0.5,0.5,1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        check_gl_error();
        
        //render text
        auto pushClear = [](vector<string> &vec, stringstream & ss)
        {
            vec.push_back(ss.str());
            ss.str(string());
            ss.clear();
        };
        vector<string> textOuts;
        stringstream textOut;
        textOut << "font frame time: " << std::setprecision(4) << dt*1000 << "ms";
        pushClear(textOuts, textOut);
        textOut << "FPS: " << std::setprecision(4) << fps.average() << "";
        pushClear(textOuts, textOut);
        textOut << "render time: " << std::setprecision(4)
                << renderTimes.average() << "ms";
        pushClear(textOuts, textOut);
        textOut << "mouse x: " << x << " y: " << y << " window size " << fbWidth << " x " << fbHeight;
        pushClear(textOuts, textOut);
        textOut << "ship (" << vec3String(sys[1].sn.pos) << ")";
        pushClear(textOuts, textOut);
        textOut << "scroll: " << yScroll << ", " << xScroll;
        pushClear(textOuts, textOut);

        // Calculate pixel ratio for hi-dpi devices.
        auto pxRatio = (float)fbWidth / (float)winWidth;
        printText(textOuts, pxRatio, fbWidth, fbHeight);
		
		glEnable(GL_DEPTH_TEST);
        check_gl_error();

        //render meshes
        double mx, my;
        static double prevMX = 0, prevMY = 0;
        glfwGetCursorPos(window, &mx, &my);
        double _x = mx - prevMX;
        double _y = my - prevMY;
        prevMX = mx;
        prevMY = my;
        static float z = 0;
        double mouseScale = .1;
        
        glUseProgram(globe.shaderProgram);
        static glm::mat4 roll;
        if (rmbPressed) {
            glm::mat4 orientation = glm::mat4();
            //globe.update(_x * mouseScale, _y * mouseScale);
            y += _y * mouseScale;
            x += _x * mouseScale;
            x = (x > 360) ? x - 360 : x;
            y = (y > 90) ? 90 : y;
            y = (y < -90) ? -90 : y;
            orientation = glm::rotate(orientation, -y, glm::vec3(1,0,0));
            orientation2 = glm::rotate(orientation, x, glm::vec3(0,0,1));
        } else if (lmbPressed) {
            z += _x * mouseScale;
            roll = glm::rotate(glm::mat4(), z, glm::vec3(0,1,0)); //very off, don't know how to fix it though
            
        }
        //scroll behavior
        proj = glm::perspective(fov, (float)width / (float)height, 0.01f, 2000.0f);
        //putting it all together
        glm::mat4 orientation3 = glm::translate(orientation2*roll, -sys[1].sn.pos);
        camera = proj * view * orientation3;
        
        glm::vec3 planetColor   (0.6, 0.0, 0.0);
        glm::vec3 shipColor     (0.0, 0.7, 0.0);
        glm::vec3 shipOrbitColor(0.4, 0.8, 0.0);
        
        globe.move(sys[0].sn.pos);
        globe.scale(glm::vec3(10));
        globe.draw(camera, planetColor);
        globe.move(sys[1].sn.pos);
        globe.scale(glm::vec3(.1,.1,.1));
        globe.draw(camera, shipColor);
       
        glUseProgram(orbit.shaderProgram);
        orbit.draw(camera, shipOrbitColor);
        
        check_gl_error();
        
        renderTime = glfwGetTime() - t;
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}