
#pragma once

#include "SimulationConfig.h"
#include "Body.h"
#include "Physics.h"

#include <vector>
#include <stdexcept>
#include <cstddef>

enum class SimulationState
{
    Paused,
    Running,
    Finished
};

class Simulation
{
public:
    using BodyId = std::size_t;

    explicit Simulation(const SimulationConfig& config);

    void start();
    void pause();
    void reset();

    void update(double realDeltaTime);
    void setTimeScale(double scale);

    bool isRunning() const;
    SimulationState getState() const;

    const std::vector<Body>& getBodies() const;

    std::vector<Body> snapshot() const;

    double getSimulationTime() const;
    bool hasCollision() const;
    bool consumeTrajectoriesResetFlag();

    bool setBodyPosition(BodyId id, const Vector2D& position);
    bool setBodyVelocity(BodyId id, const Vector2D& velocity);
    bool setBodyMass(BodyId id, double mass);
    bool setBodyRadius(BodyId id, double radius);

private:
    SimulationConfig m_config;
    std::vector<Body> m_bodies;

    Physics m_physics;

    SimulationState m_state = SimulationState::Paused;

    double m_timeScale = 1.0;
    double m_accumulator = 0.0;
    double m_simulationTime = 0.0;

    bool m_collisionDetected = false;
    bool m_trajectoriesResetRequested = false;

    static constexpr double kMaxFrameTime = 0.25;
    static constexpr double kMinTimeScale = 0.0;
    static constexpr double kMaxTimeScale = 10.0;

    static void validateBody(const Body& body);

    void onBodyEdited();
};
