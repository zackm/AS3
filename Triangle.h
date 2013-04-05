#pragma once
#include "glm/glm.hpp"

#pragma once
#include "LocalGeo.h"

class Triangle{
public:
	LocalGeo a,b,c; //three vertices a, b, and c.
	int a_index,b_index,c_index;
	int divisions_remaining; //divisions remaining only needed for adaptive tessellation.
	float area; //usual area.
	float sphere_area; //gauss sphere area of normals
	LocalGeo* geo_at_index(int);

	Triangle(LocalGeo,LocalGeo,LocalGeo);
	Triangle(LocalGeo,LocalGeo,LocalGeo,int);
	Triangle(LocalGeo,LocalGeo,LocalGeo,int,int,int);

	Triangle(void){};
	~Triangle(void){};

private:
	void set_areas();
};

