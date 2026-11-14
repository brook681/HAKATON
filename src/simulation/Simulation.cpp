#include "Simulation.h"
#include <algorithm>
#include <cmath>

static bool isValidNumber(double val) {
    return std::isfinite(val);
}

void Simulation::validateBody(const Body& body) {
    if (!isValidNumber(body.mass) || body.mass <= 0.0)
        throw std::invalid_argument("Mass must be finite and > 0");
    if (!isValidNumber(body.radius) || body.radius <= 0.0)
        throw std::invalid_argument("Radius must be finite and > 0");
    if (!isValidNumber(body.position.x) || !isValidNumber(body.position.y))
        throw std::invalid_argument("Position must be finite");
    if (!isValidNumber(body.velocity.x) || !isValidNumber(body.velocity.y))
        throw std::invalid_argument("Velocity must be finite");
}

Simulation::Simulation(const SimulationConfig& config)
    : m_config(config), 
      m_bodies(config.initialBodies),
      m_timeScale(config.timeScale)
{
    if (!isValidNumber(m_config.fixedTimeStep) || m_config.fixedTimeStep <= 0.0)
        throw std::invalid_argument("Fixed time step must be finite and > 0");
        
    if (!isValidNumber(m_timeScale) || m_timeScale <= 0.0)
        throw std::invalid_argument("Time scale must be finite and > 0");

    for (const auto& body : m_bodies) validateBody(body);
}

void Simulation::start() {
    if (m_state == SimulationState::Finished) return;
    m_state = SimulationState::Running;
}

void Simulation::pause() {
    if (m_state == SimulationState::Running) m_state = SimulationState::Paused;
}

void Simulation::reset() {
    m_bodies = m_config.initialBodies;
    m_state = SimulationState::Paused;
    m_accumulator = 0.0;
    m_simulationTime = 0.0;
    m_collisionDetected = false;
    m_trajectoriesResetRequested = true;
}

void Simulation::update(double realDeltaTime) {
    if (!isValidNumber(realDeltaTime) || realDeltaTime < 0.0)
        throw std::invalid_argument("Delta time must be finite and >= 0");

    if (m_state != SimulationState::Running) return;

    realDeltaTime = std::min(realDeltaTime, kMaxFrameTime);

    m_accumulator += realDeltaTime * m_timeScale;

    int stepsThisFrame = 0;
    constexpr int MAX_STEPS = 8;

    while (m_accumulator >= m_config.fixedTimeStep && stepsThisFrame < MAX_STEPS) {
        m_physics.update(m_bodies, m_config.fixedTimeStep);
        m_accumulator -= m_config.fixedTimeStep;
        m_simulationTime += m_config.fixedTimeStep;
        stepsThisFrame++;

        if (!m_physics.detectCollisions(m_bodies).empty()) {
            m_collisionDetected = true;
            m_state = SimulationState::Finished;
            break;
        }
    }
}

void Simulation::setTimeScale(double scale) {
    if (!isValidNumber(scale) || scale <= 0.0)
        throw std::invalid_argument("Time scale must be finite and > 0");
        
    m_timeScale = std::clamp(scale, kMinTimeScale, kMaxTimeScale);
}

bool Simulation::isRunning() const { return m_state == SimulationState::Running; }
SimulationState Simulation::getState() const { return m_state; }
const std::array<Body, 3>& Simulation::getBodies() const { return m_bodies; }
double Simulation::getSimulationTime() const { return m_simulationTime; }
bool Simulation::hasCollision() const { return m_collisionDetected; }

bool Simulation::consumeTrajectoriesResetFlag() {
    const bool flag = m_trajectoriesResetRequested;
    m_trajectoriesResetRequested = false;
    return flag;
}

void Simulation::onBodyEdited() {
    if (m_state == SimulationState::Finished) {
        m_state = SimulationState::Paused;
    }
    m_accumulator = 0.0;
}

bool Simulation::setBodyPosition(BodyId id, const Vector2D& position) {
    if (id >= m_bodies.size()) return false;

    if (!isValidNumber(position.x) || !isValidNumber(position.y))
        throw std::invalid_argument("Position must be finite");

    m_bodies[id].position = position;
    onBodyEdited();
    return true;
}

bool Simulation::setBodyVelocity(BodyId id, const Vector2D& velocity) {
    if (id >= m_bodies.size()) return false;

    if (!isValidNumber(velocity.x) || !isValidNumber(velocity.y))
        throw std::invalid_argument("Velocity must be finite");

    m_bodies[id].velocity = velocity;
    onBodyEdited();
    return true;
}

bool Simulation::setBodyMass(BodyId id, double mass) {
    if (id >= m_bodies.size()) return false;

    if (!isValidNumber(mass) || mass <= 0.0)
        throw std::invalid_argument("Mass must be finite and > 0");

    m_bodies[id].mass = mass;
    onBodyEdited();
    return true;
}

bool Simulation::setBodyRadius(BodyId id, double radius) {
    if (id >= m_bodies.size()) return false;

    if (!isValidNumber(radius) || radius <= 0.0)
        throw std::invalid_argument("Radius must be finite and > 0");

    m_bodies[id].radius = radius;
    onBodyEdited();
    return true;
}

std::array<Body, 3> Simulation::snapshot() const {
    return m_bodies;
}