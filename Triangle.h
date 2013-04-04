#pragma once
#include "glm/glm.hpp"

#pragma once
#include "LocalGeo.h"

class Triangle{
public:
	LocalGeo a,b,c; //three vertices a, b, and c.
	int divisions_remaining; //divisions remaining only needed for adaptive tessellation.

	Triangle(LocalGeo,LocalGeo,LocalGeo);
	Triangle(LocalGeo,LocalGeo,LocalGeo,int);

	Triangle(void){};
	~Triangle(void){};
};

