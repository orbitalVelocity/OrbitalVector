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
#include "rk547m.h"

using namespace std;
using namespace glm;

class ComponentManager
{
private:
    Component<body> sys;
    Component<vec3> pos, vel;
    Component<float> GM;
    Component<float> radius;
    Component<Spatial> spatial;
    Component<Unit> unit;

public:
    
    ComponentManager(){}

    int addEntry(vec3 p, vec3 v,
                  float mu,
                  float r,
                  Spatial s,
                  Unit u)
    {
        auto b = body(state(p, v), mu, r);
        return addEntry(b, s, u);
    }
    
    int addEntry(body &b)
    {
        return addEntry(b, Spatial(), Unit());
    }
    
    int addEntry(body &b, Spatial s, Unit u)
    {
        sys.push_back(b);
        const int numTerms = 8;
        ks.resize(numTerms);
        for (auto &k : ks)
            k.resize(sys.size());
        
        auto p = b.sn.pos;
        auto v = b.sn.vel;
        auto mu = b.mu;
        auto r = b.radius;
        
        pos.push_back(p);
        vel.push_back(v);
        GM.push_back(mu);
        radius.push_back(r);
        spatial.push_back(s);
        unit.push_back(u);
        
        return (int)pos.size() - 1;
    }
    void removeEntry(int index)
    {
        sys.erase(index);
        
        pos.erase(index);
        vel.erase(index);
        GM.erase(index);
        radius.erase(index);
        spatial.erase(index);
        unit.erase(index);
        
        //search for matching entity given pos.size()
        //update matched entity with index
    }
    
    vector<body>& getSysComponents() { return sys.data(); }
    vector<vec3>& getPosComponents() { return pos.data(); }
    vector<vec3>& getVelComponents() { return vel.data(); }
    vector<float>& getGMComponents() { return GM.data(); }
    vector<float>& getRadiusComponents() {return radius.data(); }
    vector<Spatial>& getSpatialComponents() { return spatial.data(); }
    vector<Unit>& getUnitComponents() { return unit.data(); }
    
    //only called from entity
    const int size()              { return pos.size(); }
    const vec3& getPos(int index) { return pos[index]; }
    const vec3& getVel(int index) { return vel[index]; }
    const float& getGM(int index) { return GM[index]; }
    const float& getRadius(int index) { return radius[index]; }
    const Spatial& getSpatial(int index) { return spatial[index]; }
    const Unit& getUnit(int index) { return unit[index]; }
    
    void setPos(int index, vec3 p) { pos[index] = p; }
    void setVel(int index, vec3 v) { vel[index] = v; }
    void setGM(int index, float gm) { GM[index] = gm; }
    void setRadius(int index, float r) { radius[index] = r; }
    void setSpatial(int index, Spatial& s) { spatial[index] = s; }
    void setUnit(int index, Unit& u) { unit[index] = u; }
};

#endif /* defined(__OrbitalVector__ComponentManager__) */
