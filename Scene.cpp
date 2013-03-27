//
//  Scene.cpp
//  AS3
//

#include "Scene.h"

void Scene::add_patch(BezierPatch current_patch) {
	patch_list.push_back(current_patch);
}

void Scene::subdivide_patch(bool use_adaptive) {
	glm::vec3 current_geo;

	if (use_adaptive){
		for (int i = 0; i < patch_list.size(); i++) {
			patch_list[i].adaptive_subdivide(step);
		}
	}else{
		for (int i = 0; i < patch_list.size(); i++) {
			patch_list[i].uniform_subdivide(step); //should rename step to tol to reduce confusion
		}
	}
}

void Scene::make_tri_list(bool use_adaptive) {
	for (int i = 0; i < patch_list.size(); i++) {
		patch_list[i].make_tri_list(step);
	}
}


//this can set camera based on patches probably, not necessarily the LocalGeo
void Scene::set_min_max() {
	float x_min,y_min,z_min,x_max,y_max,z_max;
	for (int i = 0; i < patch_list.size(); i++) {
		BezierPatch b = patch_list[i];
		for (int j = 0; j < b.geo_list.size(); j++) {
			glm::vec3 p = b.geo_list[j].point;
			x_min = glm::min(x_min,p.x);
			y_min = glm::min(y_min,p.y);
			z_min = glm::min(z_min,p.z);
			x_max = glm::max(x_max,p.x);
			y_max = glm::max(y_max,p.y);
			z_max = glm::max(z_max,p.z);
		}
	}
	max.x = x_max;
	max.y = y_max;
	max.z = z_max;
	min.x = x_min;
	min.y = y_min;
	min.z = z_min;
}