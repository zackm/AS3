#include "Triangle.h"
#include <iostream>

using namespace std;

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

LocalGeo* Triangle::geo_at_index(int n){
	if(n==a_index){
		return &a;
	}else if(n==b_index){
		return &b;
	}else{//(n==c_index){
		return &c;
	//}else{

	}
}

void Triangle::set_areas(){
	area = 1.0f; //shouldn't deault to 0
	sphere_area = 0.0;
	float PI = 3.1415926;

	//First calculate the usual area
	glm::vec3 vec1 = b.point-a.point;
	glm::vec3 vec2 = c.point-a.point;

	glm::vec3 cross_prod = glm::cross(vec1,vec2);
	area = .5f*glm::sqrt(glm::dot(cross_prod,cross_prod)); //I think it's square root

	//Now from normals (ie points on gauss sphere), calculate the triangle area
	float a_temp,b_temp,c_temp,alpha,beta,gamma;
	a_temp = glm::acos(glm::dot(b.normal,c.normal));
	b_temp = glm::acos(glm::dot(a.normal,c.normal));
	c_temp = glm::acos(glm::dot(b.normal,a.normal));
	
	//exit if values lead to infinities
	if(glm::sin(a_temp)==0.0f || glm::sin(b_temp)==0.0f|| glm::sin(c_temp)==0.0f){
		return;
	}


	//Law of cosines on the sphere
	alpha = glm::acos((glm::cos(a_temp)-(glm::cos(b_temp)*glm::cos(c_temp)))/(glm::sin(b_temp)*glm::sin(c_temp)));
	beta = glm::acos((glm::cos(b_temp)-(glm::cos(a_temp)*glm::cos(c_temp)))/(glm::sin(a_temp)*glm::sin(c_temp)));
	gamma = glm::acos((glm::cos(c_temp)-(glm::cos(b_temp)*glm::cos(a_temp)))/(glm::sin(b_temp)*glm::sin(a_temp)));

	//This follows from Gauss-Bonnet formula if we assume triangles on sphere are geodesic triangles.
	sphere_area = alpha+beta+gamma-PI;
}

//int main(char argc, char* argv[]){
//	glm::vec3 v1(1,0,0);
//	glm::vec3 v2(0,1,0);
//	glm::vec3 v3(0,0,1);
//	glm::vec3 v(0,0,0);
//
//	Triangle test_tri(LocalGeo(v1,v1),LocalGeo(v2,v2),LocalGeo(v3,v3));
//	std::cout<<test_tri.sphere_area<<std::endl;
//	std::cin.get();
//	return 0;
//}