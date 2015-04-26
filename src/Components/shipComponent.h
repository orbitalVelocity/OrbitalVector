//
//  shipComponent.h
//  OrbitalVector
//
//  Created by Si Li on 4/26/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__shipComponent__
#define __OrbitalVector__shipComponent__

#include <stdio.h>
#include <vector>
#include "entityx/Entity.h"
#include "componentTypes.h"

struct Part
{
    Part(int i) : uid(i) {}
    int uid;
};


struct Sensor : public Part
{
    Sensor(int i) : Part(i) {}
    float sensitivity; //range [0-1)
    float powerDemand;
    float heatOutput;
    float heat; //heat += heatOutput * powerDemand
};

struct Engine : public Part
{
    Engine(int i) : Part(i) {}
    float fuelFlowRate;
    float isp;
    float mass;
    float throttle; //range [0, 1)
    float heat;     // heat += throttle * heatOutput //heat is total amount of heat in some units
    float heatOutput; //rate of heat production
    //required fuel types
    
    //TODO: position relative to ship, orientation
    //FIXME: if engine becomes a component, there should be a system that handles this component specifically...
    float burn(float &availableFuel, float dt)
    {
        heat += throttle * heatOutput;
        //find required fuel for this dt
        auto requiredFuel = throttle * fuelFlowRate * dt;
        auto fuelUsed = (availableFuel >= requiredFuel) ? requiredFuel : availableFuel;
        
        //update available fuel
        availableFuel -= fuelUsed;
        
        //thrust is lowered if less fuel available than required
        return throttle * fuelFlowRate * isp * 9.81 * (fuelUsed/requiredFuel);//thrust isn't dependent on frame rate, this should be in the velocity calculation in shipSystem * dt;
        
        //from Tan Zu
        //ThrustForce = ISP * 9.81 * fuel flow rate * throttle
        //fuel-rate = thrust / ISP / grav
        //impulse is total energy of burn across dt
        //impulse = avg thrust * dt
        //ISP = func(thrust)
    }
};

enum class FuelType
{
    invalid,
    petro,
    hydrogen,
    oxygen,
    fissionable,
    fusionable,
    xeon,
    antimatter
};
struct FuelTank : public Part
{
    FuelTank(int i) : Part(i) {}
    
    float capacity;
    float fuel;
    FuelType fuelType;
    float fuelMass;
    float dryMass;
};

struct Physics
{
    float totalMass;
};

template<typename T>
class PartList
{
    std::vector<T> _list;
    int idCounter = 0;
public:
    PartList() {}
    int getID()
    {
        return idCounter++;
    }
    void push_back(T part)
    {
        _list.push_back(part);
    }
    void erase(int i)
    {
        assert(i < _list.size() && i >= 0);
        _list.erase(_list.begin()+i);
    }
    T operator [](int i) const
    {
        assert(i < _list.size() && i >= 0);
        return _list[i];
    }
    T& operator [](int i)
    {
        assert(i < _list.size() && i >= 0);
        return _list[i];
    }
    std::vector<T>& list()
    {
        return _list;
    }
    int size()
    {
        return (int)_list.size();
    }
    
};

COMPONENT(Ship)
{
    Ship()
    {
        debugName = "ship " + std::to_string(instanceCount);
        instanceCount++;
        
        //testing
//        FuelTank hydrogenTank(fuelTanks.getID());
//        hydrogenTank.dryMass = 10;
//        hydrogenTank.fuelMass = 1;
//        hydrogenTank.fuel = 100;
//        hydrogenTank.capacity = 100;
//        hydrogenTank.fuelType = FuelType::hydrogen;
//        
//        fuelTanks.push_back(hydrogenTank);
        
        FuelTank oxygenTank(fuelTanks.getID());
        oxygenTank.dryMass = 10;
        oxygenTank.fuelMass = 1.6;
        oxygenTank.capacity = 1000;
        oxygenTank.fuel = 1000;
        oxygenTank.fuelType = FuelType::oxygen;
        fuelTanks.push_back(oxygenTank);
        
        Engine mainEngine(engines.getID());
        mainEngine.throttle = 1;
        mainEngine.fuelFlowRate = 30;
        mainEngine.isp = 220;
        mainEngine.heat = 1;
        mainEngine.heatOutput = 400;
        mainEngine.mass = 1200;
        engines.push_back(mainEngine);
        
        dryMass = 4000 + mainEngine.mass + oxygenTank.dryMass;
        mass = dryMass + oxygenTank.fuel;
    }
    
    int meshID;
    std::string debugName;
    static int instanceCount;
    Physics physics;
    float mass;
    float dryMass;
    PartList<Engine> engines;
    PartList<FuelTank> fuelTanks;
    
    //actions
    bool thrust;    //thrust for this frame;
};
#endif /* defined(__OrbitalVector__shipComponent__) */
