#pragma once
#include "glm/glm.hpp"

#pragma once
#include "LocalGeo.h"

class Triangle{
public:
	//this class should aid with using barycentric coordiantes.
	LocalGeo a,b,c; //points, normals, and param value
	int divisions_remaining;

	Triangle(LocalGeo,LocalGeo,LocalGeo);
	Triangle(LocalGeo,LocalGeo,LocalGeo,int);

	Triangle(void){};
	~Triangle(void){};
};

