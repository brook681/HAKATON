
#include "Physics.h"

#include <cmath>
#include <stdexcept>

// Check whether a vector contains only finite values
static bool isValidVector(const Vector2D& vector)
{
    return std::isfinite(vector.x) &&
           std::isfinite(vector.y);
}

// Validate all bodies
static void validateBodies(const std::vector<Body>& bodies)
{
    for (const Body& body : bodies)
    {
        if (!std::isfinite(body.mass) || body.mass <= 0.0)
            throw std::invalid_argument("Invalid mass");

        if (!std::isfinite(body.radius) || body.radius < 0.0)
            throw std::invalid_argument("Invalid radius");

        if (!isValidVector(body.position))
            throw std::invalid_argument("Invalid position");

        if (!isValidVector(body.velocity))
            throw std::invalid_argument("Invalid velocity");

        if (!isValidVector(body.acceleration))
            throw std::invalid_argument("Invalid acceleration");
    }
}

// Calculate acceleration of one body
Vector2D Physics::calculateAccelerationForBody(
    const Body& body,
    const std::vector<Body>& bodies,
    int bodyIndex
) const
{
    Vector2D acceleration{0.0, 0.0};

    for (int j = 0; j < static_cast<int>(bodies.size()); ++j)
    {
        if (j == bodyIndex)
            continue;

        Vector2D direction =
            bodies[j].position - body.position;

        double distanceSquared = direction.lengthSquared();

        double softenedDistanceSquared =
            distanceSquared + SOFTENING * SOFTENING;

        double distanceCubed =
            softenedDistanceSquared *
            std::sqrt(softenedDistanceSquared);

        double factor =
            GRAVITATIONAL_CONSTANT *
            bodies[j].mass /
            distanceCubed;

        acceleration =
            acceleration + direction * factor;
    }

    if (!isValidVector(acceleration))
        throw std::runtime_error("Invalid calculated acceleration");

    return acceleration;
}

// Calculate accelerations of all bodies
void Physics::calculateAccelerations(
    std::vector<Body>& bodies
) const
{
    validateBodies(bodies);

    for (int i = 0; i < static_cast<int>(bodies.size()); ++i)
    {
        bodies[i].acceleration =
            calculateAccelerationForBody(
                bodies[i],
                bodies,
                i
            );
    }
}

// Update system using Velocity Verlet
void Physics::update(
    std::vector<Body>& bodies,
    double deltaTime
) const
{
    if (!std::isfinite(deltaTime) || deltaTime <= 0.0)
        throw std::invalid_argument("Invalid deltaTime");

    validateBodies(bodies);

    calculateAccelerations(bodies);

    std::vector<Vector2D> oldAccelerations;

    for (const Body& body : bodies)
        oldAccelerations.push_back(body.acceleration);

    for (Body& body : bodies)
    {
        body.position =
            body.position +
            body.velocity * deltaTime +
            body.acceleration * (0.5 * deltaTime * deltaTime);

        if (!isValidVector(body.position))
            throw std::runtime_error("Invalid calculated position");
    }

    calculateAccelerations(bodies);

    for (std::size_t i = 0; i < bodies.size(); ++i)
    {
        bodies[i].velocity =
            bodies[i].velocity +
            (oldAccelerations[i] + bodies[i].acceleration)
            * (0.5 * deltaTime);

        if (!isValidVector(bodies[i].velocity))
            throw std::runtime_error("Invalid calculated velocity");
    }
}

// Detect collisions between bodies
std::vector<Collision> Physics::detectCollisions(
    const std::vector<Body>& bodies
) const
{
    validateBodies(bodies);

    std::vector<Collision> collisions;

    for (int i = 0; i < static_cast<int>(bodies.size()); ++i)
    {
        for (int j = i + 1; j < static_cast<int>(bodies.size()); ++j)
        {
            Vector2D difference =
                bodies[j].position - bodies[i].position;

            double distance = difference.length();

            double minimumDistance =
                bodies[i].radius + bodies[j].radius;

            if (distance <= minimumDistance + 1e-12)
            {
                collisions.push_back({
                    i,
                    j,
                    distance
                });
            }
        }
    }

    return collisions;
}

// Merge colliding bodies
void Physics::mergeCollisions(
    std::vector<Body>& bodies
) const
{
    while (true)
    {
        const std::vector<Collision> collisions =
            detectCollisions(bodies);

        if (collisions.empty())
            break;

        const int first = collisions[0].firstBody;
        const int second = collisions[0].secondBody;

        Body merged{};

        const Body& a = bodies[first];
        const Body& b = bodies[second];

        const double totalMass = a.mass + b.mass;

        // Conservation of mass
        merged.mass = totalMass;

        // Center of mass
        merged.position =
            (a.position * a.mass + b.position * b.mass)
            * (1.0 / totalMass);

        // Conservation of momentum
        merged.velocity =
            (a.velocity * a.mass + b.velocity * b.mass)
            * (1.0 / totalMass);

        // Preserve total 2D area
        merged.radius =
            std::sqrt(a.radius * a.radius + b.radius * b.radius);

        merged.acceleration = {0.0, 0.0};

        // Replace first body and remove second
        bodies[first] = merged;
        bodies.erase(bodies.begin() + second);
    }
}
