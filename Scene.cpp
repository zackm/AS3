#include "Scene.h"

#include <limits>

#include <iostream>
#include <fstream>
using namespace std;

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

void Scene::output_obj_file(string filename){
	string temp_file = "testing.obj";

	int n = patch_list.size();

	vector<LocalGeo> geo_list; //list of full geometry
    vector<Triangle> tri_list; //represents the final list of fully subdivided triangles

	//put all triangles into a single list
	vector<Triangle> full_tri_list;
	for (int i =0 ; i<n ; i++){
		BezierPatch cur_patch = patch_list[i];

		for (int j = 0; j<cur_patch.tri_list.size(); j++){
			Triangle cur_tri = cur_patch.tri_list[j];
			full_tri_list.push_back(cur_tri);
		}
	}

	//put all points into a single list
	vector<glm::vec3> full_point_list;
	vector<glm::vec3> full_normal_list;
	for (int i=0; i<full_tri_list.size(); i++){
		Triangle cur_tri = full_tri_list[i];

		LocalGeo a = cur_tri.a;

		if (!in_list(full_point_list, a.point)){
			full_point_list.push_back(a.point);
		}

		if (!in_list(full_normal_list, a.normal)){
			full_normal_list.push_back(a.normal);
		}

		LocalGeo b = cur_tri.b;

		if (!in_list(full_point_list, b.point)){
			full_point_list.push_back(b.point);
		}

		if (!in_list(full_normal_list, b.normal)){
			full_normal_list.push_back(b.normal);
		}

		LocalGeo c = cur_tri.c;

		if (!in_list(full_point_list, c.point)){
			full_point_list.push_back(c.point);
		}

		if (!in_list(full_normal_list, c.normal)){
			full_normal_list.push_back(c.normal);
		}

	}

	//print out all points and normals into file
	ofstream myfile;
	myfile.open (temp_file);
	for (int i = 0; i<full_point_list.size(); i++){
		glm::vec3 cur_point = full_point_list[i];

		myfile <<"v "<< cur_point[0] << " "<< cur_point[1]<< " " << cur_point[2] << "\n";

	}

	for (int i = 0; i<full_normal_list.size(); i++){
		glm::vec3 cur_normal = full_normal_list[i];

		myfile <<"vn "<< cur_normal[0] << " "<< cur_normal[1]<< " " << cur_normal[2] << "\n";

	}


	//for each triangle, find corresponding point and normal, then print output
	for (int i = 0; i<full_tri_list.size(); i++){
		Triangle cur_tri = full_tri_list[i];

		int point_index = position_in_list(full_point_list, cur_tri.a.point);
		int normal_index = position_in_list(full_normal_list, cur_tri.a.normal);

		myfile << "f " << point_index << "//" << normal_index<<" " ;

		point_index = position_in_list(full_point_list, cur_tri.b.point);
		normal_index = position_in_list(full_normal_list, cur_tri.b.normal);
		myfile << point_index << "//" << normal_index<<" ";

		point_index = position_in_list(full_point_list, cur_tri.c.point);
		normal_index = position_in_list(full_normal_list, cur_tri.c.normal);
		myfile << point_index << "//" << normal_index<<"\n";
	}

	myfile.close();

}

bool Scene::in_list(vector<glm::vec3> list, glm::vec3 point){

	for (int i = 0; i<list.size(); i++){
		glm::vec3 cur = list[i];

		if (cur==point) {
			return true;
		}
	}

	return false;
}

int Scene::position_in_list(vector<glm::vec3> list, glm::vec3 point){

	for (int i = 0; i<list.size(); i++){
		glm::vec3 cur = list[i];

		if (cur==point) {
			return i+1;
		}
	}
}