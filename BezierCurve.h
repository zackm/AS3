//
//  BezierCurve.h
//  AS3
//
//  Created by Zack Mayeda on 3/25/13.
//

#ifndef __AS3__BezierCurve__
#define __AS3__BezierCurve__

#include <iostream>

#endif /* defined(__AS3__BezierCurve__) */

#pragma once
#include "glm/glm.hpp"

#pragma once
#include <vector>

using namespace std;

class BezierCurve {
public:
    vector<glm::vec3> points;
    
    glm::vec3 operator[](int) const;
    glm::vec3 & operator[](int);
    
    BezierCurve(void);
    BezierCurve(vector<glm::vec3>);
};