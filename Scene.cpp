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

void Scene::make_tri_list() {
    for (int i = 0; i < geo_list.size(); i++) {
        if (i%4 == 0) {
            if (i+1 < 16) {
                glm::vec3 tri(i+1,i,i+4);
                tri_list.push_back(tri);
            }
        } else if (i%4 == 3) {
            if (i - 4 >= 0) {
                glm::vec3 tri(i-1,i,i-4);
                tri_list.push_back(tri);
            }
        } else {
            if (i+1 < 16) {
                glm::vec3 tri(i+1,i,i+4);
                tri_list.push_back(tri);
            }
            if (i - 4 >= 0) {
                glm::vec3 tri(i-1,i,i-4);
                tri_list.push_back(tri);
            }
        }
    }
}
