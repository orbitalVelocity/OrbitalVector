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

int main(int argc, const char * argv[])
{
    int width = 800, height = 600;
    GLFWwindow* window = initGraphics(width, height);
    
    initFontStash();
    
    //camera
    glm::mat4 view = glm::lookAt(
                       glm::vec3(0.0f, 5.0f, 0.0f),
                       glm::vec3(0.0f, 0.0f, 0.0f),
                       glm::vec3(0.0f, 0.0f, 1.0f)
    );
    glm::mat4 proj = glm::perspective(45.0f, (float)width / (float)height, 0.10f, 20.0f);
    OGL triangle(view, proj);
    
    
    // performance measurement
    glfwSetTime(0);
    auto prevt = glfwGetTime();
    float renderTime = 0.0f;
    auto size = 10;
    RingBuffer<float> fps(size), renderTimes(size);
    
    //static float x=0, y=0;
    int winWidth, winHeight;
    int fbWidth, fbHeight;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        auto t = glfwGetTime();
        auto dt = t - prevt;
        prevt = t;
        fps.push(1.0f/dt);
        renderTimes.push(renderTime*1000.0f);
    
        //get window size
   		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		glViewport(0, 0, fbWidth, fbHeight);

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
        textOut << "x: " << triangle.x << " y: " << triangle.y << " window size " << fbWidth << " x " << fbHeight;
        pushClear(textOuts, textOut);

        // Calculate pixel ratio for hi-dpi devices.
        auto pxRatio = (float)fbWidth / (float)winWidth;
        printText(textOuts, pxRatio, fbWidth, fbHeight);
		
		glEnable(GL_DEPTH_TEST);
        check_gl_error();

        //render triangle
        double mx, my;
        static double prevMX = 0, prevMY = 0;
    glfwGetCursorPos(window, &mx, &my);
        double _x = mx - prevMX;
        double _y = my - prevMY;
        prevMX = mx;
        prevMY = my;
        
        double mouseScale = .8;
        
        glUseProgram(triangle.shaderProgram);
        if (rmbPressed) {
            triangle.update(_x * mouseScale, _y * mouseScale);
        }
        glm::mat4 camera = proj * view;
        triangle.draw(camera);
       
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