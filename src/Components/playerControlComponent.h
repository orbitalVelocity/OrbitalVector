//
//  playerControlComponent.h
//  OrbitalVector
//
//  Created by Si Li on 4/26/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__playerControlComponent__
#define __OrbitalVector__playerControlComponent__

#include <stdio.h>
#include "entityx/Entity.h"
#include "componentTypes.h"

class entityVector : public std::vector<entityx::Entity> {
//private:
//    friend class cereal::access;
//    template <class Archive>
//    void serialize( Archive & ar )
//    {
//        for (auto i = 0; i < size(); i++)// data())
//        {
//            ar( CEREAL_NVP(data()[i].id().id()) );
//        }
//    }
};

namespace cereal
{
    template<class Archive>
    void save( Archive &ar, entityVector const & vec)
    {
        for (auto &entity : vec)
        {
            ar( CEREAL_NVP(entity.id().id()) );
        }
    }
    
    template<class Archive>
    void load( Archive &ar, entityVector & vec)
    {
        //        ar( entity.id() );//cereal::make_nvp("entityID", entity.id()) );
    }
}

COMPONENT(PlayerControl)
{
    PlayerControl() {}
    
    void newFocus(entityx::Entity newFocus)
    {
        lastEntityFocused = focusOnEntity;
        focusOnEntity = newFocus;
        switchedFocus = true;
        currentTime = 0;
    }
    float getProgress(float dt)
    {
        currentTime += dt;
        if (currentTime > animationTime)
            switchedFocus = false;
        
        auto t = currentTime/animationTime;
        auto factor = 5.0;
        return (1.0 - pow((1.0 - t), 2 * factor));
    }
    
    float animationTime = .8;
    float currentTime = 0;
    bool switchedFocus;
    entityx::Entity focusOnEntity, lastEntityFocused;
    entityx::Entity shadowEntity; //for orbit planning
    entityVector selectedEntities, mouseOverEntities;
    
private:
    friend class cereal::access;
    template <class Archive>
#define CEREAL_ENTITY(X) cereal::make_nvp("X", X.id().id())
    void serialize( Archive & ar )
    {
        ar( CEREAL_NVP(animationTime),
            CEREAL_NVP(currentTime),
            CEREAL_NVP(switchedFocus),
            CEREAL_NVP(selectedEntities),
            CEREAL_NVP(mouseOverEntities),
            CEREAL_ENTITY(focusOnEntity),
            CEREAL_ENTITY(lastEntityFocused)
           );
    }
    
};
#endif /* defined(__OrbitalVector__playerControlComponent__) */
