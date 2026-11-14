#include "Scenarios.h"

std::array<Body, 3> Scenarios::createFigureEight()
{
    std::array<Body, 3> bodies{};

    for (Body& body : bodies)
    {
        body.mass = 1.0;
        body.radius = 0.1;
    }

    bodies[0].position = {-0.97000436,  0.24308753};
    bodies[1].position = { 0.97000436, -0.24308753};
    bodies[2].position = { 0.0, 0.0};

    bodies[0].velocity = { 0.466203685,  0.43236573};
    bodies[1].velocity = { 0.466203685,  0.43236573};
    bodies[2].velocity = {-0.932407370, -0.86473146};

    return bodies;
}