//
//  userInputSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/15/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "userInputSystem.h"
#include "componentTypes.h"
#include "ecs.h"
using namespace entityx;

UserInputSystem::UserInputSystem(UserInput *ui) : legacyUserInput(ui)
{
    
}

void UserInputSystem::configure(EventManager& eventManager)
{
//    eventManager.subscribe<PotentialSelectEvent>(*this);
}


void rotate(ComponentHandle<Orientation> orientationHandle,
            float dx, float dy, float dz)
{
    auto yAxis = glm::vec3(1.0f, 0, 0);
    auto xAxis = glm::vec3(0, 1.0f, 0);
    auto zAxis = glm::vec3(0, 0, 1.0f);
    
    orientationHandle->x += dx;
    orientationHandle->y += dy;
    
    auto tempOrientation = glm::rotate(glm::mat4(), -dx, xAxis);
    tempOrientation = glm::rotate(tempOrientation, -dy, yAxis);
    tempOrientation = glm::rotate(tempOrientation, -dz, zAxis);
    orientationHandle->orientation *= tempOrientation;
}

void UserInputSystem::update(entityx::EntityManager &entities,
                             entityx::EventManager &events,
                             double dt,
                             UserInput *legacyUserInput,
                             entityx::Entity myShip,
                             GLFWwindow *window,
                             Camera &camera)
{
    auto selectableEntity = linePick(entities, window, camera);
    
    updateMouseSelection(entities, selectableEntity);
    

    double mx, my;
    static double prevMX = 0, prevMY = 0;
    glfwGetCursorPos(window, &mx, &my);     //consider consolidating this line w/ the one in linePick();
    double _x = mx - prevMX;
    double _y = my - prevMY;
    prevMX = mx;
    prevMY = my;
    double mouseScale = .1;

    if (legacyUserInput->rmbPressed) {
        camera.rotate(_y*mouseScale, _x*mouseScale);
    } else if (legacyUserInput->lmbPressed && not selectableEntity.valid()) {
        //rotate mySHip
//        sShip[0].rotate(-_x*mouseScale, _y*mouseScale, 0.0f);
        assert(myShip.valid());
        auto orientationHandle = myShip.component<Orientation>();
        rotate(orientationHandle, -_x*mouseScale, _y*mouseScale, 0.0f);
    }

    //scroll behavior
    camera.offsetPos(glm::vec3(0,0, -legacyUserInput->yScroll));
    legacyUserInput->yScroll = 0;
    
    //process actions
    //refactor: make its own system, so AI can use it too
    processAction(entities, myShip);
}


/**
 *  only gets called when mouse overs an entity
 */
void UserInputSystem::updateMouseSelection(EntityManager &entities, Entity selectableEntity)
{
    //assuming select one thing at a time
    auto cameraRotateMode = legacyUserInput->rmbPressed;
    auto selectionMode = legacyUserInput->lmbPressed and not cameraRotateMode;
    
    //FIXME: ideally, MB callback will signal event to trigger this function too
    //          in order to detect when a user has clicked anywhere (including nothing)
    //OR don't put any of this in the receive, leave it in linePick, so much easier that way!
    //OR give the MB call back access to selectedEntities as well so it can update dynamically as well (but now we have multiple locations that writes to the same set of structures... :( easy for bugs to creep out!
    
    PlayerControl::Handle player;
    auto count = 0;
    for (Entity entity : entities.entities_with_components(player)) {
        auto &selectedEntities = player->selectedEntities;
        auto &mouseOverEntities = player->mouseOverEntities;
        auto enableMultiSelection = false;
        if (not enableMultiSelection) {
            if (selectionMode) {
                selectedEntities.clear();
            }
            mouseOverEntities.clear();
        }
        
        //placed after clear()'s to allow deselection by mousing over nothing
        if (not selectableEntity.valid()) {
            return;
        }
        
        if (selectionMode) {
            selectedEntities.emplace_back(selectableEntity);
            std::cout << "selected: " << selectableEntity.id() << std::endl;
            
        } else { //else mouseOverMode
            mouseOverEntities.emplace_back(selectableEntity);
            std::cout << "mouseOver: " << selectableEntity.id() << std::endl;
        }
        assert(count++==0);
    }
    //what happens when the cursor moves? is that called in a callback??
}


entityx::Entity UserInputSystem::linePick(EntityManager & entities,
                           GLFWwindow *pWindow,
                           Camera &camera)
{
    //construct mouse/cursor casted ray
    double mouseX, mouseY;
    int screenWidth, screenHeight;
    glfwGetCursorPos(pWindow, &mouseX, &mouseY);
    glfwGetWindowSize(pWindow, &screenWidth, &screenHeight);
    
    //convert coordinate system from [0,1] to [-1, 1] for both x and y
    mouseY = screenHeight - mouseY; //for some reason, mouseY is flipped from tutorial
    auto mouseX_NDC = ((float)mouseX/(float)screenWidth  - 0.5f) * 2.0f;
    auto mouseY_NDC = ((float)mouseY/(float)screenHeight - 0.5f) * 2.0f;
    auto mouse_NDC = glm::vec2(mouseX_NDC * screenWidth, mouseY_NDC * screenHeight);
    
    //for each entity, check if it is within the threshold of clickability,
    //pick the closest entity as a selectable entity
    //emits an event when there is a selectable entity,
    //and on state change from valid to invalid selection
    Position::Handle position;
    float shortestDistance = INFINITY;  //distance between entity and camera
    Entity selectableEntity;
    for (Entity entity : entities.entities_with_components(position))
    {
        
        //convert entity position from 3D to screen NDC space
        auto posNDC = camera.matrix() * world * glm::vec4(position->pos, 1.0);
        posNDC /= posNDC.w;
        auto screenPosNDC = glm::vec2(posNDC.x * screenWidth, posNDC.y * screenHeight);
        auto onScreenDistance = glm::length(mouse_NDC - screenPosNDC);
        
        auto distanceFromCamera = glm::length(position->pos - camera.position);
        
        //FIXME: threshold also depends on aparent size of object
        const int thresholdInPixels = 40;
        
        //pick a clickable entity that is also the closest
        if (onScreenDistance < thresholdInPixels
            && distanceFromCamera < shortestDistance) {
            shortestDistance = distanceFromCamera;
            selectableEntity = entity;
        }
    }
    
    return selectableEntity;
}
/*
 * process list of actions form legacyUserInput
 * future refactor: actionlist should be a standalone thing that both user and AI populate
 */
void UserInputSystem::processAction(entityx::EntityManager &entities, entityx::Entity myShip)
{
    PlayerControl::Handle player;
    auto count = 0;
    vector<Entity> *selectedEntities;
    vector<Entity> *mouseOverEntities;
    for (Entity entity : entities.entities_with_components(player)) {
        selectedEntities = &(player->selectedEntities);
        mouseOverEntities = &(player->mouseOverEntities);
        count++;
    }
    assert(1 == count);
    
    float deltaMove = 2;
    glm::vec3 forwardVector;
    auto hv = myShip.component<Velocity>();
//    auto hp = myShip.component<Position>();
    auto ho = myShip.component<Orientation>();
    for (auto &action : legacyUserInput->actionList )
    {
        switch (action) {
            case ActionType::transForward:
                forwardVector = glm::vec3(ho->orientation * glm::vec4(0, 0, 1, 1));
                hv->vel += glm::normalize(forwardVector) * (float).01;
                break;
            case ActionType::yawLeft:
                ho->orientation = glm::rotate(ho->orientation, deltaMove, glm::vec3(0, 1, 0));
                break;
            case ActionType::yawRight:
                ho->orientation = glm::rotate(ho->orientation, -deltaMove, glm::vec3(0, 1, 0));
                break;
            case ActionType::pitchDown:
                ho->orientation = glm::rotate(ho->orientation, -deltaMove, glm::vec3(1, 0, 0));
                break;
            case ActionType::pitchUp:
                ho->orientation = glm::rotate(ho->orientation, deltaMove, glm::vec3(1, 0, 0));
                break;
            case ActionType::rollCCW:
                ho->orientation = glm::rotate(ho->orientation, -deltaMove, glm::vec3(0, 0, 1));
                break;
            case ActionType::rollCW:
                ho->orientation = glm::rotate(ho->orientation, deltaMove, glm::vec3(0, 0, 1));
                break;
            case ActionType::timeWarpMore:
//                timeWarp *= 2;
                break;
            case ActionType::timeWarpLess:
//                timeWarp /= 2;
                break;
            default:
                break;
        }
        
        
        if (action == ActionType::newShip)
        {
            {
                double m = 7e12;
                double G = 6.673e-11;
                double gm = m * G;
                
                srand ((unsigned int)time(NULL));
                
                auto r = (rand() / 1000) % 400;
                if (r < 150) {
                    r = 150;
                }
                float v = std::sqrt(gm/r);
                glm::vec3 rad(r, 0.0, -0.5);
                glm::vec3 vel(0, v, 0.9);
                cout << "new ship: r: " << r << ", v: " << v << endl;
                m = 1e5;
                gm = m * G;

                auto entity = entities.create();
                assert(entity.valid());
                assert(not entity.has_component<Position>() && "shouldn't have this component already");
                entity.assign<Position>(rad);
                entity.assign<Velocity>(vel);
                entity.assign<GM>(gm);
                entity.assign<Parent>(myShip.component<Parent>()->parent);
                entity.assign<OrbitalBodyType>(BodyType::SHIP);
                entity.assign<Orientation>();
                entity.assign<Radius>(10);
                entity.assign<Ship>();
                entity.assign<OrbitPath>();
            }
        }
        if (action == ActionType::fireGun)
        {
            Entity entity;
            if (selectedEntities->empty()) {
                std::cout << "no target selected\n";
                break;
            }
            auto targetEntity = selectedEntities->front();
            if (targetEntity.valid() ) {//selectedEntities.front().valid()) {
                entity = entities.create();
                entity.assign<MissileLogic>(myShip, selectedEntities->front());
                assert(entity.valid());
            } else {
                break;
            }
//            events.emit<FireWeaponEvent>(myShip);
            double m = 0.0;
            double G = 6.673e-11;
            double gm = m * G;
            auto shipVector = glm::normalize(targetEntity.component<Position>()->pos - myShip.component<Position>()->pos);
            cout << "ship orientation: " << printVec3(shipVector) << "\n";
            auto pos = myShip.component<Position>()->pos
            + (shipVector)
            * 10.0f;
            auto vel = myShip.component<Velocity>()->vel
            + (shipVector)
            * 3.0f;
//            body bullet(state(pos, vel), 10, gm, nullptr, BodyType::SHIP);
//            //            addSatellite(bullet);
//            InsertToSys(bullet, BodyType::MISSILE);
//            //            newEntity(bullet, BodyType::MISSILE);
            assert(entity.valid());
                assert(not entity.has_component<Position>() && "shouldn't have this component already");
            entity.assign<Position>(pos);
            entity.assign<Velocity>(vel);
            entity.assign<GM>(gm);
            entity.assign<Parent>(myShip.component<Parent>()->parent);
            entity.assign<OrbitalBodyType>(BodyType::SHIP);
            entity.assign<Orientation>();
            entity.assign<Radius>(1);
            entity.assign<Missile>();
            entity.assign<OrbitPath>();
        }
        
    }
    legacyUserInput->actionList.clear();
}