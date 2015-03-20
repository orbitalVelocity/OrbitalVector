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

UserInputSystem::UserInputSystem(UserInput *ui, std::vector<Entity> &s, std::vector<Entity> &h) : legacyUserInput(ui), selectedEntities(s), mouseOverEntities(h)
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
    
    updateMouseSelection(selectableEntity);
    

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
}


/**
 *  only gets called when mouse overs an entity
 */
void UserInputSystem::updateMouseSelection(Entity selectableEntity)
{
    //assuming select one thing at a time
    auto cameraRotateMode = legacyUserInput->rmbPressed;
    auto selectionMode = legacyUserInput->lmbPressed and not cameraRotateMode;
    
    //FIXME: ideally, MB callback will signal event to trigger this function too
    //          in order to detect when a user has clicked anywhere (including nothing)
    //OR don't put any of this in the receive, leave it in linePick, so much easier that way!
    //OR give the MB call back access to selectedEntities as well so it can update dynamically as well (but now we have multiple locations that writes to the same set of structures... :( easy for bugs to creep out!
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
    
    //emit event for clickability
    std::cout << "can select entity " << selectableEntity.id() << std::endl;
    return selectableEntity;
}
/*
 * process list of actions form legacyUserInput
 * future refactor: actionlist should be a standalone thing that both user and AI populate
 */
void UserInputSystem::processAction()
{
    glm::vec3 forwardVector;
    for (auto &action : legacyUserInput->actionList )
    {
        switch (action) {
            case ActionType::transForward:
                //events.emit<EngineBurnEvent>(myShip, thrust);
                break;
            case ActionType::yawLeft:
//                sShip[activeShip].rotate(deltaMove, 0, 0);
                break;
            case ActionType::yawRight:
//                sShip[activeShip].rotate(-deltaMove, 0, 0);
                break;
            case ActionType::pitchDown:
//                sShip[activeShip].rotate(0, -deltaMove, 0);
                break;
            case ActionType::pitchUp:
//                sShip[activeShip].rotate(0, deltaMove, 0);
                break;
            case ActionType::rollCCW:
//                sShip[activeShip].rotate(0, 0, -deltaMove);
                break;
            case ActionType::rollCW:
//                sShip[activeShip].rotate(0, 0, deltaMove);
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
        
//        auto newEntity = [](body body, BodyType type)
//        {
//            myGameSingleton.createEntity(
//                                         body.sn.pos,
//                                         body.sn.vel,
//                                         {},
//                                         body.mu,
//                                         body.radius,
//                                         type);
//        };
        
        if (action == ActionType::newShip)
        {
//            sShip.push_back(Spatial(200.0));    //Spatial constructor inserts body into sys already! and creates a ship in ECS
//            sShip.back().scale(glm::vec3(1));
//            {
//                double m = 7e12;
//                double G = 6.673e-11;
//                double gm = m * G;
//                
//                srand ((unsigned int)time(NULL));
//                
//                r = (rand() / 1000) % 300;
//                if (r < 50) {
//                    r = 50;
//                }
//                float v = std::sqrt(gm/r);
//                glm::vec3 rad(r, 0, 0);
//                glm::vec3 vel(0, 0, v);
//                cout << "new ship: r: " << r << ", v: " << v << endl;
//                m = 1e1;
//                gm = m * G;
//                auto tmp = body(state(rad, vel),
//                                gm,
//                                20,
//                                nullptr,
//                                BodyType::SHIP
//                                );
//               myGameSingleton.createEntity(rad, <#glm::vec3 vel#>, <#glm::mat4 orientation#>, <#double gm#>, <#float r#>, <#int type#>)
//                
//            }
        }
        if (action == ActionType::fireGun)
        {
//            events.emit<FireWeaponEvent>(myShip);
//            double m = 0.0;
//            double G = 6.673e-11;
//            double gm = m * G;
//            auto shipVector = glm::vec3(sShip[activeShip].orientation * glm::vec4(0,0,1,1));
//            cout << "ship orientation: " << printVec3(shipVector) << "\n";
//            auto pos = sys[1].sn.pos
//            + glm::normalize(shipVector)
//            * 10.0f;
//            auto vel = sys[1].sn.vel
//            + glm::normalize(shipVector)
//            * 3.0f;
//            body bullet(state(pos, vel), 10, gm, nullptr, BodyType::SHIP);
//            //            addSatellite(bullet);
//            InsertToSys(bullet, BodyType::MISSILE);
//            //            newEntity(bullet, BodyType::MISSILE);
        }
        
    }
    legacyUserInput->actionList.clear();
}