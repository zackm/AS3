#pragma once
#include "glm/glm.hpp"

#pragma once
#include <vector>

using namespace std;

class BezierPatch{
public:
    vector<vector<glm::vec3> > patch;

	BezierPatch(void);
	~BezierPatch(void);
    
    void push_back(vector<glm::vec3>);

    int size();
    
	void curve_interp(float,float*,float*,glm::vec4);
	void patch_interp(float,float,glm::vec3*,glm::vec3*);
    
//	vector<glm::vec3> u_curve(int);
//	vector<glm::vec3> v_curve(int);
};

