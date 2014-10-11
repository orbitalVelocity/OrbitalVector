//
//  scene.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/14/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "scene.h"
#include "tiny_obj_loader.h"

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
    glm::vec3 com;
    auto &pos = shapes[0].mesh.positions;
    for (int i=0; i< pos.size(); i+=3)
    {
        com += glm::vec3(pos[i+0],
                         pos[i+1],
                         pos[i+2]);
    }
    com /= pos.size();
    for (int i=0; i< pos.size(); i+=3)
    {
        pos[i+0] -= com.x;
        pos[i+1] -= 3*com.y;
        pos[i+2] -= com.z;
    }
    cout << "center of mass in local coord is " << printVec3(com);
    //PrintInfo(shapes, materials);
    return true;
}

void initCamera(Camera & camera, int width, int height) {
    camera.setPosition(glm::vec3(0, 0, 10.0f));
    camera.setFocus(glm::vec3(0,3.0f,0));
    camera.setClip(0.01f, 2000.0f);
    camera.setFOV(45.0f);
    camera.setAspectRatio((float)width/(float)height);
}

#define KERNEL_SIZE   5
float kernel[KERNEL_SIZE * KERNEL_SIZE] =
{
    1, 4, 6, 4, 1,      //16
    4, 16, 24, 16, 4,   //64
    6, 24, 36, 24, 6,   //96
    4, 16, 24, 16, 4,
    1, 4, 6, 4, 1,
};

void Scene::init(int width, int height)
{
    fbWidth = width;
    fbHeight = height;
    shipIdx = 0;
    initCamera(camera, width, height);
    
    lightPos = glm::vec3(0, 0, -1000);
    
    globe.init();
    check_gl_error();
    orbit.init();
    check_gl_error();
    grid.init();
    check_gl_error();
    
    ship.loadShaders("shipVertex.glsl", "shipFragment.glsl");
    //    assert(true == TestLoadObj("cornell_box.obj"));
    //    assert(true == TestLoadObj("suzanne.obj"));
//    assert(true == TestLoadObj("olympus_1mesh.obj"));
//    assert(true == TestLoadObj("terran_corvette.obj"));

    FILE* pFile;
    char outFileName[] = "olympus.bin";
//    char outFileName[] = "terran_corvette.bin";
    vector<int> vecSizes(3);

        shapes.resize(1);
    if (0)
    {
        //write to binary
        pFile = fopen(outFileName, "wb");
        //write size of each vector
        vecSizes[0] = shapes[shipIdx].mesh.positions.size();
        vecSizes[1] = shapes[shipIdx].mesh.normals.size();
        vecSizes[2] = shapes[shipIdx].mesh.indices.size();
                cout << "actual sizes of vectors: " << endl
            << "positions: " << vecSizes[0] << endl
            << "normals:   " << vecSizes[1] << endl
            << "indices:   " << vecSizes[2] << endl;
        fwrite(vecSizes.data(), sizeof(float), 3, pFile);
        
        auto &array = shapes[shipIdx].mesh.positions;
        fwrite(array.data(), 1, array.size()*sizeof(float), pFile);
        auto &array2 = shapes[shipIdx].mesh.normals;
        fwrite(array2.data(), 1, array2.size()*sizeof(float), pFile);
        auto & arrayIdx = shapes[shipIdx].mesh.indices;
        fwrite(arrayIdx.data(), 1, arrayIdx.size()*sizeof(unsigned int), pFile);
        fclose(pFile);
        assert(false);
    }
    
    if (1)
    {
        //read back and compare
        pFile = fopen(outFileName, "r");
        fread((void *)vecSizes.data(), sizeof(int), vecSizes.size(), pFile);
        cout << "sizes of vectors: " << endl
            << "positions: " << vecSizes[0] << endl
            << "normals:   " << vecSizes[1] << endl
            << "indices:   " << vecSizes[2] << endl;
        
        vector<float> fArray1, fArray2;
        vector<unsigned int> iArray1;
        {
            
        auto &fArray = shapes[shipIdx].mesh.positions;
//            auto &fArray = fArray1;
            fArray.resize(vecSizes[0]);
            fread((void *)fArray.data(), sizeof(float), vecSizes[0], pFile);
//            cout << "positions: \n";
//            for(auto &elm : fArray)
//                cout << elm << ",\n";
        }
        {
            auto &fArray = shapes[shipIdx].mesh.normals;
//            auto &fArray = fArray2;
            fArray.resize(vecSizes[1]);
            fread((void *)fArray.data(), sizeof(float), vecSizes[1], pFile);
//            cout << "normals: \n";
//            for(auto &elm : fArray)
//                cout << elm << ",\n";
        }
        auto &iArray = shapes[shipIdx].mesh.indices;
//        auto &iArray = iArray1;
        iArray.resize(vecSizes[2]);
        fread((void *)iArray.data(), sizeof(int), vecSizes[2], pFile);
//        cout << "indices: \n";
//        for(auto &elm : iArray)
//            cout << elm << ",\n";
        
//        assert(std::equal(fArray1.begin(),
//                          fArray1.end(),
//                          shapes[shipIdx].mesh.positions.begin()
//                          )
//               );
//        assert(std::equal(fArray2.begin(),
//                          fArray2.end(),
//                          shapes[shipIdx].mesh.normals.begin()
//                          )
//               );
//        assert(std::equal(iArray1.begin(),
//                          iArray1.end(),
//                          shapes[shipIdx].mesh.indices.begin()
//                          )
//               );
//        cout << "\nsize " << array.size() << endl
//             << "of mismatches: " << mismatch << endl;
//        assert(false);
    }
    //    assert(true == TestLoadObj("marker.obj"));
    check_gl_error();
    
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
    
    rt.init(fbWidth, fbHeight);
    static const GLfloat quad[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };
    
    // Create and compile our GLSL program from the shaders
//    hdr.loadShaders("passthrough.vs", "wobblyTexture.fs", true);
    hdr.loadShaders("passthrough.vs", "bloom.fs", true);
    vector<float> v(quad, quad + sizeof quad / sizeof quad[0]);
    hdr.loadAttrib("position", v, GL_STATIC_DRAW, GL_ARRAY_BUFFER);

}

void RenderTarget::init(int fbWidth, int fbHeight)
{
    //from opengl-tutorial.org tutorial 14: render to target
    
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    // Give an empty image to OpenGL ( the last "0" means "empty" )
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, fbWidth, fbHeight, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
        check_gl_error();
    glGenerateMipmap(GL_TEXTURE_2D);
        check_gl_error();
    
    // Poor filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // The depth buffer
    if ( !GLEW_ARB_framebuffer_object ){ // OpenGL 2.1 doesn't require this, 3.1+ does
        assert(false && "no framebuffer object supported, use a texture instead.");
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
    
    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        assert(false && "framebuffer NOT OK@?");
    

}
void Scene::update()
{
    double mx, my;
    static double prevMX = 0, prevMY = 0;
    glfwGetCursorPos(window, &mx, &my);
    double _x = mx - prevMX;
    double _y = my - prevMY;
    prevMX = mx;
    prevMY = my;
    double mouseScale = .1;
    
    if (_userInput->rmbPressed) {
        camera.rotate(_y*mouseScale, _x*mouseScale);
    } else if (_userInput->lmbPressed) {
        _gameLogic->sShip[0].rotate(_y*mouseScale, _x*mouseScale, 0.0f);
    }
    
    //scroll behavior
    camera.offsetPos(glm::vec3(0,0,-_userInput->yScroll));
    _userInput->yScroll = 0;
    
    
    //calculate trajectories -- FIXME: should go in gamelogic
    static int orbitCount = 0;
    if (orbitCount++ % 30 == 0) {
        orbit.update();
    }
}
void Scene::render()
{
#if 0
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    forwardRender();
#else
    //set to render to custom frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, rt.FramebufferName);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    forwardRender();
    
    
    
//    //filter texture0 
//    glBindFramebuffer(GL_FRAMEBUFFER, rtBloom.FramebufferName);
//    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// 
//    // Bind our texture in Texture Unit 0
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, rt.renderedTexture);
//    glGenerateMipmap(GL_TEXTURE_2D); //FIXME: should remove once done
//    
//    //setup and run hdr program
//    glUseProgram(highPass.shaderProgram);
//    GLuint loc;
//    loc = glGetUniformLocation(highPass.shaderProgram, "renderedTexture");
//    glUniform1i(loc, 0);
//    loc = glGetUniformLocation(highPass.shaderProgram, "kernel");
//    glUniform1fv(loc, KERNEL_SIZE * KERNEL_SIZE, kernel);
//    loc = glGetUniformLocation(highPass.shaderProgram, "frameSize");
//    glUniform2fv(loc, 1, glm::value_ptr(glm::vec2(fbWidth, fbHeight)));
//    
//    glBindVertexArray(highPass.vao);
//    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
//    
    
    
    //now switch to post process/render to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rt.renderedTexture);
    glGenerateMipmap(GL_TEXTURE_2D); //FIXME: should remove once done
    
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, rtBloom.renderedTexture);
//    glGenerateMipmap(GL_TEXTURE_2D);

    //setup and run hdr program
    glUseProgram(hdr.shaderProgram);
    GLuint loc;
    loc = glGetUniformLocation(hdr.shaderProgram, "renderedTexture");
    glUniform1i(loc, 0);
    loc = glGetUniformLocation(hdr.shaderProgram, "kernel");
    glUniform1fv(loc, KERNEL_SIZE * KERNEL_SIZE, kernel);
    loc = glGetUniformLocation(hdr.shaderProgram, "frameSize");
    glUniform2fv(loc, 1, glm::value_ptr(glm::vec2(fbWidth, fbHeight)));
    
    glBindVertexArray(hdr.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
#endif
}

void Scene::forwardRender()
{
    auto &gameLogic = *_gameLogic;
    
    /* render meshes */
    auto _camera = camera.matrix();
    world = glm::translate(glm::mat4(), -sys[1].sn.pos);
    
    glm::vec3 planetColor   (0.4, 0.0, 0.0);
    glm::vec3 shipColor     (0.9, 0.9, 0.9);
    glm::vec3 shipOrbitColor(0.4, 0.8, 0.0);
    glm::vec3 gridColor     (0.5, 0.6, 0.6);

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
    glUseProgram(ship.shaderProgram);
    int shipOffset = 1;
    for (int i=0; i < gameLogic.sShip.size(); i++) {
        gameLogic.sShip[i].move(sys[i+shipOffset].sn.pos);
        auto mvp = gameLogic.sShip[i].orientation
                 * gameLogic.sShip[i].size;
//        auto mvp = gameLogic.sShip[i].transform();
        //                  * glm::rotate(glm::inverse(camera.orientation()),
        //                                180.0f,
        //                                glm::vec3(0,1,0));
        ship.drawIndexed(world, camera, lightPos, mvp, shipColor, shapes[shipIdx].mesh.indices.data());
        check_gl_error();
    }
    
    glUseProgram(globe.shaderProgram);
    for (auto &s : gameLogic.sGlobe) {
        auto mvp = _camera * world * s.transform();
        globe.draw(mvp, planetColor);
        check_gl_error();
    }
    
    auto projectileOffset = 2;
    for (int i=projectileOffset; i < sys.size(); i++)
    {
        auto mvp = _camera
        * world
        * glm::translate(glm::mat4(), sys[i].sn.pos)
        * glm::scale(glm::mat4(), glm::vec3(1.0f));
        globe.draw(mvp, planetColor);
        check_gl_error();
    }
    
    
}
