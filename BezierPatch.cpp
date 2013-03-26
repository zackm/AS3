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

void BezierPatch::subdivide_patch(float step) {
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

void BezierPatch::make_tri_list() {
    for (int i = 0; i < geo_list.size(); i++) {
        if (i%5 == 0) {
            if (i+5 < 25) {
                glm::vec3 tri(i+1,i,i+5);
                tri_list.push_back(tri);
            }
        } else if (i%5 == 4) {
            if (i - 5 >= 0) {
                glm::vec3 tri(i-1,i,i-5);
                tri_list.push_back(tri);
            }
        } else {
            if (i+5 < 25) {
                glm::vec3 tri(i+1,i,i+5);
                tri_list.push_back(tri);
            }
            if (i - 5 >= 0) {
                glm::vec3 tri(i-1,i,i-5);
                tri_list.push_back(tri);
            }
        }
    }
}