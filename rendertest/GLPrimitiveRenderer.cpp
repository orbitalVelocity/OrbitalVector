#include "GLPrimitiveRenderer.h"
#include "GLPrimInternalData.h"

#include "LoadShader.h"
#include "GLError.h"
#include <assert.h>


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
"//	vec4 texcolor = vec4(texcolorred.x,texcolorred.x,texcolorred.x,1);\n"
"	vec4 texcolor = vec4(1,1,1,texcolorred.x);\n"
"\n"
"    fragColour = colourV*texcolor;\n"
"}\n";


static unsigned int s_indexData[6] = {0,1,2,0,2,3};
struct vec2
{
	vec2(float x, float y)
	{
		p[0] = x;
		p[1] = y;
	}
	float p[2];
};

struct vec4
{
	vec4(float x,float y, float z, float w)
	{
		p[0] = x;
		p[1] = y;
		p[2] = z;
		p[3] = w;
        
	}
    
	float p[4];
};

typedef struct
{
    vec4 position;
    vec4 colour;
	vec2 uv;
} Vertex;


   


GLPrimitiveRenderer::GLPrimitiveRenderer(int screenWidth, int screenHeight)
:m_screenWidth(screenWidth),
m_screenHeight(screenHeight)
{
    
	m_data = new PrimInternalData;

    m_data->m_shaderProg = gltLoadShaderPair(vertexShader,fragmentShader);
    
    m_data->m_positionUniform = glGetUniformLocation(m_data->m_shaderProg, "p");
    if (m_data->m_positionUniform < 0) {
		assert(0);
	}
	m_data->m_colourAttribute = glGetAttribLocation(m_data->m_shaderProg, "colour");
	if (m_data->m_colourAttribute < 0) {
        assert(0);
    }
	m_data->m_positionAttribute = glGetAttribLocation(m_data->m_shaderProg, "position");
	if (m_data->m_positionAttribute < 0) {
		assert(0);
  	}
	m_data->m_textureAttribute = glGetAttribLocation(m_data->m_shaderProg,"texuv");
	if (m_data->m_textureAttribute < 0) {
		assert(0);
	}

    loadBufferData();
    
}

void GLPrimitiveRenderer::loadBufferData()
{
    
    Vertex vertexData[4] = {
        { vec4(-1, -1, 0.0, 1.0 ), vec4( 1.0, 0.0, 0.0, 1.0 ) ,vec2(0,0)},
        { vec4(-1,  1, 0.0, 1.0 ), vec4( 0.0, 1.0, 0.0, 1.0 ) ,vec2(0,1)},
        { vec4( 1,  1, 0.0, 1.0 ), vec4( 0.0, 0.0, 1.0, 1.0 ) ,vec2(1,1)},
        { vec4( 1, -1, 0.0, 1.0 ), vec4( 1.0, 1.0, 1.0, 1.0 ) ,vec2(1,0)}
    };
    
        
    glGenVertexArrays(1, &m_data->m_vertexArrayObject);
    glBindVertexArray(m_data->m_vertexArrayObject);
    
    glGenBuffers(1, &m_data->m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_data->m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), vertexData, GL_STATIC_DRAW);
    check_gl_error();
    
    
    glGenBuffers(1, &m_data->m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_data->m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,6*sizeof(int), s_indexData,GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(m_data->m_positionAttribute);
    glEnableVertexAttribArray(m_data->m_colourAttribute);
    check_gl_error();
    
	glEnableVertexAttribArray(m_data->m_textureAttribute);
    
    glVertexAttribPointer(m_data->m_positionAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);
    glVertexAttribPointer(m_data->m_colourAttribute  , 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)sizeof(vec4));
    glVertexAttribPointer(m_data->m_textureAttribute , 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)(sizeof(vec4)+sizeof(vec4)));
    check_gl_error();
    
    
    
    
    
    
    glActiveTexture(GL_TEXTURE0);
    
    GLubyte*	image=new GLubyte[256*256*3];
    for(int y=0;y<256;++y)
    {
     //   const int	t=y>>5;
        GLubyte*	pi=image+y*256*3;
        for(int x=0;x<256;++x)
        {
            if (x<y)//x<2||y<2||x>253||y>253)
            {
                pi[0]=255;
                pi[1]=0;
                pi[2]=0;
            } else
			
            {
                pi[0]=255;
                pi[1]=255;
                pi[2]=255;
            }
            
            pi+=3;
        }
    }
    
    glGenTextures(1,(GLuint*)&m_data->m_texturehandle);
    glBindTexture(GL_TEXTURE_2D,m_data->m_texturehandle);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256,256,0,GL_RGB,GL_UNSIGNED_BYTE,image);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    check_gl_error();
    delete[] image;
    
    
}



GLPrimitiveRenderer::~GLPrimitiveRenderer()
{
	glBindTexture(GL_TEXTURE_2D,0);
	glUseProgram(0);
	 glBindTexture(GL_TEXTURE_2D,0);
    glDeleteProgram(m_data->m_shaderProg);
}

void GLPrimitiveRenderer::drawLine()
{

}

void GLPrimitiveRenderer::drawRect(float x0, float y0, float x1, float y1, float color[4])
{
    
    check_gl_error();
	glActiveTexture(GL_TEXTURE0);
    check_gl_error();
	 glBindTexture(GL_TEXTURE_2D,m_data->m_texturehandle);
    check_gl_error();
    
	drawTexturedRect(x0,y0,x1,y1,color,0,0,1,1);
    check_gl_error();
}

void GLPrimitiveRenderer::drawTexturedRect(float x0, float y0, float x1, float y1, float color[4], float u0,float v0, float u1, float v1)//Line()//float from[4], float to[4], float color[4])
{
    glUseProgram(m_data->m_shaderProg);
    check_gl_error();
    
    
    glBindBuffer(GL_ARRAY_BUFFER, m_data->m_vertexBuffer);
    glBindVertexArray(m_data->m_vertexArrayObject);
    
	bool useFiltering = false;
	if (useFiltering)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	   Vertex vertexData[4] = {
        { vec4(-1.+2.*x0/float(m_screenWidth), 1.-2.*y0/float(m_screenHeight), 0, 0 ), vec4( color[0], color[1], color[2], color[3] ) ,vec2(u0,v0)},
        { vec4(-1.+2.*x0/float(m_screenWidth),  1.-2.*y1/float(m_screenHeight), 0, 1 ), vec4( color[0], color[1], color[2], color[3] ) ,vec2(u0,v1)},
        { vec4( -1.+2.*x1/float(m_screenWidth),  1.-2.*y1/float(m_screenHeight), 1, 1 ), vec4(color[0], color[1], color[2], color[3]) ,vec2(u1,v1)},
        { vec4( -1.+2.*x1/float(m_screenWidth), 1.-2.*y0/float(m_screenHeight), 1, 0 ), vec4( color[0], color[1], color[2], color[3] ) ,vec2(u1,v0)}
    };
    
	   glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), vertexData, GL_STATIC_DRAW);



    
    
    
    check_gl_error();
    vec2 p( 0.f,0.f);//?b?0.5f * sinf(timeValue), 0.5f * cosf(timeValue) );
    glUniform2fv(m_data->m_positionUniform, 1, (const GLfloat *)&p);
    
    check_gl_error();
    glEnableVertexAttribArray(m_data->m_positionAttribute);
    check_gl_error();
    glEnableVertexAttribArray(m_data->m_colourAttribute);
    check_gl_error();
	glEnableVertexAttribArray(m_data->m_textureAttribute);
    
    glVertexAttribPointer(m_data->m_positionAttribute, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)0);
    glVertexAttribPointer(m_data->m_colourAttribute  , 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)sizeof(vec4));
    glVertexAttribPointer(m_data->m_textureAttribute , 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *)(sizeof(vec4)+sizeof(vec4)));
    check_gl_error();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_data->m_indexBuffer);
    
    //glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    int indexCount = 6;
    check_gl_error();
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    check_gl_error();
	
    glBindVertexArray(0);
    check_gl_error();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    check_gl_error();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    check_gl_error();
	//glDisableVertexAttribArray(m_data->m_textureAttribute);
    check_gl_error();
	glUseProgram(0);
   
    check_gl_error();
}

void GLPrimitiveRenderer::setScreenSize(int width, int height)
{
	m_screenWidth = width;
	m_screenHeight = height;
    
}