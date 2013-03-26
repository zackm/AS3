//
//  BezierCurve.cpp
//  AS3
//

#include "BezierCurve.h"

using namespace std;

BezierCurve::BezierCurve() {
    glm::vec3 empty(0.0f,0.0f,0.0f);
    points.push_back(empty);
    points.push_back(empty);
    points.push_back(empty);
    points.push_back(empty);
}

BezierCurve::BezierCurve(vector<glm::vec3> vec_arg) {
    points = vec_arg;
}

glm::vec3 BezierCurve::operator[](int pos) const {
    return points[pos];
}

glm::vec3 & BezierCurve::operator[](int pos) {
    return points[pos];
}