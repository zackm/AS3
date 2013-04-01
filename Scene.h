//
//  Scene.h
//  AS3
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
    glm::vec3 min, max;
	int number_of_triangles;

	glm::vec3 camera_pos;
	glm::vec3 camera_look;
	glm::vec3 camera_up;

	//stuff for an orthographic scene.
	float left,right,bottom,top,z_near,z_far;
	float diameter; //for bounding sphere
	glm::vec3 center;
    
    float step;
    
    Scene(void);
    void add_patch(BezierPatch);
    void subdivide_patch(bool);
    void set_min_max();
	void set_camera_pos();
};