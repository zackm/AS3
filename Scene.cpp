//
//  Scene.cpp
//  AS3
//

#include "Scene.h"

void Scene::add_patch(BezierPatch current_patch) {
    patch_list.push_back(current_patch);
    cout<<"new patch added"<<endl;
}

void Scene::add_geo(LocalGeo geo_arg) {
    geo_list.push_back(geo_arg);
}

void Scene::subdivide_patch(BezierPatch bez_patch) {
    float epsilon = 0.001f;
    float numdiv = ((1 + epsilon) / step);
    LocalGeo current_geo;
    
    for (int iu = 0; iu < numdiv; iu++) {
        float u = iu * step;
        
        for (int iv = 0; iv < numdiv; iv++) {
            float v = iv * step;
            current_geo = bez_patch.patch_interp(u,v);
            add_geo(current_geo);
        }
    }
}
