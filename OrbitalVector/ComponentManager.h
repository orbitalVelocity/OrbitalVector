//
//  ComponentManager.h
//  OrbitalVector
//
//  Created by Si Li on 11/19/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__ComponentManager__
#define __OrbitalVector__ComponentManager__

#include <iostream>
#include "includes.h"
#include "Component.h"
#include "spatial.h"
#include "unit.h"

using namespace std;
using namespace glm;

class ComponentManager
{
private:
    Component<vec3> pos, vel;
    Component<double> GM;
    Component<float> radius;
    Component<Spatial> spatial;
    Component<Unit> unit;

public:
    
    ComponentManager(){}
    int addEntry(vec3 p, vec3 v,
                  double mu,
                  float r,
                  Spatial s,
                  Unit u)
    {
        pos.push_back(p);
        vel.push_back(v);
        GM.push_back(mu);
        spatial.push_back(s);
        unit.push_back(u);
        
        return (int)pos.size() - 1;
    }
    void removeEntry(int index)
    {
        pos.erase(index);
        vel.erase(index);
        GM.erase(index);
        spatial.erase(index);
        unit.erase(index);
        
        //update entity to this index
    }
    
    const vector<vec3>& getPosComponents() { return pos.data(); }
    const vector<vec3>& getVelComponents() { return vel.data(); }
    const vector<double>& getGMComponents() { return GM.data(); }
    const vector<Spatial>& getSpatialComponents() { return spatial.data(); }
    const vector<Unit>& getUnitComponents() { return unit.data(); }
    
    //only called from entity
    const vec3& getPos(int index) { return pos[index]; }
    const vec3& getVel(int index) { return vel[index]; }
    const double& getGM(int index) { return GM[index]; }
    const Spatial& getSpatial(int index) { return spatial[index]; }
    const Unit& getUnit(int index) { return unit[index]; }
    
    void setPos(int index, vec3 p) { pos[index] = p; }
    void setVel(int index, vec3 v) { vel[index] = v; }
    void setGM(int index, double gm) { GM[index] = gm; }
    void setSpatial(int index, Spatial& s) { spatial[index] = s; }
    void setUnit(int index, Unit& u) { unit[index] = u; }
};

#endif /* defined(__OrbitalVector__ComponentManager__) */
