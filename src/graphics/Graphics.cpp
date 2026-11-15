#include "graphics/Graphics.h"

#include <cstdint>
#include <algorithm>
#include <iostream>

static void drawBeautifulPlanet(sf::RenderWindow& window,
    sf::Vector2f center,
    float radius,
    sf::Color baseColor,
    const sf::Texture& texture)
{
    // 1. ТЕНЬ
    sf::CircleShape shadow(radius * 1.05f);
    shadow.setPointCount(100);
    shadow.setOrigin({ radius * 1.05f, radius * 1.05f });
    float shadowOffset = radius * 0.3f;
    shadow.setPosition({ center.x + shadowOffset, center.y + shadowOffset });
    shadow.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(shadow);

    // 2. Свечение (ореол)
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

    // 3. Сама планета (с текстурой)
    sf::CircleShape planet(radius);
    planet.setPointCount(100);
    planet.setOrigin({ radius, radius });
    planet.setPosition(center);
    
    planet.setTexture(&texture);
    planet.setFillColor(sf::Color(baseColor.r, baseColor.g, baseColor.b, 200)); 
    
    window.draw(planet);

    // 4. АТМОСФЕРА
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

    // 5. Блик
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
    m_coords = CoordinateSystem(
        { 0.0, 0.0 },
        100.0,
        { windowSize.x / 2.0f, windowSize.y / 2.0f }
    );

    generateStars(windowSize);
    generatePlanetTexture();
}

void Graphics::drawStars(sf::RenderWindow& window)
{
    for (const auto& star : m_stars) {
        sf::CircleShape s(star.size);
        s.setPointCount(8);
        s.setPosition(star.pos);
        s.setFillColor(star.color);
        window.draw(s);
    }
}

void Graphics::generateStars(sf::Vector2u windowSize)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> xDist(0.f, static_cast<float>(windowSize.x));
    std::uniform_real_distribution<float> yDist(0.f, static_cast<float>(windowSize.y));
    std::uniform_real_distribution<float> sizeDist(0.5f, 2.5f);
    std::uniform_int_distribution<int> alphaDist(50, 255);

    for (int i = 0; i < 800; ++i) {
        Star star;
        star.pos = { xDist(gen), yDist(gen) };
        star.size = sizeDist(gen);
        star.color = sf::Color(255, 255, 255, alphaDist(gen));
        m_stars.push_back(star);
    }
}

void Graphics::generatePlanetTexture()
{
    sf::Image image({256, 256});
    std::random_device rd;
    std::mt19937 gen(rd());
    
    for (unsigned int x = 0; x < 256; ++x) {
        for (unsigned int y = 0; y < 256; ++y) {
            float dx = x - 128.f;
            float dy = y - 128.f;
            float dist = std::sqrt(dx*dx + dy*dy);
            
            if (dist > 128.f) {
                image.setPixel({x, y}, sf::Color::Transparent);
                continue;
            }

            int noise = static_cast<int>(
                (std::sin(x * 0.1f) + std::cos(y * 0.1f) + 
                 std::sin(x * 0.05f + y * 0.05f)) * 20.f
            );
            
            int val = std::clamp(150 + noise, 50, 255);
            image.setPixel({x, y}, sf::Color(val, val, val));
        }
    }
    if (!m_planetTexture.loadFromImage(image)) {
        std::cerr << "Failed to load planet texture!\n";
    }
}

void Graphics::draw(
    sf::RenderWindow& window,
    const std::vector<Body>& bodies
)
{
    auto size = window.getSize();
    m_coords.setScreenCenter({ size.x / 2.0, size.y / 2.0 });

    if (trails.size() != bodies.size())
    {
        trails.clear();
        trails.resize(bodies.size());
    }

    drawStars(window);

    for (std::size_t i = 0; i < bodies.size(); ++i)
    {
        sf::Color color(bodies[i].r, bodies[i].g, bodies[i].b);
        
        Vector2D screenPos = m_coords.toScreen(bodies[i].position);
        float x = static_cast<float>(screenPos.x);
        float y = static_cast<float>(screenPos.y);

        trails[i].push_back({ x, y });
        if (trails[i].size() > 70)
            trails[i].pop_front();

        int index = 0;
        for (const auto& point : trails[i])
        {
            float alpha = 10.f + 90.f * index / trails[i].size();
            sf::CircleShape dot(1.2f);
            dot.setPointCount(15);
            dot.setOrigin({ 1.2f, 1.2f });
            dot.setPosition(point);
            dot.setFillColor(sf::Color(color.r, color.g, color.b, static_cast<std::uint8_t>(alpha)));
            window.draw(dot);
            index++;
        }

        float screenRadius = static_cast<float>(bodies[i].radius * m_coords.getScale());
        if (screenRadius < 2.f) screenRadius = 2.f;

        drawBeautifulPlanet(window, { x, y }, screenRadius, color, m_planetTexture);
    }
}