#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <deque>

#include "physics/Body.h"
#include "graphics/CoordinateSystem.h" // <--- Подключаем нашу систему

class Graphics
{
private:
    std::array<std::deque<sf::Vector2f>, 3> trails;
    CoordinateSystem m_coords;

public:
    Graphics();

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

    // Дадим графисту доступ к камере, чтобы он мог делать зум или двигать её
    CoordinateSystem& getCoordinates() { return m_coords; }
};
