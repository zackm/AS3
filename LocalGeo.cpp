#include "LocalGeo.h"
#include "glm/glm.hpp"
#include <iostream>

using namespace std;

LocalGeo::LocalGeo(glm::vec3 pt,glm::vec3 nm){
	point = pt;
    
	float norm = glm::sqrt(glm::dot(nm,nm));
    
	if (norm>0){
		nm /= norm;
	}
	normal = nm;
    
	gaussian_curvature = mean_curvature = 0;
}

LocalGeo::LocalGeo(glm::vec3 pt,glm::vec3 nm, glm::vec2 uv){
	point = pt;

	param_value = uv;

	float norm = glm::sqrt(glm::dot(nm,nm));

	if (norm>0){
		nm /= norm;
	}
	normal = nm;

	gaussian_curvature = mean_curvature = 0;
}

LocalGeo::LocalGeo(glm::vec3 pt,glm::vec3 nm,glm::vec3 u_deriv,glm::vec3 v_deriv,
				   glm::vec3 uu_deriv,glm::vec3 vv_deriv, glm::vec3 uv_deriv, 
				   glm::vec3 vu_deriv, glm::vec2 uv_value){
					   point = pt;
					   normal = nm;
					   partial_u = u_deriv;
					   partial_v = v_deriv;
					   partial_uu = uu_deriv;
					   partial_vv = vv_deriv;
					   partial_uv = uv_deriv;
					   partial_vu = vu_deriv;
					   param_value = uv_value;
					   set_curvatures();
}

void LocalGeo::set_curvatures(){
	//initialize curvature to 0, then attempt to set them.
	gaussian_curvature = mean_curvature = max_curvature = min_curvature = 0.0f;

	glm::mat2 upper_second_form;

	first_form[0][0] = glm::dot(partial_u,partial_u);
	first_form[1][1] = glm::dot(partial_v,partial_v);
	first_form[0][1] = glm::dot(partial_u,partial_v); //symmetric so shouldn't need to do this.
	first_form[1][0] = glm::dot(partial_v,partial_u);

	float g = glm::determinant(first_form);

	//I think you're supposed to check this.
	if (g==0.0f){
		//check back here later.
		return;
	}

	glm::mat2 inv = glm::inverse(first_form);

	second_form[0][0] = glm::dot(partial_uu,normal);
	second_form[1][1] = glm::dot(partial_vv,normal);
	second_form[0][1] = glm::dot(partial_uv,normal); //should be symmetric, but getting different answers.
	second_form[1][0] = glm::dot(partial_vu,normal);

	upper_second_form[0][0] = second_form[0][0]*inv[0][0]+second_form[0][1]*inv[1][0];
	upper_second_form[1][1] = second_form[1][0]*inv[0][1]+second_form[1][1]*inv[1][1];
	upper_second_form[0][1] = second_form[0][0]*inv[0][1]+second_form[0][1]*inv[1][1];
	upper_second_form[1][0] = second_form[1][0]*inv[0][0]+second_form[1][1]*inv[1][0];

	//solve for principal curvatures using quadratic formula
	//might want to check discrim prior to square rooting
	float a,b,c,discrim;
	a = 1.0f;
	b = -(upper_second_form[0][0]+upper_second_form[1][1]);
	c = upper_second_form[0][0]*upper_second_form[1][1] - upper_second_form[0][1]*upper_second_form[1][0];
	discrim = b*b-(4.0f*a*c);
	
	//I think you're supposed to check this
	if (discrim<0){
		//check back here later
		return;
	}

	principal_curvatures[0] = .5f*a*(-(b)+glm::sqrt(discrim));
	principal_curvatures[1] = (1/(2.0f*a))*(-(b)-glm::sqrt(discrim));

	gaussian_curvature = principal_curvatures[0]*principal_curvatures[1];
	mean_curvature = .5f*(principal_curvatures[0]+principal_curvatures[1]);

	max_curvature = glm::max(principal_curvatures[0],principal_curvatures[1]);
	min_curvature = glm::min(principal_curvatures[0],principal_curvatures[1]);
}

//int main(char argc, char* argv[]){
//	//test for a paraboloid
//	LocalGeo test_geo;
//	test_geo.point = glm::vec3(0,0,0);
//	test_geo.partial_u = glm::vec3(1,0,0);
//	test_geo.partial_v = glm::vec3(0,1,0);
//	test_geo.partial_uu = glm::vec3(0,0,-2);
//	test_geo.partial_vv = glm::vec3(0,0,-2);
//	test_geo.partial_uv = glm::vec3(0,0,0);
//	test_geo.partial_vu = glm::vec3(0,0,0);
//	test_geo.normal = glm::vec3(0,0,1);
//
//	test_geo.set_curvatures();
//
//	cout<<test_geo.gaussian_curvature<<',';
//	cout<<test_geo.mean_curvature<<',';
//	cout<<test_geo.max_curvature<<',';
//	cout<<test_geo.min_curvature<<endl;
//	cin.get();
//}