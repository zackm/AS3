#include "BezierPatch.h"


BezierPatch::BezierPatch(void)
{
    
}


BezierPatch::~BezierPatch(void)
{
}

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
        vcurve[i] = curve_interp(v_curve(i), v).point;
        ucurve[i] = curve_interp(u_curve(i), u).point;
    }
    LocalTangent u_local, v_local;
    v_local = curve_interp(vcurve, v);
    u_local = curve_interp(ucurve, u);
    
    glm::vec3 n = glm::cross(u_local.deriv, v_local.deriv);
    if (glm::dot(n,n) != 0.0f) {
        n = n / glm::sqrt(glm::dot(n,n));
    }
    
    // Assuming v_local.point and u_local.point are equal
    return LocalGeo(v_local.point,n);
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