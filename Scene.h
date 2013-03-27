//
//  Scene.h
//  AS3
//

#ifndef __AS3__Scene__
#define __AS3__Scene__

#include <iostream>

#endif /* defined(__AS3__Scene__) */

#pragma once
#include "BezierPatch.h"

#pragma once
#include "glm/glm.hpp"

class Scene {
public:
    vector<BezierPatch> patch_list;
    glm::vec3 min, max;
    
    float step;
    
    Scene(void){};
    void add_patch(BezierPatch);
    void subdivide_patch(bool);
    void set_min_max();
};