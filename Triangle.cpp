#include "Triangle.h"

Triangle::Triangle(LocalGeo x,LocalGeo y, LocalGeo z){
	a = x;
	b = y;
	c = z;
	a_index = b_index = c_index = 0;

	set_areas();
}

Triangle::Triangle(LocalGeo x,LocalGeo y,LocalGeo z,int n){
	a = x;
	b = y;
	c = z;
	divisions_remaining = n;
	a_index = b_index = c_index = 0;

	set_areas();
}

Triangle::Triangle(LocalGeo x,LocalGeo y, LocalGeo z, int n1, int n2, int n3){
	a = x;
	b = y;
	c = z;
	a_index = n1;
	b_index = n2;
	c_index = n3;

	set_areas();
}

void Triangle::set_areas(){
	//First calculate the usual area
	glm::vec3 vec1 = b.point-a.point;
	glm::vec3 vec2 = c.point-a.point;
	//from normals (ie points on gauss sphere), calculate the triangle area
}