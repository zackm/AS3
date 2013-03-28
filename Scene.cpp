//
//  Scene.cpp
//  AS3
//

#include "Scene.h"

#include <limits>

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


//this can set camera based on patches probably, not necessarily the LocalGeo
void Scene::set_min_max() {
	float x_min,y_min,z_min,x_max,y_max,z_max;

	x_min = y_min = z_min = numeric_limits<int>::max();
	x_max = y_max = z_max = numeric_limits<int>::min();

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

	//cout<<max.x<<','<<min.x<<endl;
	//cin.get();
	max.x = x_max;
	max.y = y_max;
	max.z = z_max;
	min.x = x_min;
	min.y = y_min;
	min.z = z_min;
}

void Scene::set_camera_pos(){
	diameter = glm::max(glm::max(max.x-min.x,max.z-min.z),max.y-min.y);

	center.x = (max.x+min.x)/2.0f;
	center.y = (max.y+min.y)/2.0f;
	center.z = (max.z+min.z)/2.0f;

	left = center.x-diameter;
	right = center.x+diameter;
	bottom = center.y-diameter;
	top = center.y+diameter;
	z_near = 1.0f;
	z_far = z_near+10.0f*diameter;

	camera_pos.x = camera_pos.y = 0;
	camera_pos.z = 2.0f*diameter;

	camera_look.x = center.x;
	camera_look.y = center.y;
	camera_look.z = center.z;
	camera_up.x = camera_up.z = 0;
	camera_up.z = 1.0f;
}