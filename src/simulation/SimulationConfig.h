
#pragma once

#include "../physics/Body.h"

#include <vector>

struct SimulationConfig
{
    // Начальные параметры тел
    std::vector<Body> initialBodies{};

    // Фиксированный шаг физического времени
    double fixedTimeStep = 1.0 / 120.0;

    // Скорость симуляции
    double timeScale = 1.0;
};
