#pragma once

#include "../physics/Body.h"

#include <array>

struct SimulationConfig
{
    // Начальные параметры трёх тел
    std::array<Body, 3> initialBodies{};

    // Фиксированный шаг физического времени
    double fixedTimeStep = 1.0 / 120.0;

    // Скорость симуляции
    double timeScale = 1.0;
};
