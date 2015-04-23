//
//  shipSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 4/16/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "shipSystem.h"
#include "componentTypes.h"

float fuelUsed, remainingFuel;
float engineBurn(Ship::Handle ship, float dt)
{
    //check how much fuel
    //TODO: this could be cached after a burn/breakup/dock
    float totalFuel = 0.0;
    for (auto fuelTank : ship->fuelTanks.list())
    {
        totalFuel += fuelTank.fuel;
    }
    //set remaining fuel to max engines
    //assumes all fuel connected to all engines
    remainingFuel = totalFuel;
    auto totalThrust = 0.0;
    for (auto engine : ship->engines.list())
    {
        totalThrust += engine.burn(remainingFuel, dt);
    }
    
    fuelUsed = totalFuel - remainingFuel;
    assert(fuelUsed >= 0);
    
    ship->mass -= fuelUsed; //FIXME: add fuelMass into equation
    
    //update fuel tanks
    auto fuelUsedTemp = fuelUsed;
//    for (auto &fuelTank : ship->fuelTanks.list())
    for (auto i = 0; i < ship->fuelTanks.size() && fuelUsedTemp > 0; i++)
    {
        auto &fuelTank = ship->fuelTanks[i];
        if (fuelTank.fuel >= fuelUsedTemp) {
            fuelTank.fuel -= fuelUsedTemp;
            fuelUsedTemp = 0;
        } else {
            fuelUsedTemp -= fuelTank.fuel;
            fuelTank.fuel = 0;
        }
    }
    assert(fuelUsedTemp == 0);
    return totalThrust;
//    std::cout << "thrust: " << totalThrust
//            << "\naccel: " << acceleration
//            << std::endl;
}

void ShipSystem::update(entityx::EntityManager &entities, entityx::EventManager &events, double dt)
{
    static float dv = 0;

    std::stringstream deltav;
    float acceleration = 0;
    
    Ship::Handle ship;
    Velocity::Handle velocity;
    Orientation::Handle orientation;
    
    for (auto entity : entities.entities_with_components(ship, velocity, orientation))
    {
        (void) entity;
        //do process actions
        //thruster
        if (ship->thrust) {
        std::cout << "thrusting on " << ship->debugName << "\n";
            auto thrust = engineBurn(ship, dt);
            ship->thrust = false;
            auto forwardVector = glm::vec3(orientation->orientation * glm::vec4(0, 0, 1, 1));
//FIXME: store acceleration in ship and push it to the physics system
            //also clear it after use
            acceleration = thrust / ship->mass;
            velocity->vel += glm::normalize(forwardVector) * acceleration;
         
            auto lnMass = log(ship->mass / ship->dryMass);
            dv = ship->engines[0].isp * 9.81 * lnMass;
            
        }
        
        float vel = glm::length(velocity->vel);
        deltav << "velocity: " << to_string_with_precision(vel);
        events.emit<GUITextEvent>(entity, 15.0f, deltav.str());
        deltav.str(std::string());

        deltav << "remaining dv: " << dv;
        events.emit<GUITextEvent>(entity, 15.0f, deltav.str());
    }
    events.emit(DebugEvent(deltav.str()));
    deltav.str(std::string());
    deltav << "fuel used: " << fuelUsed;
    events.emit(DebugEvent(deltav.str()));
    deltav.str(std::string());
    deltav << "available fuel: " << remainingFuel;
    events.emit(DebugEvent(deltav.str()));
    deltav.str(std::string());
    deltav << "acceleration: " << acceleration;
    events.emit(DebugEvent(deltav.str()));
    deltav.str(std::string());
}