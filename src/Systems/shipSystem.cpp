//
//  shipSystem.cpp
//  OrbitalVector
//
//  Created by Si Li on 4/16/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#include "shipSystem.h"
#include "componentTypes.h"

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
    auto availableFuel = totalFuel;
    auto totalThrust = 0.0;
    for (auto engine : ship->engines.list())
    {
        totalThrust += engine.burn(availableFuel, dt);
    }
    
    auto fuelUsed = totalFuel - availableFuel;
    std::cout << "fuel used: " << fuelUsed << std::endl;
    std::cout << "fuel remaining: " << availableFuel << std::endl;

    ship->mass -= fuelUsed; //FIXME: add fuelMass into equation
    
    //update fuel tanks
    for (auto &fuelTank : ship->fuelTanks.list())
    {
        if (fuelUsed > 0) {
            if (fuelTank.fuel >= fuelUsed) {
                fuelTank.fuel -= fuelUsed;
                fuelUsed = 0;
            } else {
                fuelUsed -= fuelTank.fuel;
                fuelTank.fuel = 0;
            }
        }
    }
    
    return totalThrust;
//    std::cout << "thrust: " << totalThrust
//            << "\naccel: " << acceleration
//            << std::endl;
}

void ShipSystem::update(entityx::EntityManager &entities, entityx::EventManager &events, double dt)
{
    Ship::Handle ship;
    Velocity::Handle velocity;
    Orientation::Handle orientation;
    for (auto entity : entities.entities_with_components(ship, velocity, orientation))
    {
        (void) entity;
        //do process actions
        //thruster
        if (ship->thrust) {
            auto thrust = engineBurn(ship, dt);
            ship->thrust = false;
            auto forwardVector = glm::vec3(orientation->orientation * glm::vec4(0, 0, 1, 1));

            auto acceleration = thrust / ship->mass;
            std::cout << "acceleration: " << acceleration << std::endl;
            velocity->vel += glm::normalize(forwardVector) * acceleration;
         
            auto lnMass = log(ship->mass / ship->dryMass);
            auto dv = ship->engines[0].isp /9.81 * lnMass;
            std::cout << "deltaV: " << dv << std::endl;
        }
    }
}