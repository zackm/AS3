#pragma once
#include "glm/glm.hpp"

class LocalGeo{
public:
	glm::vec3 point;
	glm::vec3 normal;
	glm::vec2 param_value; //the value of u and v used to get this point on bezier surface

	glm::vec3 partial_u,partial_v,partial_uu,partial_vv,partial_uv,partial_vu; //derivatives for curvatures

	//
	float gaussian_curvature,mean_curvature,max_curvature,min_curvature; //stored curvature values
	glm::vec2 principal_curvatures;

	LocalGeo(void){gaussian_curvature = mean_curvature = max_curvature = min_curvature = 0;};
    LocalGeo(glm::vec3,glm::vec3);
	LocalGeo(glm::vec3,glm::vec3,glm::vec3,glm::vec3,glm::vec3,glm::vec3,glm::vec3,glm::vec3,glm::vec2);
	~LocalGeo(void){};

private:
	void set_curvatures();

	//Used to calculate curvatures
	glm::mat2 first_form; //coefficients of the first fundamental form
	glm::mat2 second_form; //coefficients of the second fundamental form
};
