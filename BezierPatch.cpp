#include "BezierPatch.h"

void BezierPatch::push_back(vector<glm::vec3> new_vec){
	patch.push_back(new_vec);
}

int BezierPatch::size() {
	return patch.size();
}

void BezierPatch::add_geo(LocalGeo geo){
	geo_list.push_back(geo);
}

/*
We interpolate the patch at the input (u,v). The patch is defined in terms of bernstein polynomials
We have the patch:

B = summation over i and j (Pij * Bi * Bj)

Where the Pij are input control points, the Bi and Bj are just the 
following bernstein polynomials.

Bernstein polnomial:
B0 = (1-x)^3;
B1 = 3x(1-x)^2;
B2 = 3x^2 (1-x);
B3 = x^3;
*/
LocalGeo BezierPatch::patch_interp(float u, float v){
	//Bernstein polynomials
	float b_i[4] = {(1-u)*(1-u)*(1-u), 3.0f*u*(1-u)*(1-u), 3*u*u*(1-u), u*u*u};
	float b_j[4] = {(1-v)*(1-v)*(1-v), 3.0f*v*(1-v)*(1-v), 3*v*v*(1-v), v*v*v};

	//First partial derivatives of Bernstein polynomials
	float b_i_u[4] = {-3.0f*(1-u)*(1-u), 9.0f*u*u - 12.0f*u + 3, 6.0f*u - 9.0f*u*u, 3*u*u};
	float b_j_v[4] = {-3.0f*(1-v)*(1-v), 9.0f*v*v - 12.0f*v + 3, 6.0f*v - 9.0f*v*v, 3*v*v};

	//Second partial derivatives of Bernstein polynomials. We don't need the mixed partials with respect
	//to both u and v because each bernstein polynomial is a function of a single variable
	float b_i_uu[4] = {6.0f*(1-u), 18.0f*u - 12.0f, 6.0f - 18.0f*u, 6.0f*u};
	float b_j_vv[4] = {6.0f*(1-v), 18.0f*v - 12.0f, 6.0f - 18.0f*v, 6.0f*v};

	glm::vec3 deriv_u,deriv_v,deriv_uu,deriv_vv,deriv_uv,point,normal;

	point = deriv_u = deriv_v = deriv_uu = deriv_vv = deriv_uv = glm::vec3(0,0,0);
	//sum over the points as in the definition of the Bezier surface. We need to do
	//this for all derivatives as well.
	for (int i = 0; i<4; i++){
		for (int j = 0; j<4; j++){
			point += patch[i][j]*(b_i[i])*(b_j[j]);
			deriv_u += patch[i][j]*(b_i_u[i])*(b_j[j]);
			deriv_v += patch[i][j]*(b_i[i])*(b_j_v[j]);
			deriv_uu += patch[i][j]*(b_i_uu[i])*(b_j[j]);
			deriv_vv += patch[i][j]*(b_i[i])*(b_j_vv[j]);
			deriv_uv += patch[i][j]*(b_i_u[i])*(b_j_v[j]);
		}
	}
	glm::vec3 n = glm::cross(deriv_u,deriv_v);

	float norm = glm::dot(n,n);
	if (norm>0){
		n /= glm::sqrt(norm);
	}

	//we only need deriv_uv because we assume derive_uv = deriv_vu
	LocalGeo l_geo(point,n,deriv_u,deriv_v,deriv_uu,deriv_vv,deriv_uv,deriv_uv,glm::vec2(u,v));
	return l_geo;
}

/******************************
* Uniform Subdivision Methods *
******************************/
/*
This method comes from the Lecture 12 - Surfaces pseudocode by James O'Brien.
It simply evaluates the BezierPatch at each possible (u,v) value starting
from 0 and moving in a stepsize of @param step.
*/
void BezierPatch::uniform_subdivide(float step){
	float epsilon = 0.001f;
	float numdiv = ((1 + epsilon) / step);
	LocalGeo current_geo;
	for (int iu = 0; iu < numdiv; iu++) {
		float u = iu * step;

		for (int iv = 0; iv < numdiv; iv++) {
			float v = iv * step;
			current_geo = patch_interp(u,v);
			add_geo(current_geo);
		}
	}
	make_tri_list_uniform(step);
}

/*
Helper method for uniform_subdivide to take in the list of LocalGeo objects
and then make a list of triangles.
*/
void BezierPatch::make_tri_list_uniform(float step){
	//this function is only valid for the uniform subdivision routine.
	int row = glm::floor((1 / step) + 1);
	float block = row * row;

	Triangle* current_tri;

	for (int i = 0; i < geo_list.size(); i++) {
		if (i%row == 0) {
			if (i+row < block) {
				current_tri = new Triangle(geo_list[i+1],geo_list[i],geo_list[i+row]);
				tri_list.push_back(*current_tri);
			}
		} else if (i%row == row - 1) {
			if (i - row >= 0) {
				current_tri = new Triangle(geo_list[i-1],geo_list[i],geo_list[i-row]);
				tri_list.push_back(*current_tri);
			}
		} else {
			if (i+row < block) {
				current_tri = new Triangle(geo_list[i+1],geo_list[i],geo_list[i+row]);
				tri_list.push_back(*current_tri);
			}
			if (i - row >= 0) {
				current_tri = new Triangle(geo_list[i-1],geo_list[i],geo_list[i-row]);
				tri_list.push_back(*current_tri);
			}
		}
	}
}

/*******************************
* Adaptive Subdivision Methods *
*******************************/
/*
Start with the point u = 0,1 and v = 0,1. Make two
triangles out of these four points. Then add values 
for u and v depending on how the flat portion compares 
to the bezier patch. Uses @param tol to determine if 
the triangle approximates the bezier surface well enough.
We also implement a max depth of 5 for subdivision in
order to not loop infinitely if tolerance is never met.
*/
void BezierPatch::adaptive_subdivide(float tol){
	LocalGeo current_geo;
	const int max_iterations = 5;
	int iterations_left;

	for (int i = 0; i<=1; i++){
		for (int j = 0; j<=1; j++){
			current_geo = patch_interp(i,j);
			add_geo(current_geo);
		}
	}

	//We have two triangles, so just make queue manually. We insert
	//a triangle into tri_list once it meets the tolerance requirement

	Triangle tri1(geo_list[0],geo_list[2],geo_list[1],max_iterations);
	Triangle tri2(geo_list[1],geo_list[2],geo_list[3],max_iterations);

	//Add triangles that need to be checked to the queue.
	tri_queue.push(tri1);
	tri_queue.push(tri2);

	LocalGeo a,b,c,ab_geo,ac_geo,bc_geo;
	glm::vec2 uv_value;
	glm::vec3 ab_midpoint,ac_midpoint,bc_midpoint;
	Triangle current_tri;

	bool ab_split = false, ac_split = false, bc_split = false;

	glm::vec3 difference;
	float distance;

	while (!tri_queue.empty()){
		//pop triangle. Test it for flatness. Either, add to tri list, or subdivide and add to queue.
		Triangle current_tri = tri_queue.front();
		tri_queue.pop();
		iterations_left = current_tri.divisions_remaining;

		//reset booleans for this triangle;
		ab_split = ac_split = bc_split = false;

		//check split on edge a to b
		a = current_tri.a;
		b = current_tri.b;

		uv_value = .5f*(a.param_value+b.param_value); //interpolate param value
		ab_midpoint = a.point+(.5f*(b.point-a.point));
		ab_geo = patch_interp(uv_value[0],uv_value[1]);

		difference = ab_geo.point - ab_midpoint;
		distance = glm::sqrt(glm::dot(difference,difference));

		if (distance>=tol && iterations_left>0){
			//then we need to split along a,b.
			ab_split = true;
		}

		//check split on edge a to c
		a = current_tri.a;
		c = current_tri.c;

		uv_value = .5f*(a.param_value+c.param_value); //interpolate param value
		ac_midpoint = a.point+(.5f*(c.point-a.point));
		ac_geo = patch_interp(uv_value[0],uv_value[1]);

		difference = ac_geo.point - ac_midpoint;
		distance = glm::sqrt(glm::dot(difference,difference));

		if (distance>=tol && iterations_left>0){
			//then we need to split along a,c.
			ac_split = true;
		}

		//check split on edge b to c
		b = current_tri.b;
		c = current_tri.c;

		uv_value = .5f*(b.param_value+c.param_value); //interpolate param value
		bc_midpoint = b.point+(.5f*(c.point-b.point));
		bc_geo = patch_interp(uv_value[0],uv_value[1]);

		difference = bc_geo.point - bc_midpoint;
		distance = glm::sqrt(glm::dot(difference,difference));

		if (distance>=tol && iterations_left>0){
			//then we need to split along b,c.
			bc_split = true;
		}

		//We have checked all sides of triangle. Now we need to add triangle to final list
		//or split and add the split triangles into the queue. These 8 splits cases come from
		//Lecture 12 - Surfaces, pg. 29 in James O'Brien's lecture notes.
		if (!ab_split){
			if(!ac_split){
				if(!bc_split){
					//add current triangle to the final list. No split necessary.
					tri_list.push_back(current_tri);
				}else{
					add_geo(bc_geo);
					//split along bc.
					Triangle tri1(bc_geo,b,a,iterations_left-1);
					Triangle tri2(bc_geo,a,c,iterations_left-1);

					tri_queue.push(tri1);
					tri_queue.push(tri2);

				}
			}else{
				add_geo(ac_geo);
				if(!bc_split){
					//split along ac
					Triangle tri1(ac_geo,b,a,iterations_left-1);
					Triangle tri2(ac_geo,c,b,iterations_left-1);

					tri_queue.push(tri1);
					tri_queue.push(tri2);
				}else{
					add_geo(bc_geo);
					//need to split ac and bc
					Triangle tri1(bc_geo,b,a,iterations_left-1);
					Triangle tri2(bc_geo,a,ac_geo,iterations_left-1);
					Triangle tri3(ac_geo,c,bc_geo,iterations_left-1);

					tri_queue.push(tri1);
					tri_queue.push(tri2);
					tri_queue.push(tri3);
				}
			}
		}else{
			add_geo(ab_geo);
			if(!ac_split){
				if(!bc_split){
					//split along ab
					Triangle tri1(ab_geo,c,b,iterations_left-1);
					Triangle tri2(ab_geo,a,c,iterations_left-1);

					tri_queue.push(tri1);
					tri_queue.push(tri2);
				}else{
					add_geo(bc_geo);
					//need to split on ab and on bc
					Triangle tri1(ab_geo,a,c,iterations_left-1);
					Triangle tri2(ab_geo,c,bc_geo,iterations_left-1);
					Triangle tri3(ab_geo,bc_geo,b,iterations_left-1);

					tri_queue.push(tri1);
					tri_queue.push(tri2);
					tri_queue.push(tri3);
				}
			}else{
				add_geo(ac_geo);
				if(!bc_split){
					//need to split on ab and ac
					Triangle tri1(ac_geo,c,b,iterations_left-1);
					Triangle tri2(ab_geo,ac_geo,b,iterations_left-1);
					Triangle tri3(ab_geo,a,ac_geo,iterations_left-1);

					tri_queue.push(tri1);
					tri_queue.push(tri2);
					tri_queue.push(tri3);
				}else{
					add_geo(bc_geo);
					//need to split on all of them. Produces four triangles
					Triangle tri1(a,ac_geo,ab_geo,iterations_left-1);
					Triangle tri2(b,ab_geo,bc_geo,iterations_left-1);
					Triangle tri3(c,bc_geo,ac_geo,iterations_left-1);
					Triangle tri4(ab_geo,ac_geo,bc_geo,iterations_left-1);

					tri_queue.push(tri1);
					tri_queue.push(tri2);
					tri_queue.push(tri3);
					tri_queue.push(tri4);
				}
			}
		}
	}
}