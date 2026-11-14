
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
static void validateBodies(const std::array<Body, 3>& bodies)
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
    const std::array<Body, 3>& bodies,
    int bodyIndex
) const
{
    Vector2D acceleration{0.0, 0.0};

    for (int j = 0; j < 3; ++j)
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
    std::array<Body, 3>& bodies
) const
{
    validateBodies(bodies);

    for (int i = 0; i < 3; ++i)
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
    std::array<Body, 3>& bodies,
    double deltaTime
) const
{
    if (!std::isfinite(deltaTime) || deltaTime <= 0.0)
        throw std::invalid_argument("Invalid deltaTime");

    validateBodies(bodies);

    // 1. Calculate initial accelerations
    calculateAccelerations(bodies);

    // 2. Save initial accelerations
    std::array<Vector2D, 3> oldAccelerations;

    for (int i = 0; i < 3; ++i)
    {
        oldAccelerations[i] = bodies[i].acceleration;
    }

    // 3. Update positions
    for (Body& body : bodies)
    {
        body.position =
            body.position +
            body.velocity * deltaTime +
            body.acceleration * (0.5 * deltaTime * deltaTime);

        if (!isValidVector(body.position))
            throw std::runtime_error("Invalid calculated position");
    }

    // 4. Calculate new accelerations
    calculateAccelerations(bodies);

    // 5. Update velocities
    for (int i = 0; i < 3; ++i)
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
    const std::array<Body, 3>& bodies
) const
{
    validateBodies(bodies);

    std::vector<Collision> collisions;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = i + 1; j < 3; ++j)
        {
            Vector2D difference =
                bodies[j].position - bodies[i].position;

            double distance = difference.length();

            double minimumDistance =
                bodies[i].radius + bodies[j].radius;

            const double tolerance = 1e-12;

            if (distance <= minimumDistance + tolerance)
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
