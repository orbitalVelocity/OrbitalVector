//
//  Component.h
//  OrbitalVector
//
//  Created by Si Li on 11/19/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __OrbitalVector__Component__
#define __OrbitalVector__Component__

#include <iostream>
#include "includes.h"
#include <algorithm>

using namespace std;
using namespace glm;

class BaseComponent
{
    virtual int size() = 0;
    //can't do template in base class,
    //C++ no support heterogeneous vector
};

template <typename T>
class Component //: BaseComponent
{
public:
    void push_back(T& t)    { array.push_back(t);   }
    int size()              { return (int)array.size();  }
    vector<T>& data()         { return array;         }
    T& operator[](int index)
                            { return array.at(index);  }
    void erase(int index)   { array.erase(array.begin() + index); }
    //must test this first before using it
//    void erase(int index)
//    {
//        auto begin = array.begin()+index;
//        auto last = array.end()-1;
//        std::swap(begin, last);
//        array.resize(array.size()-1);
//    }
private:
    vector<T> array;
};

#endif /* defined(__OrbitalVector__Component__) */
