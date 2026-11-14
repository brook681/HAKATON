
#pragma once

#include "Body.h"
#include <vector>

class Scenarios
{
public:
    static std::vector<Body> createFigureEight();
    static std::vector<Body> createChaotic();
    static std::vector<Body> createHierarchical();
};
