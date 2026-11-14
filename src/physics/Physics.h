
#pragma once

#include "Body.h"

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
        std::vector<Body>& bodies
    ) const;

    void update(
        std::vector<Body>& bodies,
        double deltaTime
    ) const;

    std::vector<Collision> detectCollisions(
        const std::vector<Body>& bodies
    ) const;

    void mergeCollisions(
        std::vector<Body>& bodies
    ) const;

private:
    Vector2D calculateAccelerationForBody(
        const Body& body,
        const std::vector<Body>& bodies,
        int bodyIndex
    ) const;
};
