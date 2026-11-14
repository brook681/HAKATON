#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <deque>

#include "physics/Body.h"
#include "graphics/CoordinateSystem.h"

class Graphics
{
private:
    std::vector<std::deque<sf::Vector2f>> trails;
    CoordinateSystem m_coords;

public:
    Graphics();

    void draw(
        sf::RenderWindow& window,
        const std::vector<Body>& bodies
    );

    void resetTrails()
    {
        for (auto& trail : trails)
        {
            trail.clear();
        }
    }

    CoordinateSystem& getCoordinates()
    {
        return m_coords;
    }
};
