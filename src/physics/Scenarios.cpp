#include "Scenarios.h"

std::vector<Body> Scenarios::createFigureEight()
{
    std::vector<Body> bodies(3);

    for (Body& body : bodies)
    {
        body.mass = 1.0;
        body.radius = 0.1;
    }

    // Нежные цвета
    bodies[0].r = 140; bodies[0].g = 200; bodies[0].b = 255; // Небесно-голубой
    bodies[1].r = 255; bodies[1].g = 160; bodies[1].b = 122; // Терракотовый
    bodies[2].r = 255; bodies[2].g = 240; bodies[2].b = 180; // Бледно-золотой

    bodies[0].position = {-0.97000436,  0.24308753};
    bodies[1].position = { 0.97000436, -0.24308753};
    bodies[2].position = { 0.0, 0.0};

    bodies[0].velocity = { 0.466203685,  0.43236573};
    bodies[1].velocity = { 0.466203685,  0.43236573};
    bodies[2].velocity = {-0.932407370, -0.86473146};

    return bodies;
}

std::vector<Body> Scenarios::createChaotic()
{
    std::vector<Body> bodies(3);

    for (Body& body : bodies)
    {
        body.mass = 1.0;
        body.radius = 0.05;
    }

    bodies[0].r = 140; bodies[0].g = 200; bodies[0].b = 255;
    bodies[1].r = 255; bodies[1].g = 160; bodies[1].b = 122;
    bodies[2].r = 255; bodies[2].g = 240; bodies[2].b = 180;

    bodies[0].position = {-1.0, 0.0};
    bodies[1].position = {1.0, 0.0};
    bodies[2].position = {0.0, 0.2};

    bodies[0].velocity = {0.0, -0.5};
    bodies[1].velocity = {0.0, 0.5};
    bodies[2].velocity = {0.3, 0.0};

    return bodies;
}

std::vector<Body> Scenarios::createHierarchical()
{
    std::vector<Body> bodies(3);

    bodies[0].mass = 1.0;
    bodies[1].mass = 1.0;
    bodies[2].mass = 0.5;

    for (Body& body : bodies)
        body.radius = 0.05;

    bodies[0].r = 140; bodies[0].g = 200; bodies[0].b = 255;
    bodies[1].r = 255; bodies[1].g = 160; bodies[1].b = 122;
    bodies[2].r = 255; bodies[2].g = 240; bodies[2].b = 180;

    bodies[0].position = {-0.5, 0.0};
    bodies[1].position = {0.5, 0.0};

    bodies[0].velocity = {0.0, -0.7071};
    bodies[1].velocity = {0.0, 0.7071};

    bodies[2].position = {0.0, 5.0};
    bodies[2].velocity = {-0.2582, 0.0};

    return bodies;
}