#pragma once
#include "glm/glm.hpp"

class LocalGeo{
public:
	glm::vec3 point;
	glm::vec3 normal;
	glm::vec2 param_value;

	LocalGeo(void){};
	LocalGeo(glm::vec3,glm::vec3,glm::vec2);
	~LocalGeo(void){};
};
