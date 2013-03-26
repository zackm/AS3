//
//  Scene.h
//  AS3
//
//  Created by Zack Mayeda on 3/25/13.
//  Copyright (c) 2013 Zack Mayeda. All rights reserved.
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
    vector<LocalGeo> geo_list;
    vector<glm::vec3> tri_list;
    float step;
    
    Scene(void){};
    void add_patch(BezierPatch);
    void subdivide_patch();
    void make_tri_list();
};