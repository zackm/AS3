#include "BezierPatch.h"


void BezierPatch::push_back(vector<glm::vec3> new_vec) {
	patch.push_back(new_vec);
}

int BezierPatch::size() {
	return patch.size();
}

LocalTangent BezierPatch::curve_interp(BezierCurve bez_cur,float u) {
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

LocalGeo BezierPatch::patch_interp(float u, float v) {
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
	LocalGeo l_geo(v_local.point,n);
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

void BezierPatch::add_geo(LocalGeo geo) {
	geo_list.push_back(geo);
}

void BezierPatch::uniform_subdivide(float step) {
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
}

void BezierPatch::adaptive_subdivide(float tol){
	/*start with the point u = 0,1 and v = 0,1. Make two
	triangles out of these four points. Then add values 
	for u and v depending on how the flat portion compares 
	to the bezier patch. I think we need to use barycentric 
	coordinates for this to make sense.

	We could have adpative_subdivide call a recursive helper
	function which only looks at four points.
	*/

	//LocalGeo current_geo;

	//for (int i = 0; i<=1; i++){
	//	for (int j = 0; j<=1; j++){
	//		current_geo = patch_interp(i,j);
	//		add_geo(current_geo);
	//	}
	//}

	////two triangles, so just make queue manually. We insert
	////a triangle into list once it meets the tolerance requirement

	//glm::vec3 tri1(0,2,1);
	//glm::vec3 tri2(1,2,3);

	//tri_queue.push(tri1);
	//tri_queue.push(tri2);

	//glm::vec3 current_tri;
	//int geo_count = 4;
	//while (!tri_queue.empty()){
	//	//pop tri. Test it. Either, add to tri list, or subdivide and add to queue.

	//	current_tri = tri_queue.pop();

	//	if (below tolerance){
	//		//add to try list
	//	}else{
	//		//subdivide and add to end of queue
	//	}
	//}

	//float step = 1.0;
	//uniform_subdivide(step);
	//make_tri_list(step);



}

void BezierPatch::make_tri_list(float step) {
	//this function is only valid for the uniform subdivision routine.
	int row = glm::floor((1 / step) + 1);
	float block = row * row;

	for (int i = 0; i < geo_list.size(); i++) {
		if (i%row == 0) {
			if (i+row < block) {
				glm::vec3 tri(i+1,i,i+row);
				tri_list.push_back(tri);
			}
		} else if (i%row == row - 1) {
			if (i - row >= 0) {
				glm::vec3 tri(i-1,i,i-row);
				tri_list.push_back(tri);
			}
		} else {
			if (i+row < block) {
				glm::vec3 tri(i+1,i,i+row);
				tri_list.push_back(tri);
			}
			if (i - row >= 0) {
				glm::vec3 tri(i-1,i,i-row);
				tri_list.push_back(tri);
			}
		}
	}
}