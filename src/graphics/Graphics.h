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

    sf::Texture m_planetTexture;

    float m_zoomLevel = 1.0f;
    float m_mouseX = 0.0f;
    float m_mouseY = 0.0f;

    float m_cameraOffsetX = 0.0f;
    float m_cameraOffsetY = 0.0f;

    void generatePlanetTexture();

public:
    Graphics(sf::Vector2u windowSize);

    void drawStars(sf::RenderWindow& window);
    void draw(sf::RenderWindow& window, const std::vector<Body>& bodies);

    void resetTrails()
    {
        for (auto& trail : trails) trail.clear();
    }

    CoordinateSystem& getCoordinates() { return m_coords; }

    void setZoom(float zoom) { m_zoomLevel = zoom; }
    float getZoom() const { return m_zoomLevel; }

    void setMousePos(float x, float y) { m_mouseX = x; m_mouseY = y; }

    float getCameraOffsetX() const { return m_cameraOffsetX; }
    float getCameraOffsetY() const { return m_cameraOffsetY; }
    void setCameraOffset(float x, float y) { m_cameraOffsetX = x; m_cameraOffsetY = y; }
};
