//
//  EntityManager.h
//  OrbitalVector
//
//  Created by Si Li on 11/20/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__EntityManager__
#define __OrbitalVector__EntityManager__

#include <stdio.h>
#include "includes.h"
#include "Entity.h"
#include "ComponentManager.h"
#include <list>

using namespace std;

enum Family {
    GRAV, SHIP, MISSILE, PROJECTILE,
    MAX_FAMILIES
};
class EntityManager
{
private:
    vector<vector<Entity> > families;
    vector<ComponentManager> componentManagers;
    vector<list<int>> freeList;     //free slots in families
public:
    EntityManager() {
        families.resize(MAX_FAMILIES);
        componentManagers.resize(MAX_FAMILIES);
    }
    vector<Entity>& getFamily(Family f)
    {
        return families[f];
    }
    
    vector<ComponentManager>& getComponentManagers()
    {
        return componentManagers;
    }
    //get collection - call components directly
    ComponentManager& getComponentManager(Family f)
    {
        return componentManagers[f];
    }
    
    //remove entity
     void removeEntity(int i, Family f)
    {
        auto & id = families[f][i];
        assert(f == id.family());
        assert(id.isValid());
        id.setValidity(false);
        
        freeList[f].push_back(i);
    }
    //add entity
    void addEntity(Family f,
                   vec3 p, vec3 v,
                   double mu,
                   float r,
                   Spatial s,
                   Unit u)
    {
        //to find suitable index, look at colleciton length
        // guaranteed to be at the end
        auto index = componentManagers[f].addEntry(p, v, mu, r, s, u);
        if (freeList[f].empty())
        {
            families[f].emplace_back(Entity(f, index));
        } else {
            auto id = freeList[f].front();
            freeList[f].pop_front();
            families[f][id] = std::move(Entity(f, index));
        }
    }
    
    //TODO: unsupported right now
    //get all components from one entity
    //add collection?
    //remove collection?
};

#endif /* defined(__OrbitalVector__EntityManager__) */
