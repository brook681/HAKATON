
#include "Simulation.h"

#include <stdexcept>
#include <cmath>

// Check whether a vector contains only finite values
static bool isValidVector(const Vector2D& vector)
{
    return std::isfinite(vector.x) &&
           std::isfinite(vector.y);
}

// Validate initial body parameters
static void validateBody(const Body& body)
{
    if (!std::isfinite(body.mass) || body.mass <= 0.0)
        throw std::invalid_argument("Invalid body mass");

    if (!std::isfinite(body.radius) || body.radius < 0.0)
        throw std::invalid_argument("Invalid body radius");

    if (!isValidVector(body.position))
        throw std::invalid_argument("Invalid body position");

    if (!isValidVector(body.velocity))
        throw std::invalid_argument("Invalid body velocity");
}

// Validate the complete simulation configuration
static void validateConfig(const SimulationConfig& config)
{
    if (!std::isfinite(config.fixedTimeStep) ||
        config.fixedTimeStep <= 0.0)
    {
        throw std::invalid_argument("Invalid fixed time step");
    }

    if (!std::isfinite(config.timeScale) ||
        config.timeScale <= 0.0)
    {
        throw std::invalid_argument("Invalid time scale");
    }

    for (const Body& body : config.initialBodies)
    {
        validateBody(body);
    }
}

// Constructor
Simulation::Simulation(const SimulationConfig& config)
    : config(config),
      bodies(config.initialBodies),
      timeScale(config.timeScale)
{
    validateConfig(config);

    for (Body& body : bodies)
    {
        body.acceleration = {0.0, 0.0};
    }
}

// Start simulation
void Simulation::start()
{
    if (state == SimulationState::Finished)
        return;

    state = SimulationState::Running;
}

// Pause simulation
void Simulation::pause()
{
    if (state == SimulationState::Running)
    {
        state = SimulationState::Paused;
    }
}

// Reset simulation
void Simulation::reset()
{
    bodies = config.initialBodies;

    for (Body& body : bodies)
    {
        body.acceleration = {0.0, 0.0};
    }

    simulationTime = 0.0;
    accumulator = 0.0;
    timeScale = config.timeScale;

    state = SimulationState::Paused;
}

// Change simulation speed
void Simulation::setTimeScale(double scale)
{
    if (!std::isfinite(scale) || scale <= 0.0)
    {
        throw std::invalid_argument("Invalid time scale");
    }

    timeScale = scale;
}

// Check simulation state
bool Simulation::isRunning() const
{
    return state == SimulationState::Running;
}

// Get simulation state
SimulationState Simulation::getState() const
{
    return state;
}

// Get bodies
const std::array<Body, 3>& Simulation::getBodies() const
{
    return bodies;
}

// Get simulation time
double Simulation::getSimulationTime() const
{
    return simulationTime;
}

// Update simulation
void Simulation::update(double deltaTime)
{
    if (!std::isfinite(deltaTime) || deltaTime < 0.0)
    {
        throw std::invalid_argument("Invalid deltaTime");
    }

    if (state != SimulationState::Running)
        return;

    const double scaledDeltaTime = deltaTime * timeScale;

    if (!std::isfinite(scaledDeltaTime) ||
        !std::isfinite(accumulator + scaledDeltaTime))
    {
        throw std::overflow_error("Simulation time overflow");
    }

    const double fixedStep = config.fixedTimeStep;

    accumulator += scaledDeltaTime;

    std::size_t substeps = 0;

    while (accumulator >= fixedStep &&
           substeps < MAX_SUBSTEPS_PER_FRAME)
    {
        physics.update(bodies, fixedStep);

        if (!std::isfinite(simulationTime + fixedStep))
        {
            throw std::overflow_error("Simulation time overflow");
        }

        simulationTime += fixedStep;
        accumulator -= fixedStep;

        ++substeps;

        // Check collisions
        const auto collisions = physics.detectCollisions(bodies);

        if (!collisions.empty())
        {
            state = SimulationState::Finished;
            accumulator = 0.0;
            return;
        }
    }
}
