#include "BezierPatch.h"


void BezierPatch::push_back(vector<glm::vec3> new_vec){
	patch.push_back(new_vec);
}

int BezierPatch::size() {
	return patch.size();
}

LocalTangent BezierPatch::curve_interp(BezierCurve bez_cur,float u){
	glm::vec3 A,B,C,D,E,p,dPdu;
	A = bez_cur[0] * (1.0f - u) + bez_cur[1] * u;
	B = bez_cur[1] * (1.0f - u) + bez_cur[2] * u;
	C = bez_cur[2] * (1.0f - u) + bez_cur[3] * u;

	D = A * (1.0f - u) + B * u;
	E = B * (1.0f - u) + C * u;

	p = D * (1.0f - u) + E * u;

	dPdu = 3.0f * (E - D);

	LocalTangent l_tan(p,dPdu);
	return l_tan;
}

LocalGeo BezierPatch::patch_interp(float u, float v){
	BezierCurve vcurve, ucurve;
	for (int i = 0; i < 4; i++) {
		vcurve[i] = curve_interp(v_curve(i), u).point;
		ucurve[i] = curve_interp(u_curve(i), v).point;
	}
	LocalTangent u_local, v_local;
	v_local = curve_interp(vcurve, v);
	u_local = curve_interp(ucurve, u);

	glm::vec3 n = glm::cross(u_local.deriv, v_local.deriv);
	if (glm::dot(n,n) != 0.0f) {
		n = n / glm::sqrt(glm::dot(n,n));
	}

	// Assuming v_local.point and u_local.point are equal
	LocalGeo l_geo(v_local.point,n, glm::vec2(u,v));
	return l_geo;
}

BezierCurve BezierPatch::u_curve(int i){
	vector<glm::vec3> cur;
	cur.push_back(patch[i][0]);
	cur.push_back(patch[i][1]);
	cur.push_back(patch[i][2]);
	cur.push_back(patch[i][3]);
	return BezierCurve(cur);
}

BezierCurve BezierPatch::v_curve(int i){
	vector<glm::vec3> cur;
	cur.push_back(patch[0][i]);
	cur.push_back(patch[1][i]);
	cur.push_back(patch[2][i]);
	cur.push_back(patch[3][i]);
	return BezierCurve(cur);
}

void BezierPatch::add_geo(LocalGeo geo){
	geo_list.push_back(geo);
}

/******************************
* Uniform Subdivision Methods *
******************************/
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
void BezierPatch::adaptive_subdivide(float tol){
	/*start with the point u = 0,1 and v = 0,1. Make two
	triangles out of these four points. Then add values 
	for u and v depending on how the flat portion compares 
	to the bezier patch. I think we need to use barycentric 
	coordinates for this to make sense.

	We could have adpative_subdivide call a recursive helper
	function which only looks at four points.
	*/

	LocalGeo current_geo;

	for (int i = 0; i<=1; i++){
		for (int j = 0; j<=1; j++){
			current_geo = patch_interp(i,j);
			add_geo(current_geo);
		}
	}

	//two triangles, so just make queue manually. We insert
	//a triangle into list once it meets the tolerance requirement

	Triangle tri1(geo_list[0],geo_list[2],geo_list[1]);
	Triangle tri2(geo_list[1],geo_list[2],geo_list[3]);

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
		//cout<<tri_queue.size()<<endl;
		//pop tri. Test it. Either, add to tri list, or subdivide and add to queue.
		Triangle current_tri = tri_queue.front();
		tri_queue.pop();

		//check split on a to b
		a = current_tri.a;
		b = current_tri.b;

		uv_value = .5f*(a.param_value+b.param_value); //interpolate param value
		ab_midpoint = a.point+(.5f*(b.point-a.point));
		ab_geo = patch_interp(uv_value[0],uv_value[1]);

		difference = ab_geo.point - ab_midpoint;
		distance = glm::sqrt(glm::dot(difference,difference));

		if (distance>=tol){
			//then we need to split along a,b.
			ab_split = true;
		}

		//check split on a to c
		a = current_tri.a;
		c = current_tri.c;

		uv_value = .5f*(a.param_value+c.param_value); //interpolate param value
		ac_midpoint = a.point+(.5f*(c.point-a.point));
		ac_geo = patch_interp(uv_value[0],uv_value[1]);

		difference = ac_geo.point - ac_midpoint;
		distance = glm::sqrt(glm::dot(difference,difference));

		if (distance>=tol){
			//then we need to split along a,c.
			ac_split = true;
		}

		//check split on b to c
		b = current_tri.b;
		c = current_tri.c;

		uv_value = .5f*(b.param_value+c.param_value); //interpolate param value
		bc_midpoint = b.point+(.5f*(c.point-b.point));
		bc_geo = patch_interp(uv_value[0],uv_value[1]);

		difference = bc_geo.point - bc_midpoint;
		distance = glm::sqrt(glm::dot(difference,difference));

		if (distance>=tol){
			//then we need to split along b,c.
			bc_split = true;
		}

		if (!ab_split){
			if(!ac_split){
				if(!bc_split){
					//add current triangle to the final list. No split necessary.
					tri_list.push_back(current_tri);
				}else{
					//split along bc.
					Triangle tri1(bc_geo,b,a);
					Triangle tri2(bc_geo,a,c);

					tri_queue.push(tri1);
					tri_queue.push(tri2);
				}
			}else{
				if(!bc_split){
					//split along ac
					Triangle tri1(ac_geo,b,a);
					Triangle tri2(ac_geo,c,b);

					tri_queue.push(tri1);
					tri_queue.push(tri2);
				}else{
					//need to split ac and bc
					Triangle tri1(bc_geo,b,a);
					Triangle tri2(bc_geo,a,ac_geo);
					Triangle tri3(ac_geo,c,bc_geo);

					tri_queue.push(tri1);
					tri_queue.push(tri2);
					tri_queue.push(tri3);
				}
			}
		}else{
			if(!ac_split){
				if(!bc_split){
					//split along ab
					Triangle tri1(ab_geo,c,b);
					Triangle tri2(ab_geo,a,c);

					tri_queue.push(tri1);
					tri_queue.push(tri2);
				}else{
					//need to split on ab and on bc
					Triangle tri1(ab_geo,a,c);
					Triangle tri2(ab_geo,c,bc_geo);
					Triangle tri3(ab_geo,bc_geo,b);

					tri_queue.push(tri1);
					tri_queue.push(tri2);
					tri_queue.push(tri3);
				}
			}else{
				if(!bc_split){
					//need to split on ab and ac
					Triangle tri1(ac_geo,c,b);
					Triangle tri2(ab_geo,ac_geo,b);
					Triangle tri3(ab_geo,a,ac_geo);

					tri_queue.push(tri1);
					tri_queue.push(tri2);
					tri_queue.push(tri3);
				}else{
					//need to split on all of them. Produces four triangles
					Triangle tri1(a,ac_geo,ab_geo);
					Triangle tri2(b,ab_geo,bc_geo);
					Triangle tri3(c,bc_geo,ac_geo);
					Triangle tri4(ab_geo,ac_geo,bc_geo);

					tri_queue.push(tri1);
					tri_queue.push(tri2);
					tri_queue.push(tri3);
					tri_queue.push(tri4);
				}
			}
		}
		ab_split = ac_split = bc_split = false;
	}
}