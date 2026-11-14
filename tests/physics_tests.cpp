
#include "../src/simulation/Simulation.h"
#include "../src/physics/Physics.h"
#include "../src/physics/Scenarios.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cstddef>

using Bodies = std::vector<Body>;

double calculateTotalEnergy(const Bodies& bodies)
{
    double kineticEnergy = 0.0;
    double potentialEnergy = 0.0;

    for (const Body& body : bodies)
    {
        kineticEnergy +=
            0.5 * body.mass * body.velocity.lengthSquared();
    }

    for (std::size_t i = 0; i < bodies.size(); ++i)
    {
        for (std::size_t j = i + 1; j < bodies.size(); ++j)
        {
            Vector2D difference =
                bodies[j].position - bodies[i].position;

            double distance = std::sqrt(
                difference.lengthSquared() +
                Physics::SOFTENING * Physics::SOFTENING
            );

            potentialEnergy -=
                Physics::GRAVITATIONAL_CONSTANT *
                bodies[i].mass *
                bodies[j].mass / distance;
        }
    }

    return kineticEnergy + potentialEnergy;
}

Vector2D calculateTotalMomentum(const Bodies& bodies)
{
    Vector2D momentum{0.0, 0.0};

    for (const Body& body : bodies)
    {
        momentum = momentum + body.velocity * body.mass;
    }

    return momentum;
}

void testCollisionDetection()
{
    Physics physics;

    Bodies bodies(3);

    bodies[0].position = {0.0, 0.0};
    bodies[0].radius = 1.0;

    bodies[1].position = {1.5, 0.0};
    bodies[1].radius = 1.0;

    bodies[2].position = {10.0, 0.0};
    bodies[2].radius = 1.0;

    auto collisions = physics.detectCollisions(bodies);

    if (collisions.size() != 1)
        throw std::runtime_error("Collision detection failed!");

    if (collisions[0].firstBody != 0 ||
        collisions[0].secondBody != 1)
        throw std::runtime_error("Wrong collision pair!");

    std::cout << "[PASS] Collision detection\n";
}

void testNoCollision()
{
    Physics physics;

    Bodies bodies(3);

    bodies[0].position = {0.0, 0.0};
    bodies[1].position = {5.0, 0.0};
    bodies[2].position = {10.0, 0.0};

    auto collisions = physics.detectCollisions(bodies);

    if (!collisions.empty())
        throw std::runtime_error("False collision detected!");

    std::cout << "[PASS] No collision detection\n";
}

void testTouchingBodies()
{
    Physics physics;

    Bodies bodies(3);

    bodies[0].position = {0.0, 0.0};
    bodies[0].radius = 1.0;

    bodies[1].position = {2.0, 0.0};
    bodies[1].radius = 1.0;

    bodies[2].position = {10.0, 0.0};
    bodies[2].radius = 1.0;

    auto collisions = physics.detectCollisions(bodies);

    if (collisions.size() != 1)
        throw std::runtime_error("Touching bodies test failed!");

    if (collisions[0].firstBody != 0 ||
        collisions[0].secondBody != 1)
    {
        throw std::runtime_error("Wrong touching bodies pair!");
    }

    std::cout << "[PASS] Touching bodies\n";
}

void testFigureEightInitialConditions()
{
    const auto bodies = Scenarios::createFigureEight();

    for (const Body& body : bodies)
    {
        if (std::abs(body.mass - 1.0) > 1e-12)
            throw std::runtime_error("Figure Eight mass test failed!");
    }

    Vector2D momentum{0.0, 0.0};

    for (const Body& body : bodies)
    {
        momentum = momentum + body.velocity * body.mass;
    }

    if (momentum.length() > 1e-8)
        throw std::runtime_error("Figure Eight momentum test failed!");

    std::cout << "[PASS] Figure Eight initial conditions\n";
}

void testFigureEightOrbit()
{
    Physics physics;

    auto bodies = Scenarios::createFigureEight();
    const auto initialBodies = bodies;

    constexpr double period = 6.32591398;
    constexpr int steps = 63260;

    const double deltaTime = period / steps;

    for (int i = 0; i < steps; ++i)
    {
        physics.update(bodies, deltaTime);
    }

    double maxPositionError = 0.0;
    double maxVelocityError = 0.0;

    for (std::size_t i = 0; i < bodies.size(); ++i)
    {
        double positionError =
            (bodies[i].position -
             initialBodies[i].position).length();

        double velocityError =
            (bodies[i].velocity -
             initialBodies[i].velocity).length();

        maxPositionError =
            std::max(maxPositionError, positionError);

        maxVelocityError =
            std::max(maxVelocityError, velocityError);
    }

    std::cout << "[ORBIT] Position error: "
              << maxPositionError << '\n';

    std::cout << "[ORBIT] Velocity error: "
              << maxVelocityError << '\n';

    if (maxPositionError > 1e-5 ||
        maxVelocityError > 1e-5)
    {
        throw std::runtime_error("Figure Eight orbit test failed!");
    }

    std::cout << "[PASS] Figure Eight orbit\n";
}

void testFigureEightEnergy()
{
    Physics physics;

    auto bodies = Scenarios::createFigureEight();

    constexpr double period = 6.32591398;
    constexpr int steps = 63260;

    const double deltaTime = period / steps;

    const double initialEnergy =
        calculateTotalEnergy(bodies);

    double maxRelativeError = 0.0;

    for (int i = 0; i < steps; ++i)
    {
        physics.update(bodies, deltaTime);

        const double currentEnergy =
            calculateTotalEnergy(bodies);

        const double relativeError =
            std::abs(currentEnergy - initialEnergy) /
            std::max(std::abs(initialEnergy), 1e-12);

        maxRelativeError =
            std::max(maxRelativeError, relativeError);
    }

    std::cout << "[FIGURE EIGHT] Initial energy: "
              << initialEnergy << '\n';

    std::cout << "[FIGURE EIGHT] Max relative energy error: "
              << maxRelativeError << '\n';

    if (maxRelativeError > 1e-7)
    {
        throw std::runtime_error(
            "Figure Eight energy conservation failed!"
        );
    }

    std::cout << "[PASS] Figure Eight energy conservation\n";
}

void testFigureEightEnergyThroughSimulation()
{
    constexpr double period = 6.32591398;
    constexpr int steps = 63260;

    SimulationConfig config;
    config.initialBodies = Scenarios::createFigureEight();
    config.fixedTimeStep = period / steps;
    config.timeScale = 1.0;

    Simulation simulation(config);

    const double initialEnergy =
        calculateTotalEnergy(simulation.getBodies());

    double maxRelativeError = 0.0;

    simulation.start();

    for (int i = 0; i < steps; ++i)
    {
        simulation.update(config.fixedTimeStep);

        if (simulation.getState() != SimulationState::Running)
        {
            throw std::runtime_error(
                "Simulation unexpectedly stopped during Figure Eight test!"
            );
        }

        const double currentEnergy =
            calculateTotalEnergy(simulation.getBodies());

        const double relativeError =
            std::abs(currentEnergy - initialEnergy) /
            std::max(std::abs(initialEnergy), 1e-12);

        maxRelativeError =
            std::max(maxRelativeError, relativeError);
    }

    const double expectedTime = period;
    const double actualTime = simulation.getSimulationTime();

    std::cout << "[SIMULATION FIGURE EIGHT] Initial energy: "
              << initialEnergy << '\n';

    std::cout << "[SIMULATION FIGURE EIGHT] Max relative energy error: "
              << maxRelativeError << '\n';

    std::cout << "[SIMULATION FIGURE EIGHT] Simulation time: "
              << actualTime << '\n';

    if (maxRelativeError > 1e-7)
    {
        throw std::runtime_error(
            "Figure Eight Simulation energy conservation failed!"
        );
    }

    if (std::abs(actualTime - expectedTime) > 1e-8)
    {
        throw std::runtime_error(
            "Figure Eight Simulation time tracking failed!"
        );
    }

    std::cout << "[PASS] Figure Eight energy through Simulation\n";
}

void testGravity()
{
    Physics physics;

    Bodies bodies(3);

    bodies[0].mass = 1.0;
    bodies[0].position = {-1.0, 0.0};

    bodies[1].mass = 1.0;
    bodies[1].position = {1.0, 0.0};

    bodies[2].mass = 1.0;
    bodies[2].position = {0.0, 10.0};

    physics.calculateAccelerations(bodies);

    if (bodies[0].acceleration.x <= 0.0)
        throw std::runtime_error("Gravity test failed!");

    if (bodies[1].acceleration.x >= 0.0)
        throw std::runtime_error("Gravity test failed!");

    std::cout << "[PASS] Gravity direction\n";
}

void testMomentumConservation()
{
    Physics physics;

    Bodies bodies(3);

    bodies[0].mass = 2.0;
    bodies[0].position = {-1.0, 0.0};
    bodies[0].velocity = {0.0, -0.5};

    bodies[1].mass = 3.0;
    bodies[1].position = {1.0, 0.0};
    bodies[1].velocity = {0.0, 0.2};

    bodies[2].mass = 1.0;
    bodies[2].position = {0.0, 2.0};
    bodies[2].velocity = {0.0, 0.4};

    Vector2D initialMomentum =
        calculateTotalMomentum(bodies);

    for (int i = 0; i < 1000; ++i)
        physics.update(bodies, 0.001);

    Vector2D finalMomentum =
        calculateTotalMomentum(bodies);

    double errorX =
        std::abs(finalMomentum.x - initialMomentum.x);

    double errorY =
        std::abs(finalMomentum.y - initialMomentum.y);

    if (errorX > 1e-9 || errorY > 1e-9)
        throw std::runtime_error("Momentum conservation failed!");

    std::cout << "[PASS] Momentum conservation\n";
}

void testEnergyConservation()
{
    Physics physics;

    Bodies bodies(3);

    bodies[0].mass = 1.0;
    bodies[0].position = {-1.0, 0.0};
    bodies[0].velocity = {0.0, -0.2};

    bodies[1].mass = 1.0;
    bodies[1].position = {1.0, 0.0};
    bodies[1].velocity = {0.0, -0.2};

    bodies[2].mass = 1.0;
    bodies[2].position = {0.0, 1.0};
    bodies[2].velocity = {0.0, 0.4};

    double initialEnergy =
        calculateTotalEnergy(bodies);

    for (int i = 0; i < 10000; ++i)
        physics.update(bodies, 0.0001);

    double finalEnergy =
        calculateTotalEnergy(bodies);

    double relativeError =
        std::abs(finalEnergy - initialEnergy) /
        std::max(std::abs(initialEnergy), 1e-12);

    if (relativeError > 1e-5)
        throw std::runtime_error("Energy conservation failed!");

    std::cout << "[PASS] Energy conservation\n";
    std::cout << "Initial energy: " << initialEnergy << '\n';
    std::cout << "Final energy: " << finalEnergy << '\n';
    std::cout << "Relative error: " << relativeError << '\n';
}

void testEditBodyPosition()
{
    SimulationConfig config;
    config.initialBodies = Scenarios::createFigureEight();

    Simulation simulation(config);

    const Vector2D newPos{0.5, -0.25};

    if (!simulation.setBodyPosition(0, newPos))
        throw std::runtime_error(
            "setBodyPosition returned false for valid id"
        );

    const Vector2D actual =
        simulation.getBodies()[0].position;

    if ((actual - newPos).length() > 1e-12)
        throw std::runtime_error("Position was not updated");

    std::cout << "[PASS] Edit body position\n";
}

void testEditInvalidId()
{
    SimulationConfig config;
    config.initialBodies = Scenarios::createFigureEight();

    Simulation simulation(config);

    if (simulation.setBodyPosition(999, {0.0, 0.0}))
        throw std::runtime_error("setBodyPosition accepted invalid id");

    if (simulation.setBodyVelocity(999, {0.0, 0.0}))
        throw std::runtime_error("setBodyVelocity accepted invalid id");

    if (simulation.setBodyMass(999, 1.0))
        throw std::runtime_error("setBodyMass accepted invalid id");

    if (simulation.setBodyRadius(999, 0.1))
        throw std::runtime_error("setBodyRadius accepted invalid id");

    std::cout << "[PASS] Invalid BodyId rejected\n";
}

void testEditInvalidData()
{
    SimulationConfig config;
    config.initialBodies = Scenarios::createFigureEight();

    Simulation simulation(config);

    const double oldMass =
        simulation.getBodies()[0].mass;

    bool caught = false;

    try
    {
        simulation.setBodyMass(0, 0.0);
    }
    catch (const std::invalid_argument&)
    {
        caught = true;
    }

    if (!caught)
        throw std::runtime_error("Zero mass was not rejected");

    if (std::abs(simulation.getBodies()[0].mass - oldMass) > 1e-12)
        throw std::runtime_error("Body mutated after rejected edit");

    caught = false;

    try
    {
        simulation.setBodyRadius(1, -0.5);
    }
    catch (const std::invalid_argument&)
    {
        caught = true;
    }

    if (!caught)
        throw std::runtime_error("Negative radius was not rejected");

    caught = false;

    try
    {
        simulation.setBodyPosition(2, {std::nan(""), 0.0});
    }
    catch (const std::invalid_argument&)
    {
        caught = true;
    }

    if (!caught)
        throw std::runtime_error("NaN position was not rejected");

    std::cout << "[PASS] Invalid body data rejected\n";
}

void testEditAffectsDynamics()
{
    SimulationConfig config;
    config.initialBodies = Scenarios::createFigureEight();
    config.fixedTimeStep = 1e-4;

    Simulation simulation(config);

    simulation.setBodyPosition(0, {0.0, 0.0});
    simulation.setBodyPosition(1, {100.0, 0.0});
    simulation.setBodyPosition(2, {200.0, 0.0});

    simulation.start();

    simulation.setBodyVelocity(0, {50.0, 0.0});

    for (int i = 0; i < 10; ++i)
        simulation.update(config.fixedTimeStep);

    double currentX =
        simulation.getBodies()[0].position.x;

    if (currentX <= 0.01)
        throw std::runtime_error(
            "Velocity edit did not affect dynamics"
        );

    std::cout << "[PASS] Edit affects dynamics\n";
}

void testCollisionMerging()
{
    SimulationConfig config;
    config.initialBodies.resize(3);
    config.fixedTimeStep = 1e-3;

    config.initialBodies[0].mass = 2.0;
    config.initialBodies[0].position = {0.0, 0.0};
    config.initialBodies[0].velocity = {1.0, 0.0};
    config.initialBodies[0].radius = 1.0;

    config.initialBodies[1].mass = 3.0;
    config.initialBodies[1].position = {1.5, 0.0};
    config.initialBodies[1].velocity = {-1.0, 0.0};
    config.initialBodies[1].radius = 1.0;

    config.initialBodies[2].mass = 1.0;
    config.initialBodies[2].position = {10.0, 0.0};
    config.initialBodies[2].velocity = {0.0, 0.0};
    config.initialBodies[2].radius = 1.0;

    Simulation simulation(config);

    const Vector2D initialMomentum =
        calculateTotalMomentum(simulation.getBodies());

    simulation.start();
    simulation.update(config.fixedTimeStep);

    const auto& bodies = simulation.getBodies();

    if (bodies.size() != 2)
        throw std::runtime_error(
            "Bodies were not merged after collision!"
        );

    if (std::abs(bodies[0].mass - 5.0) > 1e-10)
        throw std::runtime_error(
            "Merged body has incorrect mass!"
        );

    const Vector2D finalMomentum =
        calculateTotalMomentum(bodies);

    if ((finalMomentum - initialMomentum).length() > 1e-8)
        throw std::runtime_error(
            "Momentum was not conserved during merging!"
        );

    if (simulation.getState() != SimulationState::Running)
        throw std::runtime_error(
            "Simulation stopped after collision!"
        );

    if (!simulation.hasCollision())
        throw std::runtime_error(
            "Collision flag was not set!"
        );

    std::cout << "[PASS] Collision merging\n";
}

int main()
{
    try
    {
        testCollisionDetection();
        testNoCollision();
        testTouchingBodies();

        testFigureEightInitialConditions();
        testFigureEightOrbit();
        testFigureEightEnergy();
        testFigureEightEnergyThroughSimulation();

        testGravity();
        testMomentumConservation();
        testEnergyConservation();

        testEditBodyPosition();
        testEditInvalidId();
        testEditInvalidData();
        testEditAffectsDynamics();

        testCollisionMerging();

        std::cout << "\nAll physics tests passed!\n";
    }
    catch (const std::exception& error)
    {
        std::cerr << "\nTEST FAILED: "
                  << error.what() << '\n';

        return 1;
    }

    return 0;
}
