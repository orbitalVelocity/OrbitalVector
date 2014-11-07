//
//  renderer.cpp
//  OrbitalVector
//
//  Created by Si Li on 11/6/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "renderer.h"
#include "tiny_obj_loader.h"
#include "glm/gtx/closest_point.hpp"

using namespace glm;


bool showDepth = false;


#define KERNEL_SIZE   5
float kernel[KERNEL_SIZE * KERNEL_SIZE] =
{
    1, 4, 6, 4, 1,      //16
    4, 16, 24, 16, 4,   //64
    6, 24, 36, 24, 6,   //96
    4, 16, 24, 16, 4,
    1, 4, 6, 4, 1,
};



static const GLfloat quad[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
};

void Renderer::init(int width, int height)
{
    fbWidth = width;
    fbHeight = height;
    auto shipIdx = gameLogic.activeShip;
    lightPos = vec3(0, 0, -1000);
    
    globe.init();
    check_gl_error();
    orbit.init();
    check_gl_error();
    grid.init();
    check_gl_error();

    /* loading shaders and render targets */
    //ship.id = shipIdx;
    {
        ship.loadShaders("shipVertex.glsl", "shipFragment.glsl");
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
    }
    
    
    {
        auto shipIdx = 1;
        sprite.loadShaders("spriteVertex.glsl", "spriteFragment.glsl", true);
        sprite.loadAttrib("position", shapes[shipIdx].mesh.positions, GL_STATIC_DRAW);
        check_gl_error();
        sprite.loadAttrib("normal", shapes[shipIdx].mesh.normals, GL_STATIC_DRAW);
        glBindVertexArray(sprite.vao);
        check_gl_error();
        glGenBuffers(1, &sprite.elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite.elementBuffer);
        check_gl_error();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     shapes[shipIdx].mesh.indices.size() * sizeof(GLuint),
                     shapes[shipIdx].mesh.indices.data(),
                     GL_STATIC_DRAW
                     );
        sprite.drawCount = (int)shapes[shipIdx].mesh.indices.size();
    }
    
    // Create and compile our GLSL program from the shaders
    vector<float> v(quad, quad + sizeof quad / sizeof quad[0]);
    hdr.loadShaders("passthrough.vs", "bloom.fs", true);
    hdr.loadAttrib("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
    composite.loadShaders("passthrough.vs", "composite.fs", true);
    composite.loadAttrib("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
    
    fxaa.loadShaders("passthrough.vs", "fxaa.fs", true);
    fxaa.loadAttrib("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
    
    highPass.loadShaders("passthrough.vs", "highpass.fs", true);
    highPass.loadAttrib("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
    
    blit.loadShaders("passthrough.vs", "passthrough.fs", true);
    blit.loadAttrib("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
    
    {
        shadowMap.depthTexture = true;
        shadowMap.loadShaders("simplePassthrough.vs", "depth.fs", true);
        shadowMap.loadAttrib("position", shapes[shipIdx].mesh.positions, GL_STATIC_DRAW);
        glBindVertexArray(shadowMap.vao);
        glGenBuffers(1, &shadowMap.elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shadowMap.elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     shapes[shipIdx].mesh.indices.size() * sizeof(GLuint),
                     shapes[shipIdx].mesh.indices.data(),
                     GL_STATIC_DRAW
                     );
        shadowMap.drawCount = (int)shapes[shipIdx].mesh.indices.size();
    }
    
    //setup hdr and associated rt
    rt[SN::shadowMap   ].useMipMap = true;
    rt[SN::shadowMap   ].init(fbWidth, fbHeight, true);
    
    rt[SN::comp].useHDR = false;
    rt[SN::fxaa].useHDR = false;
    rt[SN::forward].init(fbWidth, fbHeight);
    for (int i=SN::highPass; i < SN::fxaa; i++) {
        rt[i].useMipMap = true;         //TODO: forward rt doesn't need mipmap
        rt[i].init(fbWidth/downSizeFactor, fbHeight/downSizeFactor);
    }
    rt[SN::fxaa].init(fbWidth, fbHeight);
}

void RenderTarget::init(int fbWidth, int fbHeight, bool depthTexture)
{
    if ( !GLEW_ARB_framebuffer_object ){ // OpenGL 2.1 doesn't require this, 3.1+ does
        assert(false && "no framebuffer object supported, use a texture instead.");
    }
    
    GLint internalFormat = GL_RGB16F;
    GLenum format = GL_RGB;
    auto type = GL_HALF_FLOAT;
    if (not useHDR) {
        internalFormat = GL_RGB8;
        type = GL_UNSIGNED_BYTE;
    }
    if (depthTexture) {
        internalFormat = GL_DEPTH_COMPONENT16;
        format = GL_DEPTH_COMPONENT;
        type = GL_FLOAT;
    }
    //from opengl-tutorial.org tutorial 14: render to target
    
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    // Give an empty image to OpenGL ( the last "0" means "empty" )
    glTexImage2D(GL_TEXTURE_2D, 0,
                 internalFormat,
                 fbWidth,
                 fbHeight,
                 0,
                 format,
                 type,
                 0);
    check_gl_error();
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    if (depthTexture) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        if (not showDepth) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE); //comment out to view depth texture
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
        //        glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderedTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    } else {
        if (useMipMap) {
            glGenerateMipmap(GL_TEXTURE_2D);
            check_gl_error();
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        
        glGenRenderbuffers(1, &depthrenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fbWidth, fbHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
        
        // Set "renderedTexture" as our colour attachement #0
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);
        
        // Set the list of draw buffers.
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
        
    }
    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        assert(false && "framebuffer NOT OK@?");
    
    
}

//runs right between frames -- no shaders are running now
void Renderer::postFrame()
{
    //render done? (may need to take place after swap buffer
    static int reloadCount = 1;
    if (globalReload) {
        globalReload = false;
        cout << "reloading #" << reloadCount++ << "\n";
        //shut down shader program and delete shaders
        glUseProgram(0);
        glDeleteProgram(fxaa.shaderProgram);
        for (auto &shaderID : hdr.shaderIDs)
            glDeleteShader(shaderID);
        
        //reload bloom
        fxaa.loadShaders("passthrough.vs", "fxaa.fs", false);
        //        vector<float> v(quad, quad + sizeof quad / sizeof quad[0]);
        //        hdr.loadAttrib("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
    }
}

vec3 srcPerspective(0,0,-4);
mat4 depthProjectionMatrix = ortho<float>(-1,1,-1,1,-10,10);
//		mat4 depthProjectionMatrix = ortho<float>(-10,10,-10,10,-10,20);
mat4 depthViewMatrix = lookAt(srcPerspective, vec3(0,0,0), vec3(0,1,0));
mat4 depthMVP;

void Renderer::update()
{
    double mx, my;
    static double prevMX = 0, prevMY = 0;
    glfwGetCursorPos(gameLogic.window, &mx, &my);
    double _x = mx - prevMX;
    double _y = my - prevMY;
    prevMX = mx;
    prevMY = my;
    double mouseScale = .1;
    
    if (userInput.rmbPressed) {
        camera.rotate(_y*mouseScale, _x*mouseScale);
    } else if (userInput.lmbPressed) {
        gameLogic.sShip[0].rotate(-_x*mouseScale, _y*mouseScale, 0.0f);
    }
    
    //scroll behavior
    camera.offsetPos(vec3(0,0, -userInput.yScroll));
    userInput.yScroll = 0;
    
    
    
    //calculate trajectories -- FIXME: should go in gamelogic
    static int orbitCount = 0;
    if (orbitCount++ % 30 == 0) {
        orbit.update();
    }
    
    //remove elements
    if (true)
    {
        //mark elements that needs to be removed
        //TODO: due to collision, too far/escape velocity
        vector<bool> markedForRemoval(sys.size(), false);
        markForDeletion(sys, markedForRemoval);
        
        //remove all marked elements
        //FIXME: only works because first element never designed to be removed
        auto it = sys.end() - 1;
        for (int i = (int)sys.size()-1; i >= 0; --i, --it)
        {
            if (markedForRemoval[i]) {      //TODO: wrap sys and ids into 1 object
                sys.erase(it);
            }
        }
    }
    
    
    world = translate(mat4(), -sys[1].sn.pos);
}

void Renderer::render()
{
    if (0 == renderStage) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        forwardRender();
    } else {
        auto shipIdx = gameLogic.activeShip;
        float rtForComp[3] {0,0,0};   //rt indices for comp stage input
        int myRT = -1;
        if (renderStage & stage1) //shadow map rendering
        {
            myRT++;
            glViewport(0, 0, fbWidth/downSizeFactor, fbHeight/downSizeFactor);
            glBindFramebuffer(GL_FRAMEBUFFER, rt[myRT].FramebufferName);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(shadowMap.shaderProgram);
            depthMVP = depthProjectionMatrix * depthViewMatrix *
            gameLogic.sShip[shipIdx].orientation
            * gameLogic.sShip[shipIdx].size;
            shadowMap.drawIndexed(world, camera, depthMVP, shapes[shipIdx].mesh.indices.data());
        }
        //TODO: fix glViewPort toggle in stages too!
        if (renderStage & stage2)  //regular forward rendering
        {
            auto lastRT = myRT++;
            rtForComp[0] = myRT;
            glViewport(0, 0, fbWidth, fbHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, rt[myRT].FramebufferName);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rt[lastRT].renderedTexture);
            forwardRender();
        }
        
        if (renderStage & stage3)  // high pass to get highlights onto rtBloom
        {
            auto lastRT = myRT++;
            glViewport(0, 0, fbWidth/downSizeFactor, fbHeight/downSizeFactor);
            
            glBindFramebuffer(GL_FRAMEBUFFER, rt[myRT].FramebufferName);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rt[lastRT].renderedTexture);
            check_gl_error();
            
            glUseProgram(highPass.shaderProgram);
            auto loc = glGetUniformLocation(highPass.shaderProgram, "renderedTexture");
            glUniform1i(loc, 0);
            
            glBindVertexArray(highPass.vao);
            glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
            check_gl_error();
        }
        
        if (renderStage & stage4) //first blur
        {
            auto lastRT = myRT++;
            rtForComp[1] = myRT;
            glViewport(0, 0, fbWidth/downSizeFactor, fbHeight/downSizeFactor);
            glBindFramebuffer(GL_FRAMEBUFFER, rt[myRT].FramebufferName);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rt[lastRT].renderedTexture);
            glGenerateMipmap(GL_TEXTURE_2D);
            
            glUseProgram(hdr.shaderProgram);
            auto loc = glGetUniformLocation(hdr.shaderProgram, "renderedTexture");
            glUniform1i(loc, 0);
            loc = glGetUniformLocation(hdr.shaderProgram, "offset");
            float offsetx = 0, offsety = 0;
            offsetx = 1.0f / fbWidth / 4.0;
            glUniform2fv(loc, 1, value_ptr(vec2(offsetx, offsety)));
            
            glBindVertexArray(hdr.vao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            check_gl_error();
        }
        
        if (renderStage & stage5) //second blur
        {
            auto lastRT = myRT++;
            rtForComp[2] = myRT;
            glViewport(0, 0, fbWidth/downSizeFactor, fbHeight/downSizeFactor);
            glBindFramebuffer(GL_FRAMEBUFFER, rt[myRT].FramebufferName);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rt[lastRT-1].renderedTexture);
            glGenerateMipmap(GL_TEXTURE_2D);
            
            glUseProgram(hdr.shaderProgram);
            auto loc = glGetUniformLocation(hdr.shaderProgram, "renderedTexture");
            glUniform1i(loc, 0);
            loc = glGetUniformLocation(hdr.shaderProgram, "offset");
            float offsetx = 0, offsety = 0;
            offsety = 1.0f / fbHeight / 4.0;
            glUniform2fv(loc, 1, value_ptr(vec2(offsetx, offsety)));
            
            glBindVertexArray(hdr.vao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            check_gl_error();
        }
        
        if (renderStage & stage6) //composite
        {
            myRT++;
            glViewport(0, 0, fbWidth/downSizeFactor, fbHeight/downSizeFactor);
            glBindFramebuffer(GL_FRAMEBUFFER, rt[myRT].FramebufferName);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rt[rtForComp[0]].renderedTexture);
            
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, rt[rtForComp[1]].renderedTexture);
            
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, rt[rtForComp[2]].renderedTexture);
            
            glUseProgram(composite.shaderProgram);
            auto loc = glGetUniformLocation(composite.shaderProgram, "tex0");
            glUniform1i(loc, 0);
            loc = glGetUniformLocation(composite.shaderProgram, "tex1");
            glUniform1i(loc, 1);
            loc = glGetUniformLocation(composite.shaderProgram, "tex2");
            glUniform1i(loc, 2);
            loc = glGetUniformLocation(composite.shaderProgram, "enable");
            for (auto & r : rtForComp)
                if (r != 0) {
                    r = 1;
                }
            glUniform1fv(loc, 3, rtForComp);
            
            glBindVertexArray(composite.vao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            check_gl_error();
        }
        
        if (renderStage & stage7) //fxaa
        {
            auto lastRT = myRT++;
            glViewport(0, 0, fbWidth, fbHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, rt[myRT].FramebufferName);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rt[lastRT].renderedTexture);
            
            glUseProgram(fxaa.shaderProgram);
            auto loc = glGetUniformLocation(fxaa.shaderProgram, "forwardTexture");
            glUniform1i(loc, 0);
            loc = glGetUniformLocation(fxaa.shaderProgram, "resolution");
            float resolution[2] = {(float)fbWidth, (float)fbHeight};
            glUniform2f(loc, resolution[0], resolution[1]);
            loc = glGetUniformLocation(fxaa.shaderProgram, "showDir");
            glUniform1i(loc, globalShowFXAAAAirection);
            
            glBindVertexArray(fxaa.vao);
            glDisable(GL_BLEND);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glEnable(GL_BLEND);
            check_gl_error();
        }
        
        if (1) //always last stage
        {
            auto lastRT = myRT++;
            //blit back
            glViewport(0, 0, fbWidth, fbHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rt[lastRT].renderedTexture);
            glUseProgram(blit.shaderProgram);
            check_gl_error();
            auto loc = glGetUniformLocation(blit.shaderProgram, "renderedTexture");
            glUniform1i(loc, 0);
            check_gl_error();
            
            glBindVertexArray(blit.vao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            check_gl_error();
            
            return;
        }
        
    }
    
    
}

void Renderer::forwardRender()
{
    auto shipIdx = gameLogic.activeShip;
    
    /* render meshes */
    auto _camera = camera.matrix();
    
    vec3 planetColor   (0.4, 0.0, 0.0);
    vec3 shipColor     (0.9, 0.9, 0.9);
    vec3 shipOrbitColor(0.4, 0.8, 0.0);
    vec3 gridColor     (0.2, 0.21, 0.2);
    
    // orbit and grid
    {
        auto mvp = _camera * world;
        glUseProgram(grid.shaderProgram);
        grid.draw(mvp, gridColor);
        check_gl_error();
        glUseProgram(orbit.shaderProgram);
        orbit.draw(mvp, shipOrbitColor);
        check_gl_error();
    }
    
    //ship
    //add shadow related uniforms
    mat4 biasMatrix(
                    0.5, 0.0, 0.0, 0.0,
                    0.0, 0.5, 0.0, 0.0,
                    0.0, 0.0, 0.5, 0.0,
                    0.5, 0.5, 0.5, 1.0
                    );
    
    glUseProgram(ship.shaderProgram);
    int shipOffset = 1;
    for (int i=0; i < gameLogic.sShip.size(); i++) {
        gameLogic.sShip[i].move(sys[i+shipOffset].sn.pos);
        auto mvp =
        world *
        translate(mat4(), sys[i+shipOffset].sn.pos) *
        gameLogic.sShip[i].orientation
        * gameLogic.sShip[i].size;
        
        auto loc = glGetUniformLocation(ship.shaderProgram, "shadowMap");
        glUniform1i(loc, 0);
        loc = glGetUniformLocation(ship.shaderProgram, "depthBiasMVP");
        
		mat4 depthBiasMVP = biasMatrix * depthMVP;
		glUniformMatrix4fv(loc, 1, GL_FALSE, &depthBiasMVP[0][0]);
        ship.drawIndexed(world, camera, lightPos, mvp, shipColor, shapes[shipIdx].mesh.indices.data());
        check_gl_error();
        //break;
    }
    
    glUseProgram(sprite.shaderProgram);
    //    for (int i=0; i < gameLogic.sShip.size(); i++)
    auto drawSelector = [&](int i, vec3 &color)
    {
        if (i < 0) {
            return;
        }
        gameLogic.sShip[i].move(sys[i+shipOffset].sn.pos);
        auto centralPos = vec3(world * vec4(sys[i+shipOffset].sn.pos, 1.0));
        auto loc = glGetUniformLocation(sprite.shaderProgram, "centralPos");
        glUniform3fv(loc, 1, value_ptr(centralPos));
        sprite.drawIndexed(_camera, color, shapes[shipIdx].mesh.indices.data());
    };
    
    drawSelector(selected-1, shipOrbitColor);
    drawSelector(mouseHover-1, gridColor);
    
    glUseProgram(globe.shaderProgram);
    for (auto &s : gameLogic.sGlobe) {
        auto mvp = _camera * world * s.transform();
        globe.draw(mvp, planetColor);
        check_gl_error();
    }
    auto mvp = _camera * world;
    globe.draw(mvp, planetColor);
    
    int projectileOffset = 1 + (int)gameLogic.sShip.size();
    for (int i=projectileOffset; i < sys.size(); i++)
    {
        auto mvp = _camera
        * world
        * translate(mat4(), sys[i].sn.pos)
        * scale(mat4(), vec3(0.001f));
        //        globe.draw(mvp, planetColor);
#if 1
        shadowMap.drawIndexed(world, camera, mvp, shapes[shipIdx].mesh.indices.data());
#else
        auto loc = glGetUniformLocation(ship.shaderProgram, "shadowMap");
        glUniform1i(loc, 0);
        loc = glGetUniformLocation(ship.shaderProgram, "depthBiasMVP");
		mat4 depthBiasMVP = biasMatrix * depthMVP;
		glUniformMatrix4fv(loc, 1, GL_FALSE, &depthBiasMVP[0][0]);
        ship.drawIndexed(world, camera, lightPos, mvp, shipColor, shapes[shipIdx].mesh.indices.data());
#endif
        check_gl_error();
    }
    
    
}