
#pragma once

#include <array>
#include <cstddef>

#include "SimulationConfig.h"
#include "../physics/Physics.h"

enum class SimulationState
{
    Paused,
    Running,
    Finished
};

class Simulation
{
public:
    explicit Simulation(const SimulationConfig& config);

    void start();
    void pause();
    void reset();

    void update(double deltaTime);

    void setTimeScale(double scale);

    bool isRunning() const;

    SimulationState getState() const;

    const std::array<Body, 3>& getBodies() const;

    double getSimulationTime() const;

private:
    // Maximum number of physics updates per frame.
    static constexpr std::size_t MAX_SUBSTEPS_PER_FRAME = 8;

    SimulationConfig config;

    Physics physics;

    std::array<Body, 3> bodies{};

    SimulationState state = SimulationState::Paused;

    double simulationTime = 0.0;

    double accumulator = 0.0;

    double timeScale = 1.0;
};
