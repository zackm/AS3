//
//  Scene.cpp
//  AS3
//

#include "Scene.h"

void Scene::add_patch(BezierPatch current_patch) {
    patch_list.push_back(current_patch);
}

void Scene::subdivide_patch() {
    for (int i = 0; i < patch_list.size(); i++) {
        patch_list[i].subdivide_patch(step);
    }
}

void Scene::make_tri_list() {
    for (int i = 0; i < patch_list.size(); i++) {
        patch_list[i].make_tri_list();
    }
}
