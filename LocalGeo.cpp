#include "LocalGeo.h"
#include "glm/glm.hpp"

LocalGeo::LocalGeo(glm::vec3 pt,glm::vec3 nm){
	point = pt;
    
	float norm = glm::sqrt(glm::dot(nm,nm));
    
	if (norm>0){
		nm /= norm;
	}
	normal = nm;
    
	gaussian_curvature = mean_curvature = max_curvature = min_curvature = 0;
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

/*
Sets curvatures using the g_{ij} and L_{ij}. Specifically, we need to get the
L^{ij} coefficients which make up the elements of the Weingarten map. We then
calculate the eigenvalues k_1,k_2 of the Weingarten map and calculate the 
Gaussian curvature by k_1*k_2 and Mean curvature by .5*(k1+k2).

To get the coefficients we do:
	g_{ij} = dot(x_i,x_j)
	
and

	L_{ij} = dot(x_{ij},n)

where n is the normal.

And, to get L^{ij} we do:

	L^{ij} = sum on k(L_{ik}*g^{kj})

where the g^{kj} are elements of the inverse of the metric matrix g_{li}.

Then we just calculate the eigen values of the L^{ij} matrix.
*/
void LocalGeo::set_curvatures(){
	//initialize curvature to 0, then attempt to set them.
	gaussian_curvature = mean_curvature = max_curvature = min_curvature = 0.0f;

	glm::mat2 upper_second_form;

	//The g_{ij}
	first_form[0][0] = glm::dot(partial_u,partial_u);
	first_form[1][1] = glm::dot(partial_v,partial_v);
	first_form[0][1] = glm::dot(partial_u,partial_v); //symmetric so shouldn't need to do this.
	first_form[1][0] = glm::dot(partial_v,partial_u);

	float g = glm::determinant(first_form);

	if (g==0.0f){
		return;
	}

	//The g^{ij}.
	glm::mat2 inv = glm::inverse(first_form);

	//The L_{ij}
	second_form[0][0] = glm::dot(partial_uu,normal);
	second_form[1][1] = glm::dot(partial_vv,normal);
	second_form[0][1] = glm::dot(partial_uv,normal);
	second_form[1][0] = glm::dot(partial_vu,normal);

	//The L^{ij}
	upper_second_form[0][0] = second_form[0][0]*inv[0][0]+second_form[0][1]*inv[1][0];
	upper_second_form[1][1] = second_form[1][0]*inv[0][1]+second_form[1][1]*inv[1][1];
	upper_second_form[0][1] = second_form[0][0]*inv[0][1]+second_form[0][1]*inv[1][1];
	upper_second_form[1][0] = second_form[1][0]*inv[0][0]+second_form[1][1]*inv[1][0];

	float a,b,c,discrim;
	a = 1.0f;
	b = -(upper_second_form[0][0]+upper_second_form[1][1]);
	c = upper_second_form[0][0]*upper_second_form[1][1] - upper_second_form[0][1]*upper_second_form[1][0];
	discrim = b*b-(4.0f*a*c);
	
	if (discrim<0){
		return;
	}

	principal_curvatures[0] = .5f*a*(-(b)+glm::sqrt(discrim));
	principal_curvatures[1] = .5f*a*(-(b)-glm::sqrt(discrim));

	gaussian_curvature = principal_curvatures[0]*principal_curvatures[1];
	mean_curvature = .5f*(principal_curvatures[0]+principal_curvatures[1]);

	max_curvature = glm::max(principal_curvatures[0],principal_curvatures[1]);
	min_curvature = glm::min(principal_curvatures[0],principal_curvatures[1]);
}