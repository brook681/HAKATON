
#include "../src/simulation/Simulation.h"

#include <iostream>
#include <cmath>
#include <stdexcept>
#include <limits>
#include <vector>

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

    config.initialBodies.resize(3);

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

    config.initialBodies[0].position = {-0.08, 0.0};
    config.initialBodies[1].position = {0.08, 0.0};

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

    // 10. Collision merges bodies
    {
        Simulation simulation(createCollisionConfig());

        simulation.start();
        simulation.update(0.01);

        check(
            simulation.hasCollision() &&
            simulation.getBodies().size() == 2,
            "Collision merges two bodies"
        );
    }

    // 11. Simulation continues after collision
    {
        Simulation simulation(createCollisionConfig());

        simulation.start();
        simulation.update(0.01);

        const bool runningAfterCollision = simulation.isRunning();

        simulation.update(0.01);

        check(
            runningAfterCollision &&
            simulation.isRunning(),
            "Simulation continues after collision"
        );
    }

    // 12. Merged body conserves mass
    {
        Simulation simulation(createCollisionConfig());

        simulation.start();
        simulation.update(0.01);

        const auto& bodies = simulation.getBodies();

        check(
            bodies.size() == 2 &&
            std::abs(bodies[0].mass - 2.0) < 1e-9,
            "Merged body conserves mass"
        );
    }

    // 13. Merged body conserves momentum
    {
        Simulation simulation(createCollisionConfig());

        simulation.start();
        simulation.update(0.01);

        const auto& bodies = simulation.getBodies();

        double totalMomentumX = 0.0;
        double totalMomentumY = 0.0;

        for (const Body& body : bodies)
        {
            totalMomentumX += body.mass * body.velocity.x;
            totalMomentumY += body.mass * body.velocity.y;
        }

        check(
            std::abs(totalMomentumX) < 1e-7 &&
            std::abs(totalMomentumY) < 1e-7,
            "Merged bodies conserve total momentum"
        );
    }

    // 14. Reset after collision
    {
        SimulationConfig config = createCollisionConfig();
        Simulation simulation(config);

        simulation.start();
        simulation.update(0.01);

        simulation.reset();

        check(
            simulation.getState() == SimulationState::Paused &&
            simulation.getSimulationTime() == 0.0 &&
            simulation.getBodies().size() == 3 &&
            !simulation.hasCollision(),
            "Reset restores bodies after collision"
        );
    }

    // 15. Reject zero fixed timestep
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

    // 16. Reject negative time scale in config
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

    // 17. Reject zero mass
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

    // 18. Reject negative radius
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

    // 19. Reject infinite position
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

    // 20. Reject NaN velocity
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

    // 21. Reject zero time scale
    {
        Simulation simulation(createTestConfig());

        check(
            throwsInvalidArgument([&]() {
                simulation.setTimeScale(0.0);
            }),
            "Rejects zero time scale"
        );
    }

    // 22. Reject negative deltaTime
    {
        Simulation simulation(createTestConfig());

        check(
            throwsInvalidArgument([&]() {
                simulation.update(-0.1);
            }),
            "Rejects negative deltaTime"
        );
    }

    // 23. Long-running simulation stability
    {
        SimulationConfig config = createTestConfig();
        config.fixedTimeStep = 0.001;

        Simulation simulation(config);
        simulation.start();

        constexpr int iterations = 10000;
        constexpr double deltaTime = 0.001;

        for (int i = 0; i < iterations; ++i)
        {
            simulation.update(deltaTime);
        }

        const double expectedTime = iterations * deltaTime;

        check(
            std::abs(simulation.getSimulationTime() - expectedTime) < 1e-8,
            "Long-running simulation time stability"
        );
    }

    // Test summary
    std::cout << "\n============================\n";
    std::cout << "Passed: " << passed << '\n';
    std::cout << "Failed: " << failed << '\n';
    std::cout << "============================\n";

    return failed == 0 ? 0 : 1;
}
