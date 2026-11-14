
#include "Simulation.h"

#include <stdexcept>
#include <cmath>

// Конструктор симуляции
Simulation::Simulation(const SimulationConfig& config)
    : config(config),
      bodies(config.initialBodies),
      timeScale(config.timeScale)
{
    if (!std::isfinite(config.fixedTimeStep) ||
        config.fixedTimeStep <= 0.0)
    {
        throw std::invalid_argument("Invalid fixed time step");
    }

    if (!std::isfinite(timeScale) || timeScale <= 0.0)
    {
        throw std::invalid_argument("Invalid time scale");
    }

    // Сбрасываем ускорения, чтобы физика рассчитала их сама
    for (Body& body : bodies)
    {
        body.acceleration = {0.0, 0.0};
    }
}

// Запуск симуляции
void Simulation::start()
{
    if (state == SimulationState::Finished)
        return;

    state = SimulationState::Running;
}

// Пауза
void Simulation::pause()
{
    if (state == SimulationState::Running)
    {
        state = SimulationState::Paused;
    }
}

// Полный сброс симуляции
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

// Изменение скорости симуляции
void Simulation::setTimeScale(double scale)
{
    if (!std::isfinite(scale) || scale <= 0.0)
    {
        throw std::invalid_argument("Invalid time scale");
    }

    timeScale = scale;
}

// Проверка состояния
bool Simulation::isRunning() const
{
    return state == SimulationState::Running;
}

// Получение состояния
SimulationState Simulation::getState() const
{
    return state;
}

// Получение тел
const std::array<Body, 3>& Simulation::getBodies() const
{
    return bodies;
}

// Получение времени симуляции
double Simulation::getSimulationTime() const
{
    return simulationTime;
}

// Обновление симуляции
void Simulation::update(double deltaTime)
{
    if (!std::isfinite(deltaTime) || deltaTime < 0.0)
    {
        throw std::invalid_argument("Invalid deltaTime");
    }

    if (state != SimulationState::Running)
        return;

    const double fixedStep = config.fixedTimeStep;

    accumulator += deltaTime * timeScale;

    while (accumulator >= fixedStep)
    {
        physics.update(bodies, fixedStep);

        simulationTime += fixedStep;
        accumulator -= fixedStep;

        // Проверяем столкновения
        const auto collisions = physics.detectCollisions(bodies);

        if (!collisions.empty())
        {
            state = SimulationState::Finished;
            accumulator = 0.0;
            return;
        }
    }
}
