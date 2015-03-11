//
//  ecs.h
//  OrbitalVector
//
//  Created by Si Li on 3/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_ECS_H
#define OrbitalVector_ECS_H
#include "includes.h"
#include "rk547m.h"
#include "entityx/entityx.h"
#include "componentTypes.h"

#define OLDECS true

const int SHIPOFFSET = 1;
const int MISSILEOFFSET = 3; //only works with 1 grav, 2 ships, and 1 missile

glm::vec3 getShipPos(int index);
glm::vec3 getMissilePos(int index);
glm::vec3 getEntityPosition(int index);
glm::vec3* getMissileVelocityPointer(int index);

glm::vec3 getMyShipPos();
glm::vec3 getMyShipVel();

int getNumberOfShips();
int getNumberOfMissiles();
int getNumberOfEntities();

std::vector<body> getAllOrbitalObjects();
void setAllOrbitalObjects(std::vector<body>);
void updateOrbitalPhysics(float dt, std::vector<std::vector<state> > &ks, bool variableDT);

void createRandomShip();


void initECS();

class Level : public entityx::EntityX {
private:
    entityx::Entity myShipID;
    entityx::Entity mainGrav;
public:
    explicit Level(std::string filename) {
//        systems.add<DebugSystem>();
//        systems.add<MovementSystem>();
//        systems.add<CollisionSystem>();
        systems.configure();
        
        load(filename);
        
//        for (auto e : entity_data()) {
//            entityx::Entity entity = entities.create();
//            entity.assign<Position>(e.component<Position>());
//            entity.assign<Velocity>(e.component<Velocity>());
//        }
    }

    /**
     * Convenience function for creating a generic orbiting body in ECS
     * @parameter pos glm::vec3 position
     * @parameter vel glm::vec3 velocity
     * @parameter orientation glm::mat4 4x4 matrix for storing orientation
     * @parameter _gm  double GM gravity
     * @parameter r    float radius
     * @parameter _parent entityx::Entity::Id refer to parent gravity well
     * @parameter bt   BodyType enum designating body type
     */
    void loadEntity(entityx::Entity entity,
                    glm::vec3 pos,
                    glm::vec3 vel,
                    glm::mat4 orientation,
                    double _gm,
                    float r,
                    entityx::Entity::Id _parent,
                    BodyType bt
                    )
    {
        entity.assign<Position>(pos);
        entity.assign<Velocity>(vel);
        entity.assign<GM>(_gm);
        entity.assign<Parent>(_parent);
        entity.assign<OrbitalBodyType>(bt);
        entity.assign<Orientation>(orientation);
        entity.assign<Radius>(r);
    }
    
    void createRandomShip();
    void createEntity(
                    glm::vec3 pos,
                    glm::vec3 vel,
                    glm::mat4 orientation,
                    double gm,
                    float r,
                    int type)
    {
        auto newShip = entities.create();
        loadEntity(newShip,
                   pos,
                   vel,
                   orientation,
                   gm,
                   r,
                   mainGrav.id(),
                   (BodyType)type);
    }
    
    void createShip(
                    glm::vec3 pos,
                    glm::vec3 vel,
                    glm::mat4 orientation,
                    double gm,
                    float r)
    {
        auto newShip = entities.create();
        loadEntity(newShip,
                   pos,
                   vel,
                   orientation,
                   gm,
                   r,
                   mainGrav.id(),
                   BodyType::SHIP);
    }
    
    //loads level of name filename
    void load(std::string filename)
    {
        return;
        //FIXME: this is because InsertToSys creates new entities directly
        double m = 7e12;
        double G = 6.673e-11;
        double gm = m * G;
        
        //load json and create entities
        auto nullEntity = entities.create();
        nullEntity.invalidate();
        mainGrav = entities.create();
        loadEntity(mainGrav,
                   {},
                   glm::vec3(0,0,-.1),
                   {},
                   gm,
                   10,
                   nullEntity.id(),
                   BodyType::GRAV
                   );

        
        m = 1e5;
        gm = m * G;
        
        myShipID = entities.create();
        loadEntity(myShipID,
                   {110,0,0},
                   {0,0,2.3},
                   {},
                   gm,
                   10,
                   mainGrav.id(),
                   BodyType::SHIP
                   );
    };
   
    //grabs entity from data loaded in load
    std::vector<entityx::Entity> entity_data();
    
    void update(entityx::TimeDelta dt) {
//        systems.update<DebugSystem>(dt);
//        systems.update<MovementSystem>(dt);
//        systems.update<CollisionSystem>(dt);
    }
    
};

extern Level myLevel;
#endif
