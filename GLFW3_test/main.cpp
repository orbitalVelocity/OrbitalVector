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
#include "camera.h"
#include "scene.h"
#include "orbit.h"
#include "tiny_obj_loader.h"
#include "spatial.h"

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

glm::vec3 cameraVector;
static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		sys[1].incCustom(.1, cameraVector);
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
		sys[1].incPrograde(-.1);
#if 0
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		blowup = !blowup;
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
    fov = (fov < 10 ) ? 10 : fov;
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
        const char *txt = text.c_str();
        sth_draw_text(stash, droidRegular, size1, leftMargin, dy, txt, &dx, fbWidth,fbHeight);
        dy += size1;
    }
    
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
    glm::vec3 vel(0, 0, 1.3);
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

void initCamera(Camera & camera, int width, int height) {
    camera.setPosition(glm::vec3(0, 0, 10.0f));
    camera.setFocus(glm::vec3(0,3.0f,0));
    camera.setClip(0.01f, 2000.0f);
    camera.setFOV(45.0f);
    camera.setAspectRatio((float)width/(float)height);
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

//  for (size_t i = 0; i < materials.size(); i++) {
//    printf("material[%ld].name = %s\n", i, materials[i].name.c_str());
//    printf("  material.Ka = (%f, %f ,%f)\n", materials[i].ambient[0], materials[i].ambient[1], materials[i].ambient[2]);
//    printf("  material.Kd = (%f, %f ,%f)\n", materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
//    printf("  material.Ks = (%f, %f ,%f)\n", materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
//    printf("  material.Tr = (%f, %f ,%f)\n", materials[i].transmittance[0], materials[i].transmittance[1], materials[i].transmittance[2]);
//    printf("  material.Ke = (%f, %f ,%f)\n", materials[i].emission[0], materials[i].emission[1], materials[i].emission[2]);
//    printf("  material.Ns = %f\n", materials[i].shininess);
//    printf("  material.Ni = %f\n", materials[i].ior);
//    printf("  material.dissolve = %f\n", materials[i].dissolve);
//    printf("  material.illum = %d\n", materials[i].illum);
//    printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
//    printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
//    printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
//    printf("  material.map_Ns = %s\n", materials[i].normal_texname.c_str());
//    std::map<std::string, std::string>::const_iterator it(materials[i].unknown_parameter.begin());
//    std::map<std::string, std::string>::const_iterator itEnd(materials[i].unknown_parameter.end());
//    for (; it != itEnd; it++) {
//      printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
//    }
//    printf("\n");
//  }
}

std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;

static bool
TestLoadObj(
            const char* filename,
            const char* basepath = NULL)
{
    std::cout << "Loading " << filename << std::endl;
    
    std::string err = tinyobj::LoadObj(shapes, materials, filename, basepath);
    
    if (!err.empty()) {
        std::cerr << err << std::endl;
        return false;
    }
    
    PrintInfo(shapes, materials);
    
    return true;
}
int main(int argc, const char * argv[])
{
//    assert(true == TestLoadObj("cornell_box.obj"));
//    assert(true == TestLoadObj("jet.obj"));
//    assert(true == TestLoadObj("cube.obj"));
//    assert(true == TestLoadObj("marker.obj"));
//    return 1;
    
    int width = 800, height = 600;
    GLFWwindow* window = initGraphics(width, height);
    
    initFontStash();
    
    Camera camera;
    initCamera(camera, width, height);
    
    initPhysics();
    Scene scene;
    
    //create Spatial objects for each thing FIXME not done yet
    Spatial sGlobe, sOrbit, sShip;
    
    OGL _globe(GL_TRIANGLES);
    _globe.init();
        check_gl_error();
    Orbit _orbit(GL_LINES);
    _orbit.init();
        check_gl_error();
    OGL grid(GL_LINEAR_ATTENUATION); //just something that's not triangles and lines
    grid.init();
        check_gl_error();
    
    OGL ship(GL_TRIANGLES);
    ship.loadShaders("shipVertex.glsl", "shipFragment.glsl");
//    assert(true == TestLoadObj("cornell_box.obj"));
//    assert(true == TestLoadObj("suzanne.obj"));
    assert(true == TestLoadObj("terran_corvette.obj"));
//    assert(true == TestLoadObj("marker.obj"));
        check_gl_error();

    auto shipIdx = 0;
    ship.loadAttrib("position", shapes[shipIdx].mesh.positions, GL_STATIC_DRAW);
        check_gl_error();
    ship.loadAttrib("normal", shapes[shipIdx].mesh.normals, GL_STATIC_DRAW);
    glBindVertexArray(ship.vao);
        check_gl_error();
    glGenBuffers(1, &ship.elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ship.elementBuffer);
        check_gl_error();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 shapes[shipIdx].mesh.indices.size() * sizeof(GLuint),
                 shapes[shipIdx].mesh.indices.data(),
                 GL_STATIC_DRAW
                 );
    ship.drawCount = (int)shapes[shipIdx].mesh.indices.size();
    
 
//    scene.renderables.push_back(std::move(_globe));
        check_gl_error();
//    scene.renderables.push_back(std::move(_orbit));
        check_gl_error();
//    scene.renderables.push_back(std::move(grid));
//        check_gl_error();
    
    auto &globe = _globe;//scene.renderables[0];
    auto &orbit = _orbit;//scene.renderables[1];
    
    // performance measurement
    glfwSetTime(0);
    auto prevt = glfwGetTime();
    float renderTime = 0.0f;
    auto size = 60;
    RingBuffer<float> fps(size), renderTimes(size);
    
    static float x=0, y=0;
    int winWidth, winHeight;
    int fbWidth, fbHeight;
    /* Loop until the user closes the window */
    glm::mat4 orientation2;
    glm::vec3 cameraGrade;
    glm::mat4 mvp;
    while (!glfwWindowShouldClose(window))
    {
        auto t = glfwGetTime();
        float dt = t - prevt;
        prevt = t;
        fps.push(1.0f/dt);
        renderTimes.push(renderTime*1000.0f);
        orbit.update();
//        scene.renderables[1].update();
    
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
        textOut << "FPS: " << std::setprecision(4) << fps.average() << "";
        pushClear(textOuts, textOut);
        textOut << "render time: " << std::setprecision(4)
                << renderTimes.average() << "ms";
        pushClear(textOuts, textOut);
        textOut << "mouse x: " << x << " y: " << y << " window size " << fbWidth << " x " << fbHeight;
        pushClear(textOuts, textOut);
        textOut << "ship (" << vec3String(sys[1].sn.pos) << ")";
        pushClear(textOuts, textOut);
        textOut << "cameraGrade: " << vec3String(cameraGrade);
        pushClear(textOuts, textOut);
        textOut << "camera pos: " << vec3String(camera.getPosition());
        pushClear(textOuts, textOut);

        // Calculate pixel ratio for hi-dpi devices.
        auto pxRatio = (float)fbWidth / (float)winWidth;
        printText(textOuts, pxRatio, fbWidth, fbHeight);
		
        //render everything else
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
            camera.rotate(_y*mouseScale, _x*mouseScale);
        } else if (lmbPressed) {
            z += _x * mouseScale;
            roll = glm::rotate(glm::mat4(), z, glm::vec3(0,1,0)); //very off, don't know how to fix it though
        }
        //scroll behavior
        camera.setFOV(fov);
        
        auto _camera = camera.matrix();
        world = glm::translate(glm::mat4(), -sys[1].sn.pos);
        
        glm::vec3 planetColor   (0.6, 0.0, 0.0);
        glm::vec3 shipColor     (0.0, 0.7, 0.0);
        glm::vec3 shipOrbitColor(0.4, 0.8, 0.0);
        glm::vec3 gridColor     (0.5, 0.6, 0.6);
        
        //central planet
        sGlobe.move(sys[0].sn.pos);
        sGlobe.scale(glm::vec3(10));
        check_gl_error();
        mvp = _camera * world * sGlobe.transform();
        globe.draw(mvp, planetColor);
        check_gl_error();
       
        //UI
        //prograde
        auto progradeOffset = glm::normalize(sys[1].sn.vel);
        sGlobe.move(sys[1].sn.pos+progradeOffset);
        sGlobe.scale(glm::vec3(.05));
        mvp = _camera * world * sGlobe.transform();
        globe.draw(mvp, planetColor);
        check_gl_error();
        
        //retrograde
        sGlobe.move(sys[1].sn.pos-progradeOffset);
        sGlobe.scale(glm::vec3(.05));
        mvp = _camera * world * sGlobe.transform();
        globe.draw(mvp, planetColor);
        check_gl_error();
        
        //camera grade
        cameraVector = camera.forward();
        cameraVector = glm::normalize(cameraVector);
        cameraVector = glm::rotateX(cameraVector, y);
        cameraVector = glm::rotateZ(cameraVector, -x);
        cameraGrade = glm::vec3(cameraVector);
        sGlobe.move(sys[1].sn.pos+cameraGrade);
        sGlobe.scale(glm::vec3(.05));
        
        mvp = _camera * world * sGlobe.transform();
        globe.draw(mvp, planetColor);
        check_gl_error();
        
        //ship
        glUseProgram(ship.shaderProgram);
        sShip.move(sys[1].sn.pos);
        sShip.scale(glm::vec3(.001));
        auto orientation = glm::orientation(sys[1].sn.vel, glm::vec3(0,1,0));
        sShip.rotate(orientation);
        mvp = camera.matrix() * world * sShip.transform();
        ship.drawIndexed(camera, mvp, planetColor, shapes[shipIdx].mesh.indices.data());
        check_gl_error();
        
        glUseProgram(orbit.shaderProgram);
        mvp = _camera * world;
        orbit.draw(mvp, shipOrbitColor);
        check_gl_error();
        grid.draw(mvp, gridColor);
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