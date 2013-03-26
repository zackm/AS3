//
//  LocalTangent.h
//  AS3
//
//  Created by Zack Mayeda on 3/25/13.
//  Copyright (c) 2013 Zack Mayeda. All rights reserved.
//

#ifndef __AS3__LocalTangent__
#define __AS3__LocalTangent__

#include <iostream>

#endif /* defined(__AS3__LocalTangent__) */

#pragma once
#include "glm/glm.hpp"

#pragma once
#include <vector>


class LocalTangent {
public:
    glm::vec3 point;
    glm::vec3 deriv;
    
    LocalTangent(void){};
    LocalTangent(glm::vec3, glm::vec3);
};