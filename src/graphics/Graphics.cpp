
#include "graphics/Graphics.h"

#include <cstdint>

Graphics::Graphics()
{
    m_coords = CoordinateSystem(
        {0.0, 0.0},
        100.0,
        {500.0, 350.0}
    );
}

void Graphics::draw(
    sf::RenderWindow& window,
    const std::vector<Body>& bodies
)
{
    auto size = window.getSize();

    m_coords.setScreenCenter({
        size.x / 2.0,
        size.y / 2.0
    });

    // If the number of bodies changes, reset all trails
    if (trails.size() != bodies.size())
    {
        trails.clear();
        trails.resize(bodies.size());
    }

    const sf::Color colors[3] = {
        sf::Color(255, 90, 90),
        sf::Color(80, 160, 255),
        sf::Color(255, 210, 70)
    };

    for (std::size_t i = 0; i < bodies.size(); ++i)
    {
        const sf::Color& color = colors[i % 3];

        Vector2D screenPos =
            m_coords.toScreen(bodies[i].position);

        float x = static_cast<float>(screenPos.x);
        float y = static_cast<float>(screenPos.y);

        // Record trajectory
        trails[i].push_back({x, y});

        if (trails[i].size() > 250)
            trails[i].pop_front();

        // Draw trajectory
        int index = 0;

        for (const auto& point : trails[i])
        {
            float alpha =
                30.f + 180.f * index / trails[i].size();

            sf::CircleShape dot(2.f);

            dot.setOrigin({2.f, 2.f});
            dot.setPosition(point);

            dot.setFillColor(sf::Color(
                color.r,
                color.g,
                color.b,
                static_cast<std::uint8_t>(alpha)
            ));

            window.draw(dot);
            index++;
        }

        // Draw body
        float screenRadius = static_cast<float>(
            bodies[i].radius * m_coords.getScale()
        );

        if (screenRadius < 2.f)
            screenRadius = 2.f;

        sf::CircleShape bodyShape(screenRadius);

        bodyShape.setOrigin({screenRadius, screenRadius});
        bodyShape.setPosition({x, y});
        bodyShape.setFillColor(color);

        window.draw(bodyShape);
    }
}
