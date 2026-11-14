#pragma once

#include "Body.h"

#include <array>
#include <vector>

struct Collision
{
    int firstBody;
    int secondBody;
    double distance;
};

class Physics
{
public:
    static constexpr double GRAVITATIONAL_CONSTANT = 1.0;

    static constexpr double SOFTENING = 1e-9;

    void calculateAccelerations(
        std::array<Body, 3>& bodies
    ) const;

    void update(
        std::array<Body, 3>& bodies,
        double deltaTime
    ) const;

    std::vector<Collision> detectCollisions(
        const std::array<Body, 3>& bodies
    ) const;

private:
    Vector2D calculateAccelerationForBody(
        const Body& body,
        const std::array<Body, 3>& bodies,
        int bodyIndex
    ) const;
};