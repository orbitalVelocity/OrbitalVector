//
//  debugTextSystem.h
//  OrbitalVector
//
//  Created by Si Li on 3/17/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__debugTextSystem__
#define __OrbitalVector__debugTextSystem__

#include <stdio.h>

#include "entityx/System.h"
#include "events.h"
#include "text.h"
#include "camera.h"

class DebugTextSystem: public entityx::System<DebugTextSystem>,
public entityx::Receiver<DebugEvent>
{
public:
    
    DebugTextSystem(TextRenderer *, Camera *, glm::mat4 *);
//    void init(TextRenderer *text);
    
    void configure(entityx::EventManager& eventManager);
    
    void receive(const DebugEvent &e);
    void receive(const GUITextEvent &e);
    
    void update(entityx::EntityManager & entities,
                entityx::EventManager & events,
                double dt);
    
private:
    struct expiringMessage{
//        expiringMessage(double t, string m) : expirationTime(t), message(m) {}
        
        double expirationTime;
        string message;
    };
    TextRenderer *debugTextPtr = nullptr;
    Camera *cameraPtr = nullptr;
    glm::mat4 *worldPtr = nullptr;
    std::vector<expiringMessage> messages;
    double currentTime = 0.0;
    double lifeTime = 3;
};

#endif /* defined(__OrbitalVector__debugTextSystem__) */
