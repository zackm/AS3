#include "LocalGeo.h"
#include "glm/glm.hpp"

//float PI = 3.1415926;

LocalGeo::LocalGeo(glm::vec3 pt,glm::vec3 nm, glm::vec2 uv){
	point = pt;

	param_value = uv;

	float norm = glm::sqrt(glm::dot(nm,nm));

	if (norm>0){
		nm /= norm;
	}
	normal = nm;

	gaussian_curvature = mean_curvature = 0;
}

void LocalGeo::set_curvatures(){
	glm::mat2 upper_second_form;

	first_form[0][0] = glm::dot(partial_u,partial_u);
	first_form[1][1] = glm::dot(partial_v,partial_v);
	first_form[0][1] = first_form[1][0] = glm::dot(partial_u,partial_v); //symmetric

	float g = glm::determinant(first_form);

	if (g==0.0f){
		//check back here later.
		return;
	}

	glm::mat2 inv = glm::inverse(first_form);

	second_form[0][0] = glm::dot(partial_uu,normal);
	second_form[1][1] = glm::dot(partial_vv,normal);
	second_form[0][1] = second_form[1][0] = glm::dot(partial_uv,normal);

	upper_second_form[0][0] = second_form[0][0]*inv[0][0]+second_form[0][1]*inv[1][0];
	upper_second_form[1][1] = second_form[1][0]*inv[0][1]+second_form[1][1]*inv[1][1];
	upper_second_form[0][1] = second_form[0][0]*inv[0][1]+second_form[0][1]*inv[1][1];
	upper_second_form[1][0] = second_form[1][0]*inv[0][0]+second_form[1][1]*inv[1][0];

	gaussian_curvature = glm::determinant(upper_second_form);
	mean_curvature = (upper_second_form[0][0]+upper_second_form[1][1])/(2.0f);

	////Signed curvature mapped to [0,1]
	//gaussian_curvature = (glm::atan(gaussian_curvature)+(PI/2))/(PI);
	//mean_curvature = (glm::atan(mean_curvature)+(PI/2))/(PI);

	////Unsigned curvature mapped to [0,1]
	//gaussian_curvature = glm::atan(gaussian_curvature)/PI;
	//mean_curvature = glm::atan(mean_curvature)/PI;
}