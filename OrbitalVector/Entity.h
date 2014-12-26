//
//  Entity.h
//  OrbitalVector
//
//  Created by Si Li on 11/20/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__Entity__
#define __OrbitalVector__Entity__

#include <stdio.h>

class EntityManager;
#define MAX_ID 4096
#define ID_MASK MAX_ID - 1
#define ID_BITS 12
//max family implicit (2^32/idMax/2) (last 2 for sign bit)
class Entity
{
    friend EntityManager;
public:
    Entity(int index, int family)
    {
        id = (1 << 31) | (family << ID_BITS) | index;
    }
    //index points to location in collection (vector of components)
    int index()             { return id & ID_MASK; }
    int family()
    {
        return (id & ~(0x1 << 31)) >> ID_BITS;
    }
    int getRaw()            { return id; }
    bool isValid()
    {
        return id & ~(0x1 << 31);
    }
private:
    int id;
    void setID(int i)       { id = i; }
    
    void setValidity(bool b)
    {
        if (b)
            id |= 1 << 31;
        else
            id &= ~(1 << 31);
    }
    void toggleValidity()
    {
        id ^= 0x1 << 31;
    }
};
#endif /* defined(__OrbitalVector__Entity__) */
