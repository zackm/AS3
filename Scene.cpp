#include "Scene.h"

#include <limits>

Scene::Scene(){
	number_of_triangles = 0;
	glm::vec3 camera_pos(0,0,0);
	glm::vec3 camera_look(0,0,0);
	glm::vec3 camera_up(0,0,0);
	glm::vec3 min(0,0,0);
	glm::vec3 max(0,0,0);
	tolerance = step = 0.0f;
}

/*
Adds BezierPatch current_patch to the end of the patch list stored in Scene.
*/
void Scene::add_patch(BezierPatch current_patch) {
	patch_list.push_back(current_patch);
}

/*
Subdivides all patches in patch list. If use_adaptive if true, then we 
call a adaptive subdivision method in BezierPatch. Else, we call uniform
subdivision.
*/
void Scene::subdivide_patch(bool use_adaptive) {
	float tolerance = step;
	int n = patch_list.size();

	cout<<"Patches to subdivide: "<<n<<'\n'<<endl;
	cout<<"Patches completed: "<<0<<'/'<<n<<endl;
	if (use_adaptive){
		for (int i = 0; i < patch_list.size(); i++) {
			patch_list[i].adaptive_subdivide(tolerance);
			cout<<"Patches completed: "<<i+1<<'/'<<n<<endl;
			number_of_triangles += patch_list[i].tri_list.size();
		}
	}else{
		for (int i = 0; i < patch_list.size(); i++) {
			patch_list[i].uniform_subdivide(step); //should rename step to tol to reduce confusion
			cout<<"Patches completed: "<<i+1<<'/'<<n<<endl;
			number_of_triangles += patch_list[i].tri_list.size();
		}
	}
	set_min_max();
}

/*
This method is called after the list of all LocalGeo objects have been made
by subdivide_patch. It sets the boudning box of the scene by calculating the max and min values
in the x, y, and z directions.
*/
void Scene::set_min_max() {
	float x_min,y_min,z_min,x_max,y_max,z_max;

	x_min = y_min = z_min = numeric_limits<int>::max();
	x_max = y_max = z_max = numeric_limits<int>::min();

	for (int i = 0; i < patch_list.size(); i++) {
		BezierPatch b = patch_list[i];
		for (int j = 0; j < b.geo_list.size(); j++) {
			glm::vec3 p = b.geo_list[j].point;
			x_min = glm::min(x_min,p[0]);
			y_min = glm::min(y_min,p[1]);
			z_min = glm::min(z_min,p[2]);

			x_max = glm::max(x_max,p[0]);
			y_max = glm::max(y_max,p[1]);
			z_max = glm::max(z_max,p[2]);
		}
	}

	max.x = x_max;
	max.y = y_max;
	max.z = z_max;
	min.x = x_min;
	min.y = y_min;
	min.z = z_min;
}

/*
Called after min and max have been set. It sets the camera using
a bounding sphere about the object in the scene. 
*/
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
	camera_up.y = 1.0f;
}