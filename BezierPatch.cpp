#include "BezierPatch.h"


void BezierPatch::push_back(vector<glm::vec3> new_vec){
	patch.push_back(new_vec);
}

int BezierPatch::size() {
	return patch.size();
}

glm::vec3 BezierPatch::deCasteljau(BezierCurve bez_cur,float u){
	int n = bez_cur.number_points();
	if (n==2){
		glm::vec3 interp_point = bez_cur[0]*(1.0f-u)+bez_cur[1]*u;
		return interp_point;
	}else{
		vector<glm::vec3> new_curve;
		for(int i = 0; i<n-1; i++){
			new_curve.push_back(bez_cur[i]*(1.0f-u)+bez_cur[i+1]*u);
		}
		return deCasteljau(BezierCurve(new_curve),u);
	}
}

LocalTangent BezierPatch::curve_interp(BezierCurve bez_cur,float u){
	glm::vec3 p = deCasteljau(bez_cur,u);

	float degree = bez_cur.number_points();
	//generate the first derivative curve.
	vector<glm::vec3> deriv_points;
	glm::vec3 d_point;
	for(int i = 0; i<bez_cur.number_points()-1; i++){
		d_point = (degree-1)*(bez_cur[i+1]-bez_cur[i]);
		deriv_points.push_back(d_point);
	}

	BezierCurve first_deriv(deriv_points);

	glm::vec3 dPdu = deCasteljau(first_deriv,u);

	LocalTangent l_tan(p,dPdu);
	return l_tan;
}

LocalGeo BezierPatch::patch_interp(float u, float v){
	LocalGeo l_geo;

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

	l_geo.param_value = glm::vec2(u,v);
	l_geo.point = u_local.point;
	l_geo.normal = n;
	l_geo.partial_u = u_local.deriv;
	l_geo.partial_v = v_local.deriv;
	
	//now we generate the first derivative curve in order to get the second derive with u and v.
	vector<glm::vec3> v_deriv_points;
	glm::vec3 d_point;
	for(int i = 0; i<vcurve.number_points()-1; i++){
		d_point = 3.0f*(vcurve[i+1]-vcurve[i]);
		v_deriv_points.push_back(d_point);
	}
	BezierCurve v_deriv(v_deriv_points);

	//now for u
	vector<glm::vec3> u_deriv_points;
	for(int i = 0; i<ucurve.number_points()-1; i++){
		d_point = 3.0f*(ucurve[i+1]-ucurve[i]);
		u_deriv_points.push_back(d_point);
	}
	BezierCurve u_deriv(u_deriv_points);

	LocalTangent second_u,second_v,mixed_partial_uv,mixed_partial_vu;

	second_u = curve_interp(u_deriv,u);
	second_v = curve_interp(v_deriv,v);
	mixed_partial_uv = curve_interp(u_deriv,v);
	mixed_partial_vu = curve_interp(v_deriv,u);

	l_geo.partial_uu = second_u.deriv;
	l_geo.partial_vv = second_v.deriv;
	l_geo.partial_uv = mixed_partial_uv.deriv;
	l_geo.partial_vu = mixed_partial_vu.deriv;
	l_geo.set_curvatures();
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