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

//vector<glm::vec3> BezierPatch::u_curve(int i){
//	return glm::vec4(patch[i][0],patch[i][1],patch[i][2],patch[i][3]);
//}

//vector<glm::vec3> BezierPatch::v_curve(int i){
//	return glm::vec4(patch[0][i],patch[1][i],patch[2][i],patch[3][i]);
//}