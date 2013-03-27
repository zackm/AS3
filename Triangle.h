#pragma once
#include "glm/glm.hpp"

#pragma once
#include "LocalGeo.h"

class Triangle{
public:
	//this class should aid with using barycentric coordiantes.
	LocalGeo a,b,c; //points, normals, and param value

	Triangle(LocalGeo,LocalGeo,LocalGeo);

	Triangle(void){};
	~Triangle(void){};
};

