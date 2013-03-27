#include "LocalGeo.h"
#include "glm/glm.hpp"

LocalGeo::LocalGeo(glm::vec3 pt,glm::vec3 nm, glm::vec2 uv){
	point = pt;

	param_value = uv;

	float norm = glm::sqrt(glm::dot(nm,nm));

	if (norm>0){
		nm /= norm;
	}
	normal = nm;
}
