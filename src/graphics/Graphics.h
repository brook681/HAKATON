#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <deque>

#include "../physics/Body.h"

class Graphics
{
private:
    std::array<std::deque<sf::Vector2f>, 3> trails;

public:
    void draw(
        sf::RenderWindow& window,
        const std::array<Body, 3>& bodies
    );

    void resetTrails()
    {
        for (auto& trail : trails)
        {
            trail.clear();
        }
    }
};