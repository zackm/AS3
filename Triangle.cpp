#include "Triangle.h"

Triangle::Triangle(LocalGeo x,LocalGeo y, LocalGeo z){
	a = x;
	b = y;
	c = z;
}

Triangle::Triangle(LocalGeo x,LocalGeo y,LocalGeo z,int n){
	a = x;
	b = y;
	c = z;
	divisions_remaining = n;
}