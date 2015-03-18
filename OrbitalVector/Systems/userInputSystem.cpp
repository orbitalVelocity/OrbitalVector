//
//  userInputSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 3/15/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "userInputSystem.h"
#include "componentTypes.h"

using namespace entityx;

UserInputSystem::UserInputSystem(UserInput *ui, std::vector<Entity> &s) : legacyUserInput(ui), selectedEntities(s)
{
    
}

void UserInputSystem::configure(EventManager& eventManager)
{
    eventManager.subscribe<PotentialSelectEvent>(*this);
}

/**
 *  only gets called when a mouse hovers over an entity
 */
void UserInputSystem::receive(const PotentialSelectEvent& e)
{
    potentiallySelectedEntity = e.entity;
    assert(potentiallySelectedEntity.valid());
    
    //assuming select one thing at a time
    auto cameraRotateMode = legacyUserInput->rmbPressed;
    auto selectionMode = legacyUserInput->lmbPressed and not cameraRotateMode;
    
    auto enableMultiSelection = false;
    if (not enableMultiSelection) {
        if (selectionMode) {
            selectedEntities.clear();
        }
        mouseOverEntities.clear();
    }
    
    if (selectionMode) {
        selectedEntities.emplace_back(potentiallySelectedEntity);
        std::cout << "selected: " << potentiallySelectedEntity.id() << std::endl;
        
    } else { //else hovermode
        mouseOverEntities.emplace_back(potentiallySelectedEntity);
        std::cout << "hovered: " << potentiallySelectedEntity.id() << std::endl;
    }

    //what happens when the cursor moves? is that called in a callback??
}


void UserInputSystem::update(entityx::EntityManager &entities,
                             entityx::EventManager &events,
                             double dt,
                             UserInput *legacyUserInput,
                             entityx::Entity myShip)

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
        }
        if (action == ActionType::fireGun)
        {
            events.emit<FireWeaponEvent>(myShip);
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
//            InsertToSys(bullet, BodyType::PROJECTILE);
//            //            newEntity(bullet, BodyType::PROJECTILE);
        }
        
    }
    legacyUserInput->actionList.clear();
}
