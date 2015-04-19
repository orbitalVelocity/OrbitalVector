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


void rotate(Orientation::Handle orientationHandle,
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
    

    //update camera, or update shiporientation should be in shipSystem and cameraSystem...
    double mx, my;
    static double prevMX = 0, prevMY = 0;
    glfwGetCursorPos(window, &mx, &my);     //consider consolidating this line w/ the one in linePick();
    double _x = mx - prevMX;
    double _y = my - prevMY;
    prevMX = mx;
    prevMY = my;
    double mouseScale = .005;

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
 *  updates mouse selection states
 */
void UserInputSystem::updateMouseSelection(EntityManager &entities, Entity selectableEntity)
{
    //assuming select one thing at a time
    auto cameraRotateMode = legacyUserInput->rmbPressed;
    auto selectionMode = legacyUserInput->lmbPressed and not cameraRotateMode;
    
//    enableMultiSelection =
    
    PlayerControl::Handle player;
    auto count = 0;
    for (Entity entity : entities.entities_with_components(player)) {
        (void) entity;
        auto &selectedEntities = player->selectedEntities;
        auto &mouseOverEntities = player->mouseOverEntities;
        auto enableMultiSelection = false;
        
        //clear selection in preparation for writing over selection states
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
        
        //move camera, prevent selection from happening if so
        if (legacyUserInput->altPressed and legacyUserInput->lmbPressed) {
            player->newFocus(selectableEntity);
            std::cout << "alt pressed!!!\n";
            continue;
        }
        
        if (selectionMode) {
            selectedEntities.emplace_back(selectableEntity);
//            std::cout << "selected: " << selectableEntity.id() << std::endl;
            
        } else { //else mouseOverMode
            mouseOverEntities.emplace_back(selectableEntity);
//            std::cout << "mouseOver: " << selectableEntity.id() << std::endl;
        }
        assert(count++==0);
     
    }
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
    auto mouse_NDC = glm::vec2(mouseX_NDC , mouseY_NDC);
   
    auto getDistanceToCursor = [&](glm::vec3 pos)
    {
        auto posNDC = camera.matrix() * world * glm::vec4(pos, 1.0);
        posNDC /= posNDC.w;
        auto screenPosNDC = glm::vec2(posNDC.x , posNDC.y);
        return glm::length(mouse_NDC - screenPosNDC);
    };
    
    //for each physics(position, velocity) entity, check if it is within the threshold of clickability,
    //pick the closest entity as a selectable entity
    //emits an event when there is a selectable entity,
    //and on state change from valid to invalid selection
    Position::Handle position;
    Velocity::Handle velocity;
    float shortestDistance = INFINITY;  //distance between entity and camera
    Entity selectableEntity;
    for (Entity entity : entities.entities_with_components(velocity, position))
    {
        auto onScreenDistance = getDistanceToCursor(position->pos);
        
        auto distanceFromCamera = glm::length(position->pos - camera.position);
        
        float thresholdInPixels = 40/900.0; //TODO: value should be in entity
        
        //pick a clickable entity that is also the closest
        if (onScreenDistance < thresholdInPixels
            && distanceFromCamera < shortestDistance) {
            shortestDistance = distanceFromCamera;
            selectableEntity = entity;
        }
    }
    //DEBUG code for barycenters of petal menus////////////////////////
    auto rotate2d = [](float theta)
    {
        float array[4];
        array[0] = cos(theta);
        array[1] = -sin(theta);
        array[2] = sin(theta);
        array[3] = cos(theta);
        glm::mat2 temp = glm::make_mat2(array);
        return temp;
    };
    //FIXME: ideally this would go in the menu class
    GUICircleMenu::Handle circle;
    for (Entity entity : entities.entities_with_components(circle))
    {
        (void) entity;
        //takes a 3D position, converts it to NDC, then adds leaf offset,
        //then compares against mouseNDC;
        
        //when checking vertices of leaf it self, it should already be in NDC (no aspect ratio adjustments though, but maybe don't need to since not going through camera projection transformation
        //so rotate leaf vertices (instead of the bary center)
        //and compare rotated vertices(RV) against bary center,
        //if any RV-mouse is more than 90 degrees off from mouse - barycenter
        auto position = circle->target.component<Position>();
       
        //check if mouse hits any elements in circle
        for (auto &leaf: circle->leafMenus)
        {
            //barycenter is offset already
            auto petalCenter = barycenters[5];
            auto offset2d = rotate2d(leaf.rotateByRadian) * glm::vec2(petalCenter);
            
            offset2d *= glm::vec2(1, 1.6); //adjust for aspect ratio
            
            auto pos = position->pos;
            auto posNDC = camera.matrix() * world * glm::vec4(pos, 1.0);
            posNDC /= posNDC.w;
            
            posNDC.x += offset2d.x; //CRUCIAL!
            posNDC.y += offset2d.y; //CRUCIAL!
            
            auto screenPosNDC = glm::vec2(posNDC.x, posNDC.y);
            auto onScreenDistance = glm::length(mouse_NDC - screenPosNDC);
            
        
            //assumes circular hit target
            if (onScreenDistance < .05) {
                leaf.hover = true;
            } else {
                leaf.hover = false;
            }
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
    vector<Entity> *selectedEntities = nullptr;
    vector<Entity> *mouseOverEntities = nullptr;
    for (Entity entity : entities.entities_with_components(player)) {
        (void) entity;
        selectedEntities = &(player->selectedEntities);
        mouseOverEntities = &(player->mouseOverEntities);
        count++;
    }
    assert(1 == count);
  
    auto getThisMenu = [&](entityx::Entity selection)
    {
        GUICircleMenu::Handle menu;
        for (Entity entity : entities.entities_with_components(menu)) {
            if (menu->target.id() == selection.id()) {
                return entity.id();
            }
        }
        auto temp = entityx::Entity::INVALID;
        return temp;
    };

    float deltaMove = .1;
    glm::vec3 forwardVector;
    auto hv = myShip.component<Velocity>();
//    auto hp = myShip.component<Position>();
    auto ho = myShip.component<Orientation>();
    auto hs = myShip.component<Ship>();
    for (auto &action : legacyUserInput->actionList )
    {
        switch (action) {
            case ActionType::transForward:
//                forwardVector = glm::vec3(ho->orientation * glm::vec4(0, 0, 1, 1));
//                hv->vel += glm::normalize(forwardVector) * (float).01;
                hs->thrust = true;
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
                legacyUserInput->timeWarp *= 2;
                break;
            case ActionType::timeWarpLess:
                legacyUserInput->timeWarp /= 2;
                break;
            case ActionType::spawnMenu:
                //check if anything selected and no other menu for this has been created
                if (selectedEntities->size() == 1)
                {
                    auto selection = selectedEntities->front();
                    auto preexistingMenu = getThisMenu(selection);
                    if (entityx::Entity::INVALID != preexistingMenu)
                    {
                        entities.get(preexistingMenu).destroy();
                        //TODO: set entity to destroy state (w/ animation)
                    } else {
                        auto entity = entities.create();
                        entity.assign<GUICircleMenu>(selectedEntities->front(), 4);
                        std::cout << "new menu spawned!\n";
                    }
                }
                //then create menu
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
            if (selectedEntities->empty()
                ) {
                std::cout << "no target selected\n";
                break;
            }
            auto targetEntity = selectedEntities->front();
            if (targetEntity.valid()
                and targetEntity.id() != myShip.id()) {
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
