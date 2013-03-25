//
//  Scene.cpp
//  AS3
//
//  Created by Zack Mayeda on 3/25/13.
//  Copyright (c) 2013 Zack Mayeda. All rights reserved.
//

#include "Scene.h"

#pragma once
#include "BezierPatch.h"

void Scene::add_patch(BezierPatch current_patch) {
    patch_list.push_back(current_patch);
    cout<<"new patch added"<<endl;
}
