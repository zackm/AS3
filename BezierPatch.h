#pragma once
#include "glm/glm.hpp"

#pragma once
#include <vector>

#pragma once
#include <queue>

#pragma once
#include "LocalTangent.h"

#pragma once
#include "BezierCurve.h"

#pragma once
#include "LocalGeo.h"

using namespace std;

class BezierPatch{
public:
    vector<vector<glm::vec3> > patch;
    vector<LocalGeo> geo_list;
    vector<glm::vec3> tri_list;

	queue<glm::vec3> tri_queue;
	queue<LocalGeo> geo_queue;

	BezierPatch(void){};
	~BezierPatch(void){};
    
    void push_back(vector<glm::vec3>);

    int size();
    
	LocalTangent curve_interp(BezierCurve,float);
	LocalGeo patch_interp(float,float);
	BezierCurve u_curve(int);
	BezierCurve v_curve(int);
    void add_geo(LocalGeo);
    void uniform_subdivide(float);
    void make_tri_list(float);

	void adaptive_subdivide(float);
};

