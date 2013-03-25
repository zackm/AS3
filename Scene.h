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
    
    void add_patch(BezierPatch);
    
    Scene(void){};
};