#include "graphics/Graphics.h"
#include <cstdint>
#include <algorithm>
#include <iostream>

static void drawBeautifulPlanet(sf::RenderWindow& window, sf::Vector2f center, float radius, sf::Color baseColor, const sf::Texture& texture)
{
    sf::CircleShape shadow(radius * 1.05f);
    shadow.setPointCount(100);
    shadow.setOrigin({ radius * 1.05f, radius * 1.05f });
    float shadowOffset = radius * 0.3f;
    shadow.setPosition({ center.x + shadowOffset, center.y + shadowOffset });
    shadow.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(shadow);

    for (int i = 4; i >= 1; --i)
    {
        float glowRadius = radius * (1.0f + 0.25f * i);
        sf::CircleShape glow(glowRadius);
        glow.setPointCount(100);
        glow.setOrigin({ glowRadius, glowRadius });
        glow.setPosition(center);
        sf::Color glowColor = baseColor;
        glowColor.a = static_cast<std::uint8_t>(20 / i);
        glow.setFillColor(glowColor);
        window.draw(glow);
    }

    sf::CircleShape planet(radius);
    planet.setPointCount(100);
    planet.setOrigin({ radius, radius });
    planet.setPosition(center);
    planet.setTexture(&texture);
    planet.setFillColor(sf::Color(baseColor.r, baseColor.g, baseColor.b, 200));
    window.draw(planet);

    float atmoRadius = radius * 1.02f;
    sf::CircleShape atmosphere(atmoRadius);
    atmosphere.setPointCount(100);
    atmosphere.setOrigin({ atmoRadius, atmoRadius });
    atmosphere.setPosition(center);
    sf::Color atmoColor = baseColor;
    atmoColor.a = 40;
    atmosphere.setFillColor(atmoColor);
    atmosphere.setOutlineThickness(0);
    window.draw(atmosphere);

    float highlightRadius = radius * 0.4f;
    sf::CircleShape highlight(highlightRadius);
    highlight.setPointCount(100);
    highlight.setOrigin({ highlightRadius, highlightRadius });
    highlight.setPosition({ center.x - radius * 0.4f, center.y - radius * 0.4f });
    highlight.setFillColor(sf::Color(255, 255, 255, 180));
    window.draw(highlight);
}

Graphics::Graphics(sf::Vector2u windowSize)
{
    m_coords = CoordinateSystem({ 0.0, 0.0 }, 100.0, { windowSize.x / 2.0f, windowSize.y / 2.0f });
    generatePlanetTexture();
}

void Graphics::drawStars(sf::RenderWindow& window)
{
    // Бесконечное звездное поле на основе координатной сетки
    const float cellSize = 80.0f;
    sf::Vector2f windowSize(window.getSize().x, window.getSize().y);

    for (int x = -2; x <= static_cast<int>(windowSize.x / cellSize) + 2; ++x)
    {
        for (int y = -2; y <= static_cast<int>(windowSize.y / cellSize) + 2; ++y)
        {
            float worldX = x * cellSize;
            float worldY = y * cellSize;

            float screenX = m_mouseX + (worldX - m_mouseX) * m_zoomLevel + m_cameraOffsetX;
            float screenY = m_mouseY + (worldY - m_mouseY) * m_zoomLevel + m_cameraOffsetY;

            if (screenX >= -20 && screenX <= windowSize.x + 20 && screenY >= -20 && screenY <= windowSize.y + 20)
            {
                unsigned int seed = static_cast<unsigned int>((x * 73856093) ^ (y * 19349663));
                float randVal = static_cast<float>(seed % 100) / 100.0f;

                if (randVal > 0.25f) continue; // не в каждой ячейке есть звезда

                float size = (randVal * 2.0f + 0.5f) * m_zoomLevel;
                if (size < 0.5f) size = 0.5f;
                int alpha = static_cast<int>(randVal * 155 + 100);

                sf::CircleShape star(size);
                star.setPointCount(8);
                star.setOrigin({ size, size });
                star.setPosition({ screenX, screenY });
                star.setFillColor(sf::Color(255, 255, 255, alpha));
                window.draw(star);
            }
        }
    }
}

void Graphics::generatePlanetTexture()
{
    sf::Image image(sf::Vector2u(256, 256));
    for (unsigned int x = 0; x < 256; ++x) {
        for (unsigned int y = 0; y < 256; ++y) {
            float dx = x - 128.f;
            float dy = y - 128.f;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist > 128.f) {
                image.setPixel({ x, y }, sf::Color::Transparent);
                continue;
            }
            int noise = static_cast<int>((std::sin(x * 0.1f) + std::cos(y * 0.1f) + std::sin(x * 0.05f + y * 0.05f)) * 20.f);
            int val = std::clamp(150 + noise, 50, 255);
            image.setPixel({ x, y }, sf::Color(val, val, val));
        }
    }
    if (!m_planetTexture.loadFromImage(image)) std::cerr << "Failed to load planet texture!\n";
}

void Graphics::draw(sf::RenderWindow& window, const std::vector<Body>& bodies)
{
    auto size = window.getSize();
    m_coords.setScreenCenter({ size.x / 2.0, size.y / 2.0 });

    // Градиентный фон (не обрезается)
    sf::VertexArray background(sf::PrimitiveType::TriangleFan, 4);
    background[0].position = { 0.f, 0.f };
    background[0].color = sf::Color(8, 10, 25);
    background[1].position = { static_cast<float>(size.x), 0.f };
    background[1].color = sf::Color(8, 10, 25);
    background[2].position = { static_cast<float>(size.x), static_cast<float>(size.y) };
    background[2].color = sf::Color(25, 30, 60);
    background[3].position = { 0.f, static_cast<float>(size.y) };
    background[3].color = sf::Color(25, 30, 60);
    window.draw(background);

    if (trails.size() != bodies.size()) {
        trails.clear();
        trails.resize(bodies.size());
    }

    drawStars(window);

    for (std::size_t i = 0; i < bodies.size(); ++i)
    {
        sf::Color color(bodies[i].r, bodies[i].g, bodies[i].b);
        Vector2D screenPos = m_coords.toScreen(bodies[i].position);

        float x = m_mouseX + (static_cast<float>(screenPos.x) - m_mouseX) * m_zoomLevel + m_cameraOffsetX;
        float y = m_mouseY + (static_cast<float>(screenPos.y) - m_mouseY) * m_zoomLevel + m_cameraOffsetY;

        trails[i].push_back({ x, y });
        if (trails[i].size() > 70) trails[i].pop_front();

        int index = 0;
        for (const auto& point : trails[i]) {
            float alpha = 10.f + 90.f * index / trails[i].size();
            sf::CircleShape dot(1.2f);
            dot.setPointCount(15);
            dot.setOrigin({ 1.2f, 1.2f });
            dot.setPosition(point);
            dot.setFillColor(sf::Color(color.r, color.g, color.b, static_cast<std::uint8_t>(alpha)));
            window.draw(dot);
            index++;
        }

        float screenRadius = static_cast<float>(bodies[i].radius * m_coords.getScale()) * m_zoomLevel;
        if (screenRadius < 2.f) screenRadius = 2.f;

        drawBeautifulPlanet(window, { x, y }, screenRadius, color, m_planetTexture);
    }
}
