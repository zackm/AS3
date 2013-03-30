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
	first_form[0][1] = glm::dot(partial_u,partial_v); //symmetric so shouldn't need to do this.
	first_form[1][0] = glm::dot(partial_v,partial_u);

	float g = glm::determinant(first_form);

	if (g==0.0f){
		//check back here later.
		return;
	}

	glm::mat2 inv = glm::inverse(first_form);

	second_form[0][0] = glm::dot(partial_uu,normal);
	second_form[1][1] = glm::dot(partial_vv,normal);
	second_form[0][1] = glm::dot(partial_uv,normal); //should be symmetric, but getting different answers.
	second_form[1][0] = glm::dot(partial_vu,normal);

	upper_second_form[0][0] = second_form[0][0]*inv[0][0]+second_form[0][1]*inv[1][0];
	upper_second_form[1][1] = second_form[1][0]*inv[0][1]+second_form[1][1]*inv[1][1];
	upper_second_form[0][1] = second_form[0][0]*inv[0][1]+second_form[0][1]*inv[1][1];
	upper_second_form[1][0] = second_form[1][0]*inv[0][0]+second_form[1][1]*inv[1][0];

	//solve for principal curvatures using quadratic formula
	//might want to check discrim prior to square rooting
	float a,b,c,discrim;
	a = 1.0f;
	b = -(upper_second_form[0][0]+upper_second_form[1][1]);
	c = upper_second_form[0][0]*upper_second_form[1][1] - upper_second_form[0][1]*upper_second_form[1][0];
	principal_curvatures[0] = .5f*a*(-(b)+glm::sqrt(b*b-4.0f*a*c));
	principal_curvatures[1] = (1/(2.0f*a))*(-(b)-glm::sqrt(b*b-4.0f*a*c));

	gaussian_curvature = principal_curvatures[0]*principal_curvatures[1];
	mean_curvature = .5f*(principal_curvatures[0]+principal_curvatures[1]);

	max_curvature = glm::max(principal_curvatures[0],principal_curvatures[1]);
	min_curvature = glm::min(principal_curvatures[0],principal_curvatures[1]);
}