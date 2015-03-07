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
    scene.orbit.init();
    check_gl_error();
    grid.init();
    check_gl_error();

    /* loading shaders and render targets */
    //ship.id = shipIdx;
    {
        shipIdx = 0;
        ship.loadShaders("shipVertex.glsl", "shipFragment.glsl");
        
        ship.loadAttribute("position", shapes[shipIdx].mesh.positions, GL_STATIC_DRAW);
        ship.loadAttribute("normal", shapes[shipIdx].mesh.normals, GL_STATIC_DRAW);
        ship.setupBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, shapes[shipIdx].mesh.indices);

    }
    
    
    {
        auto shipIdx = 1;
        sprite.loadShaders("spriteVertex.glsl", "spriteFragment.glsl", true);
        sprite.loadAttribute("position", shapes[shipIdx].mesh.positions, GL_STATIC_DRAW);
        check_gl_error();
        sprite.loadAttribute("normal", shapes[shipIdx].mesh.normals, GL_STATIC_DRAW);
        sprite.setupBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, shapes[shipIdx].mesh.indices);
    }
    
    if (1)
    {
        shipIdx = 2;
        missile.loadShaders("shipVertex.glsl", "shipFragment.glsl");
        missile.loadAttribute("position", shapes[shipIdx].mesh.positions, GL_STATIC_DRAW);
        check_gl_error();
        missile.loadAttribute("normal", shapes[shipIdx].mesh.normals, GL_STATIC_DRAW);
        missile.setupBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, shapes[shipIdx].mesh.indices);
    }
    shipIdx = 0;
    
    // Create and compile our GLSL program from the shaders
    vector<float> v(quad, quad + sizeof quad / sizeof quad[0]);
    quadVAO.setupBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, v);
    
#if true//NOSHADER
    hdr.loadShaders("passthrough.vs", "bloom.fs", true);
    hdr.loadAttribute("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
    
    composite.loadShaders("passthrough.vs", "composite.fs", true);
    composite.loadAttribute("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
    
    fxaa.loadShaders("passthrough.vs", "fxaa.fs", true);
    fxaa.loadAttribute("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);

    highPass.loadShaders("passthrough.vs", "highpass.fs", true);
    highPass.loadAttribute("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
//    glBindVertexArray(quadVAO.vao);
//    highPass.setAttribute("position");
//#else
    glBindVertexArray(quadVAO.vao); //need to bind vao before setAttribute to shader
    highpassShader.init();
//    shadowmapShader.init();
    blurShader.init();
    compositeShader.init();
//    fxaaShader.init();
#endif
    
    blit.loadShaders("passthrough.vs", "passthrough.fs", true);
    blit.loadAttribute("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
    
    {
        shadowMap.depthTexture = true;
        shadowMap.loadShaders("simplePassthrough.vs", "depth.fs", true);
        shadowMap.loadAttribute("position", shapes[shipIdx].mesh.positions, GL_STATIC_DRAW);
        shadowMap.setupBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, shapes[shipIdx].mesh.indices);
    }
    
    //FIXME: some modifications to compositeShader and fxaaShader is causing a cascaded error that results in downSizeFactor getting messed up, causing an incomplete buffer in rt.init
    //compositeShader and fxaaShader can't compile for some reason, most likely not the reason as reported by the debugger
    //something I did this morning has caused the error, think think think...
    //setup hdr and associated rt
    //SOLUTION: just had to clean and rebuild, haha
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
    
    //bind framebuffer object with texture object together
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
        //        hdr.loadAttribute("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);
    }
}

vec3 srcPerspective(0,0,-4);
mat4 depthProjectionMatrix = ortho<float>(-1,1,-1,1,-10,10);
//		mat4 depthProjectionMatrix = ortho<float>(-10,10,-10,10,-10,20);
mat4 depthViewMatrix = lookAt(srcPerspective, vec3(0,0,0), vec3(0,1,0));
mat4 depthMVP;

void Renderer::update()
{
    //maybe this will be useful someday
}

void Renderer::render()
{
    
    /* Set up a blank screen */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    check_gl_error();
    
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
            shadowMap.drawIndexed(world, scene.camera, depthMVP, shapes[shipIdx].mesh.indices.data());
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

            highpassShader.renderPass(quadVAO.vao,
                                rt[myRT].FramebufferName,
                                rt[lastRT].renderedTexture);
        }
        
        if (renderStage & stage4) //first blur
        {
            auto lastRT = myRT++;
            rtForComp[1] = myRT;
            glViewport(0, 0, fbWidth/downSizeFactor, fbHeight/downSizeFactor);

            blurShader.setOffsets(1.0f/fbWidth/4.0, 0);
            blurShader.renderPass(quadVAO.vao,
                                  rt[myRT].FramebufferName,
                                  rt[lastRT].renderedTexture);
        }
        
        if (renderStage & stage5) //second blur
        {
            auto lastRT = myRT++;
            rtForComp[2] = myRT;
            glViewport(0, 0, fbWidth/downSizeFactor, fbHeight/downSizeFactor);

            blurShader.setOffsets(0, 1.0f/fbHeight/4.0);
            blurShader.renderPass(quadVAO.vao,
                                  rt[myRT].FramebufferName,
                                  rt[lastRT-1].renderedTexture);
            
        }
        
        if (renderStage & stage6) //composite
        {
            myRT++;
            glViewport(0, 0, fbWidth/downSizeFactor, fbHeight/downSizeFactor);
#if true//NOSHADER
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
            check_gl_error()
#else
            compositeShader.rtForComp.resize(3);
            for (int i=0; i<3; i++) {
                compositeShader.rtForComp[i] = rtForComp[i];
            }
            compositeShader.renderPass(quadVAO.vao,
                                       rt[myRT].FramebufferName,
                                       rt[rtForComp[0]].renderedTexture,
                                       rt[rtForComp[1]].renderedTexture,
                                       rt[rtForComp[2]].renderedTexture);
#endif
            
        }
        
        if (renderStage & stage7) //fxaa
        {
            auto lastRT = myRT++;
            glViewport(0, 0, fbWidth, fbHeight);
#if true//NOSHADER
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
#else
            fxaaShader.renderPass(quadVAO.vao, rt[myRT].FramebufferName, fbWidth, fbHeight, rt[lastRT].renderedTexture);
#endif
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
    
    auto _camera = scene.camera.matrix();
    
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
        glUseProgram(scene.orbit.shaderProgram);
        scene.orbit.draw(mvp, shipOrbitColor);
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
    int shipOffset = sysIndexOffset[BodyType::SHIP];
    int shipNum = numBodyPerType[BodyType::SHIP];
    //int shipNum = gameLogic.sShip.size();
    for (int i=0; i < shipNum; i++) {
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
        ship.drawIndexed(world, scene.camera, lightPos, mvp, shipColor, shapes[shipIdx].mesh.indices.data());
        check_gl_error();
        //break;
    }
    glUseProgram(sprite.shaderProgram);
    auto drawSelector = [&](int i, vec3 &color)
    {
        if (i < 1 || i >= sys.size()) {
            return;
        }
        auto centralPos = vec3(world * vec4(sys[i].sn.pos, 1.0));
        auto loc = glGetUniformLocation(sprite.shaderProgram, "centralPos");
        glUniform3fv(loc, 1, value_ptr(centralPos));
        sprite.drawIndexed(_camera, color, shapes[0].mesh.indices.data());
    };
    
#if 1
    drawSelector(gameLogic.selected, shipOrbitColor);
    drawSelector(gameLogic.mouseHover, gridColor);
#endif
    
    glUseProgram(globe.shaderProgram);
    for (auto &s : gameLogic.sGlobe) {
        auto mvp = _camera * world * s.transform();
        globe.draw(mvp, planetColor);
        check_gl_error();
    }
    auto mvp = _camera * world;
    globe.draw(mvp, planetColor);
    
    //draw projectile
    int projectileOffset = sysIndexOffset[BodyType::PROJECTILE];
    for (int i=projectileOffset; i < sys.size(); i++)
    {
    glUseProgram(missile.shaderProgram);
        auto mvp =
        world
//        * lookAt(sys[i].sn.vel, vec3(0), vec3(0,1,0))
        * translate(mat4(), sys[i].sn.pos);
        
        missile.drawIndexed(world, scene.camera, lightPos, mvp, shipColor, shapes[2].mesh.indices.data());

        check_gl_error();
    }
    
    
}