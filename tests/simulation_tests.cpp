
#include "../src/simulation/Simulation.h"

#include <iostream>
#include <cmath>
#include <stdexcept>
#include <limits>

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

template <typename Function>
bool throwsInvalidArgument(Function function)
{
    try
    {
        function();
    }
    catch (const std::invalid_argument&)
    {
        return true;
    }

    return false;
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

SimulationConfig createCollisionConfig()
{
    SimulationConfig config = createTestConfig();

    config.initialBodies[0].position = {-0.15, 0.0};
    config.initialBodies[1].position = {0.15, 0.0};

    config.initialBodies[0].velocity = {1.0, 0.0};
    config.initialBodies[1].velocity = {-1.0, 0.0};

    return config;
}

int main()
{
    // 1. Initial state
    {
        Simulation simulation(createTestConfig());

        check(
            simulation.getState() == SimulationState::Paused,
            "Initial state is Paused"
        );
    }

    // 2. Start
    {
        Simulation simulation(createTestConfig());
        simulation.start();

        check(
            simulation.isRunning(),
            "Simulation starts"
        );
    }

    // 3. Pause
    {
        Simulation simulation(createTestConfig());

        simulation.start();
        simulation.pause();

        check(
            simulation.getState() == SimulationState::Paused,
            "Simulation pauses"
        );
    }

    // 4. Fixed timestep
    {
        Simulation simulation(createTestConfig());

        simulation.start();
        simulation.update(1.0 / 30.0);

        check(
            std::abs(simulation.getSimulationTime() - 1.0 / 30.0) < 1e-10,
            "Fixed timestep accumulator"
        );
    }

    // 5. Reset
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

    // 6. Time does not advance while paused
    {
        Simulation simulation(createTestConfig());

        simulation.start();
        simulation.update(0.1);

        const double timeBeforePause =
            simulation.getSimulationTime();

        simulation.pause();
        simulation.update(1.0);

        check(
            simulation.getSimulationTime() == timeBeforePause,
            "Paused simulation does not advance"
        );
    }

    // 7. Time scale 2x
    {
        SimulationConfig config = createTestConfig();

        config.fixedTimeStep = 0.01;
        config.timeScale = 2.0;

        Simulation simulation(config);
        simulation.start();
        simulation.update(0.01);

        check(
            std::abs(simulation.getSimulationTime() - 0.02) < 1e-9,
            "Time scale 2x"
        );
    }

    // 8. Maximum 8 physics steps per frame
    {
        SimulationConfig config = createTestConfig();
        config.fixedTimeStep = 0.01;

        Simulation simulation(config);
        simulation.start();

        simulation.update(0.20);

        check(
            std::abs(simulation.getSimulationTime() - 0.08) < 1e-9,
            "Maximum 8 physics steps per frame"
        );
    }

    // 9. Accumulated time is preserved
    {
        SimulationConfig config = createTestConfig();
        config.fixedTimeStep = 0.01;

        Simulation simulation(config);
        simulation.start();

        simulation.update(0.20);
        simulation.update(0.0);

        check(
            std::abs(simulation.getSimulationTime() - 0.16) < 1e-9,
            "Accumulator preserves remaining time"
        );
    }

    // 10. Collision finishes simulation
    {
        Simulation simulation(createCollisionConfig());

        simulation.start();
        simulation.update(0.1);

        check(
            simulation.getState() == SimulationState::Finished,
            "Collision finishes simulation"
        );
    }

    // 11. Finished simulation cannot restart without reset
    {
        Simulation simulation(createCollisionConfig());

        simulation.start();
        simulation.update(0.1);
        simulation.start();

        check(
            simulation.getState() == SimulationState::Finished,
            "Finished simulation cannot restart without reset"
        );
    }

    // 12. Reset after collision allows restarting
    {
        SimulationConfig config = createCollisionConfig();
        Simulation simulation(config);

        simulation.start();
        simulation.update(0.1);

        const bool finishedCorrectly =
            simulation.getState() == SimulationState::Finished;

        simulation.reset();

        const bool resetCorrectly =
            simulation.getState() == SimulationState::Paused &&
            simulation.getSimulationTime() == 0.0 &&
            simulation.getBodies()[0].position.x ==
                config.initialBodies[0].position.x &&
            simulation.getBodies()[1].position.x ==
                config.initialBodies[1].position.x;

        simulation.start();

        check(
            finishedCorrectly &&
            resetCorrectly &&
            simulation.isRunning(),
            "Reset allows restarting finished simulation"
        );
    }

    // 13. Reject zero fixed timestep
    {
        SimulationConfig config = createTestConfig();
        config.fixedTimeStep = 0.0;

        check(
            throwsInvalidArgument([&]() {
                Simulation simulation(config);
            }),
            "Rejects zero fixed time step"
        );
    }

    // 14. Reject negative time scale in config
    {
        SimulationConfig config = createTestConfig();
        config.timeScale = -1.0;

        check(
            throwsInvalidArgument([&]() {
                Simulation simulation(config);
            }),
            "Rejects negative time scale"
        );
    }

    // 15. Reject zero mass
    {
        SimulationConfig config = createTestConfig();
        config.initialBodies[0].mass = 0.0;

        check(
            throwsInvalidArgument([&]() {
                Simulation simulation(config);
            }),
            "Rejects zero mass"
        );
    }

    // 16. Reject negative radius
    {
        SimulationConfig config = createTestConfig();
        config.initialBodies[0].radius = -1.0;

        check(
            throwsInvalidArgument([&]() {
                Simulation simulation(config);
            }),
            "Rejects negative radius"
        );
    }

    // 17. Reject infinite position
    {
        SimulationConfig config = createTestConfig();

        config.initialBodies[0].position.x =
            std::numeric_limits<double>::infinity();

        check(
            throwsInvalidArgument([&]() {
                Simulation simulation(config);
            }),
            "Rejects infinite position"
        );
    }

    // 18. Reject NaN velocity
    {
        SimulationConfig config = createTestConfig();

        config.initialBodies[0].velocity.x =
            std::numeric_limits<double>::quiet_NaN();

        check(
            throwsInvalidArgument([&]() {
                Simulation simulation(config);
            }),
            "Rejects NaN velocity"
        );
    }

    // 19. Reject zero time scale
    {
        Simulation simulation(createTestConfig());

        check(
            throwsInvalidArgument([&]() {
                simulation.setTimeScale(0.0);
            }),
            "Rejects zero time scale"
        );
    }

    // 20. Reject negative deltaTime
    {
        Simulation simulation(createTestConfig());

        check(
            throwsInvalidArgument([&]() {
                simulation.update(-0.1);
            }),
            "Rejects negative deltaTime"
        );
    }

    // Test summary
    std::cout << "\n============================\n";
    std::cout << "Passed: " << passed << '\n';
    std::cout << "Failed: " << failed << '\n';
    std::cout << "============================\n";

    return failed == 0 ? 0 : 1;
}
