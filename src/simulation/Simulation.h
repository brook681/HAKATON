#pragma once

#include "SimulationConfig.h"
#include "Body.h"
#include "Physics.h"
#include <array>
#include <stdexcept>

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

    void update(double realDeltaTime);
    void setTimeScale(double scale);

    bool isRunning() const;
    SimulationState getState() const;

    const std::array<Body, 3>& getBodies() const;
    double getSimulationTime() const;
    bool hasCollision() const;
    bool consumeTrajectoriesResetFlag();

private:
    SimulationConfig m_config;       // <-- Храним весь конфиг (чтобы тесты брали fixedTimeStep)
    std::array<Body, 3> m_bodies;    // <-- Текущее состояние

    Physics m_physics;               // <-- Экземпляр физики (не конфликтует с именем класса)
    
    SimulationState m_state = SimulationState::Paused;

    double m_timeScale = 1.0;
    double m_accumulator = 0.0;
    double m_simulationTime = 0.0;

    bool m_collisionDetected = false;
    bool m_trajectoriesResetRequested = false;

    static constexpr double kMaxFrameTime = 0.25;
    static constexpr double kMinTimeScale = 0.0;
    static constexpr double kMaxTimeScale = 10.0;
};
