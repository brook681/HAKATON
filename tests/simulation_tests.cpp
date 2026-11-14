
#include "../src/simulation/Simulation.h"

#include <iostream>
#include <cmath>
#include <stdexcept>

int passed = 0;
int failed = 0;

void check(bool condition, const char* testName)
{
    if (condition)
    {
        std::cout << "[PASS] " << testName << '\n';
        ++passed;
    }
    else
    {
        std::cout << "[FAIL] " << testName << '\n';
        ++failed;
    }
}

SimulationConfig createTestConfig()
{
    SimulationConfig config;

    config.fixedTimeStep = 1.0 / 120.0;
    config.timeScale = 1.0;

    config.initialBodies[0].mass = 1.0;
    config.initialBodies[0].position = {-10.0, 0.0};
    config.initialBodies[0].velocity = {0.0, 0.0};
    config.initialBodies[0].radius = 0.1;

    config.initialBodies[1].mass = 1.0;
    config.initialBodies[1].position = {10.0, 0.0};
    config.initialBodies[1].velocity = {0.0, 0.0};
    config.initialBodies[1].radius = 0.1;

    config.initialBodies[2].mass = 1.0;
    config.initialBodies[2].position = {0.0, 10.0};
    config.initialBodies[2].velocity = {0.0, 0.0};
    config.initialBodies[2].radius = 0.1;

    return config;
}

int main()
{
    {
        Simulation simulation(createTestConfig());

        check(
            simulation.getState() == SimulationState::Paused,
            "Initial state is Paused"
        );
    }

    {
        Simulation simulation(createTestConfig());

        simulation.start();

        check(
            simulation.isRunning(),
            "Simulation starts"
        );
    }

    {
        Simulation simulation(createTestConfig());

        simulation.start();
        simulation.pause();

        check(
            simulation.getState() == SimulationState::Paused,
            "Simulation pauses"
        );
    }

    {
        Simulation simulation(createTestConfig());

        simulation.start();
        simulation.update(1.0 / 30.0);

        check(
            std::abs(simulation.getSimulationTime() - 1.0 / 30.0) < 1e-10,
            "Fixed timestep accumulator"
        );
    }

    {
        Simulation simulation(createTestConfig());

        simulation.start();
        simulation.update(0.1);

        simulation.reset();

        check(
            simulation.getState() == SimulationState::Paused &&
            simulation.getSimulationTime() == 0.0,
            "Reset restores initial state and time"
        );
    }

    {
        Simulation simulation(createTestConfig());

        simulation.start();
        simulation.update(0.1);

        const double timeBeforePause = simulation.getSimulationTime();

        simulation.pause();
        simulation.update(1.0);

        check(
            simulation.getSimulationTime() == timeBeforePause,
            "Paused simulation does not advance"
        );
    }

    {
        Simulation simulation(createTestConfig());

        simulation.setTimeScale(2.0);
        simulation.start();
        simulation.update(0.1);

        check(
            std::abs(simulation.getSimulationTime() - 0.2) < 1e-10,
            "Time scale 2x"
        );
    }

    std::cout << "\nPassed: " << passed
              << "\nFailed: " << failed << '\n';

    return failed == 0 ? 0 : 1;
}
