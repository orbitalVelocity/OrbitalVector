//
//  cerealHelper.h
//  OrbitalVector
//
//  Created by Si Li on 6/3/15.
//  Copyright (c) 2015 Si Li. All rights reserved.
//

#ifndef OrbitalVector_cerealHelper_h
#define OrbitalVector_cerealHelper_h

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>

#include "includes.h"

namespace cereal {
    template<class Archive>
    void save(Archive & ar, glm::vec3 const & vec)
    {
        ar( make_nvp("x", vec.x));
        ar( make_nvp("y", vec.y));
        ar( make_nvp("z", vec.z));
    }
    template<class Archive>
    void load( Archive &ar, glm::vec3 & vec)
    {
        float x,y,z;
        ar(x);
        ar(y);
        ar(z);
        vec.x = x;
        vec.y = y;
        vec.z = z;
    }
}
#endif
