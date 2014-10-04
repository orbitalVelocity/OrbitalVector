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

void Scene::init(int width, int height)
{
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
    assert(true == TestLoadObj("terran_corvette.obj"));
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
    auto &gameLogic = *_gameLogic;
    
    /* render meshes */
    auto _camera = camera.matrix();
    world = glm::translate(glm::mat4(), -sys[1].sn.pos);
    
    glm::vec3 planetColor   (0.6, 0.0, 0.0);
    glm::vec3 shipColor     (0.0, 0.7, 0.0);
    glm::vec3 shipOrbitColor(0.4, 0.8, 0.0);
    glm::vec3 gridColor     (0.5, 0.6, 0.6);
    
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
    
    //ship
    glUseProgram(ship.shaderProgram);
    int shipOffset = 1;
    for (int i=0; i < gameLogic.sShip.size(); i++) {
        gameLogic.sShip[i].move(sys[i+shipOffset].sn.pos);
        auto mvp = world
        * gameLogic.sShip[i].transform();
        //                  * glm::rotate(glm::inverse(camera.orientation()),
        //                                180.0f,
        //                                glm::vec3(0,1,0));
        ship.drawIndexed(camera, lightPos, mvp, planetColor, shapes[shipIdx].mesh.indices.data());
        check_gl_error();
    }
    
    {
        glUseProgram(orbit.shaderProgram);
        auto mvp = _camera * world;
        orbit.draw(mvp, shipOrbitColor);
        check_gl_error();
        grid.draw(mvp, gridColor);
        check_gl_error();
    }
}