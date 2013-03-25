#include "BezierPatch.h"


BezierPatch::BezierPatch(void)
{
}


BezierPatch::~BezierPatch(void)
{
}



void BezierPatch::curve_interp(float u,float* point,float* deriv,glm::vec4 curve){
	glm::vec3 A,B,C,D,E;

	A = curve[0]*(1.0-u)+curve[1]*u;

}

void BezierPatch::patch_interp(float,float,glm::vec3*,glm::vec3*){
	
}

glm::vec4 BezierPatch::u_curve(int i){
	return glm::vec4(patch[i][0],patch[i][1],patch[i][2],patch[i][3]);
}

glm::vec4 BezierPatch::v_curve(int i){
	return glm::vec4(patch[0][i],patch[1][i],patch[2][i],patch[3][i]);
}