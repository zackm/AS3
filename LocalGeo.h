#pragma once
#include "glm/glm.hpp"

class LocalGeo{
public:
	glm::vec3 point;
	glm::vec3 normal;
	glm::vec2 param_value;

	glm::vec3 partial_u,partial_v,partial_uu,partial_vv,partial_uv,partial_vu;
	glm::mat2 first_form;
	glm::mat2 second_form;
	float gaussian_curvature,mean_curvature,max_curvature,min_curvature;
	glm::vec2 principal_curvatures;

	LocalGeo(void){gaussian_curvature = mean_curvature = max_curvature = min_curvature = 0;};
	LocalGeo(glm::vec3,glm::vec3,glm::vec2);
	~LocalGeo(void){};

	void set_curvatures();
};
