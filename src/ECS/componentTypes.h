//
//  componentTypes.h
//  OrbitalVector
//
//  Created by Si Li on 3/7/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_componentTypes_h
#define OrbitalVector_componentTypes_h

#include "includes.h"
#include "entityx/Entity.h"

#define COMPONENT( X ) struct X : public entityx::Component<X>

enum BodyType {
    GRAV,
    SHIP,
    MISSILE,
    PROJECTILE,
    MAX_BODY_TYPE
};

extern glm::mat4 world;

typedef unsigned int tag_t;

COMPONENT(Tag)
{
    static tag_t previous;

    Tag()
    {
        tag = ++previous;
    }

    tag_t tag;
};

COMPONENT(MissileLogic)
{
    MissileLogic() {}
    MissileLogic(entityx::Entity p, entityx::Entity t)
    : parent(p), target(t) {}

    tag_t targettag;
    entityx::Entity target;
    tag_t parenttag;
    entityx::Entity parent;
    bool done = false;
    
};

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
    std::vector<entityx::Entity> selectedEntities;
    std::vector<entityx::Entity> mouseOverEntities;
};

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
        return throttle * fuelFlowRate * isp * 9.81 * (fuelUsed/requiredFuel) * dt;
        
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


COMPONENT(Missile)
{
    Missile()
    {
        debugName = "missile" + std::to_string(instanceCount);
        instanceCount++;
    }
    int meshID;
    std::string debugName;
    static int instanceCount;
};


COMPONENT(Weapon)
{
    Weapon() {}
    
};

enum UISelectionType
{
    INVALID,
    HOVER,
    SELECTED
};

COMPONENT(UISelection)
{
    UISelection() {}
    UISelection(int t) : type(t) {}
    int type;
};

COMPONENT(Velocity)
{
    Velocity() {}
    Velocity(glm::vec3 v) : vel(v) {}
    
    glm::vec3 vel;
};

COMPONENT(Position)
{
    Position() {}
    Position(glm::vec3 p) : pos(p) {}
    
    glm::vec3 pos;
};

COMPONENT(GM)
{
    GM() {}
    GM(double _gm) : gm(_gm) {}
    double gm;
};

COMPONENT(Parent)
{
    Parent() {}
    Parent(entityx::Entity::Id p) : parent(p) {}
    
    tag_t parenttag;
    entityx::Entity::Id parent;
};

COMPONENT(OrbitalBodyType)
{
    OrbitalBodyType() {}
    OrbitalBodyType(BodyType bt) : orbitalBodyType(bt) {}
    
    BodyType orbitalBodyType;
};

COMPONENT(Orientation)
{
    Orientation () {}
    Orientation (glm::mat4 o) : orientation(o) {}
    
    glm::mat4 orientation;
    float x=0, y=90;
};

COMPONENT(Radius)
{
    Radius () {}
    Radius (float r) : radius(r) {}
    
    float radius;
};

COMPONENT(CameraComponent)
{
    CameraComponent () {}

    glm::vec3 position;
    glm::vec3 focus;
    
    float fov;
    float hAngle, vAngle;
    float nearPlane, farPlane;
    float ratio;
};

COMPONENT(OrbitPath)
{
    OrbitPath() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
    }
    
    ~OrbitPath() {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
    
    std::vector<float> path;
    glm::mat4 transform;
    GLuint vao;
    GLuint vbo;
};

struct Animation
{
    Animation(float t) : totalTime(t) {}
    Animation(float d, float t) : delayedStartTime(d),
                                           totalTime(t)
    {}
    
    float delayedStartTime = 0;
    float elapsedTime = 0.01;
    float totalTime;
};

enum class AnimationState
{
    invalid,
    start,
    end
};

struct UIElement
{
    UIElement(float t, float r) : time(t), rotateByRadian(r) {};
    UIElement(float d, float t, float r) : time(d, t), initialRotation(r) {};
    
    bool hover = false;
    AnimationState state = AnimationState::invalid;
    Animation time;
    glm::vec2 offset2d;
    glm::vec2 scale2d;
    float initialRotation;
    float rotateByRadian;
    //size, color, animation style?
};


//GUI Circle menu consists of one center element and n leaf elements
//on activation, center animates, then leaves animate to open state
//mouseHover state
//mouseClick state
// send event to relevant system(s)
COMPONENT(GUICircleMenu)
{
    GUICircleMenu() {}
    GUICircleMenu(entityx::Entity t, int n) : target(t)
    {
        leafMenus.reserve(n);
        for (auto i = 0; i < n; i++)
        {
            leafMenus.push_back(UIElement((i+1) * 0.05 / (float)n,
                                          0.6,
                                          -M_PI/4 + i * 2.0*M_PI/(float)n));
        }
        centerElement.state = AnimationState::start;
    }
    
    float size = 50; //circular hit target (radius)
    entityx::Entity target;
    UIElement centerElement = UIElement(0.6, 0);
    std::vector<UIElement> leafMenus;
};

COMPONENT(UIText)
{
    UIText() {}
    UIText(float size) : fontSize(size), totalOffset2d(glm::vec2())
    {
        auto verticalOffset = 0.02 / 15.0 * size; //FIXME: needs access to window size
        offset2d = glm::vec2(0, verticalOffset);
    }
    glm::vec2 getOffset()
    {
        totalOffset2d += offset2d;
        return totalOffset2d;
    }
    void clearOffset()
    {
        totalOffset2d = glm::vec2();
    }
    
    glm::vec2 offset2d, totalOffset2d;
    float fontSize;
    //rotation?
};

#endif
