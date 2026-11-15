#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <deque>
#include <random>

#include "physics/Body.h"
#include "graphics/CoordinateSystem.h"

class Graphics
{
private:
    std::vector<std::deque<sf::Vector2f>> trails;
    CoordinateSystem m_coords;

    // Для звездного неба
    struct Star {
        sf::Vector2f pos;
        float size;
        sf::Color color;
    };
    std::vector<Star> m_stars;

    // Для текстур планет
    sf::Texture m_planetTexture;

    void generateStars(sf::Vector2u windowSize);
    void generatePlanetTexture();

public:
    Graphics(sf::Vector2u windowSize);

    // Отрисовка только звезд (для меню)
    void drawStars(sf::RenderWindow& window);

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
