#pragma once
#include "glm/glm.hpp"

#pragma once
#include <vector>

#pragma once
#include <queue>

#pragma once
#include "LocalGeo.h"

#pragma once
#include "Triangle.h"

using namespace std;

class BezierPatch{
public:
	BezierPatch(void){};
	~BezierPatch(void){};
    vector<vector<glm::vec3> > patch;
    vector<LocalGeo> geo_list; //list of full geometry
    vector<Triangle> tri_list; //represents the final list of fully subdivided triangles
	int size();
    
    void push_back(vector<glm::vec3>);
	LocalGeo patch_interp(float,float);
    void add_geo(LocalGeo);
    void uniform_subdivide(float);
	void adaptive_subdivide(float);

private:
	void make_tri_list_uniform(float); //helper method for uniform subdivision
	queue<Triangle> tri_queue; //holds triangles that need to be checked for subdivision.
};

