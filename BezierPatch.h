#pragma once
#include "glm/glm.hpp"

class BezierPatch{
public:
	glm::mat4<glm::vec3> patch;

	BezierPatch(void);
	~BezierPatch(void);

	void curve_interp(float,float*,float*,glm::vec4);
	void patch_interp(float,float,glm::vec3*,glm::vec3*);
	glm::vec4 u_curve(int);
	glm::vec4 v_curve(int);
};

