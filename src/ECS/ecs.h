//
//  ecs.h
//  OrbitalVector
//
//  Created by Si Li on 3/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_ECS_H
#define OrbitalVector_ECS_H
#include "includes.h"
#include "entityx/entityx.h"
#include "componentTypes.h"
#include "cameraComponent.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "input.h"
#include "text.h"
#include "orbit.h"
#include "scene.h"
#include "renderer.h"

#define OLDECS true

const int SHIPOFFSET = 1;
const int MISSILEOFFSET = 3; //only works with 1 grav, 2 ships, and 1 missile

glm::vec3 getShipPos(int index);
glm::vec3 getMissilePos(int index);
glm::vec3 getEntityPosition(int index);
glm::vec3* getMissileVelocityPointer(int index);

glm::vec3 getMyShipPos();
glm::vec3 getMyShipVel();

int getNumberOfShips();
int getNumberOfMissiles();
int getNumberOfEntities();

class GameSingleton : public entityx::EntityX {
    //FIXME: make this private after removing gameLogic/scene class
public:
    entityx::Entity myShip;
    entityx::Entity mainGrav;

    GLFWwindow *pWindow;
    UserInput *legacyUserInput;
//    std::vector<entityx::Entity> selectedEntities;
//    std::vector<entityx::Entity> mouseOverEntities;
    glm::vec3 lightPos;
    
    TextRenderer textObj;
    UserInput userInput;
    Scene scene;
    Renderer renderer;
public:
    explicit GameSingleton(std::string filename);

    void init();//UserInput *ui, TextRenderer *text);
    void load(std::string filename, int width, int height);
    void initCamera(int width, int height);
   
    /**
     * Convenience function for creating a generic orbiting body in ECS
     * @parameter pos glm::vec3 position
     * @parameter vel glm::vec3 velocity
     * @parameter orientation glm::mat4 4x4 matrix for storing orientation
     * @parameter _gm  double GM gravity
     * @parameter r    float radius
     * @parameter _parent entityx::Entity::Id refer to parent gravity well
     * @parameter bt   BodyType enum designating body type
     */
    void loadEntity(entityx::Entity entity,
                    glm::vec3 pos,
                    glm::vec3 vel,
                    glm::mat4 orientation,
                    double _gm,
                    float r,
                    entityx::Entity::Id _parent,
                    BodyType bt);
    
    
    void createRandomShip();
    void createEntity(glm::vec3 pos,
                      glm::vec3 vel,
                      glm::mat4 orientation,
                      double gm,
                      float r,
                      int type);
    
    void createShip(glm::vec3 pos,
                    glm::vec3 vel,
                    glm::mat4 orientation,
                    double gm,
                    float r);
    
    
    //grabs entity from data loaded in load
//    std::vector<entityx::Entity> entity_data();
    
    void update(double dt);

};

//extern GameSingleton myGameSingleton;
#endif
