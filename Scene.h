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
    glm::vec3 min, max;
	int number_of_triangles;

	glm::vec3 camera_pos;
	glm::vec3 camera_look;
	glm::vec3 camera_up;

	//stuff for projection
	float left,right,bottom,top,z_near,z_far;
	float diameter; //diameter of bounding sphere
	glm::vec3 center; //center of bounding sphere
    
    float step; //step size to use for uniform tessellation
	float tolerance; //tolerance to use for adaptive tessellation
    
    Scene(void);
    void add_patch(BezierPatch);
    void subdivide_patch(bool);
    void set_min_max();
	void set_camera_pos();
};