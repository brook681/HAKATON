#include "graphics/Graphics.h"

void Graphics::draw(
    sf::RenderWindow& window,
    const std::array<Body, 3>& bodies
)
{
    const float SCALE = 100.f;

    auto size = window.getSize();

    float centerX = size.x / 2.f;
    float centerY = size.y / 2.f;

    const sf::Color colors[3] = {
        sf::Color(255, 90, 90),
        sf::Color(80, 160, 255),
        sf::Color(255, 210, 70)
    };

    for (int i = 0; i < 3; i++)
    {
        float x = centerX + static_cast<float>(bodies[i].position.x) * SCALE;
        float y = centerY - static_cast<float>(bodies[i].position.y) * SCALE;

        trails[i].push_back({x, y});

        if (trails[i].size() > 250)
            trails[i].pop_front();

        // Рисуем траекторию
        int index = 0;

        for (const auto& point : trails[i])
        {
            float alpha = 30.f +
                180.f * index / trails[i].size();

            sf::CircleShape dot(2.f);

            dot.setOrigin({2.f, 2.f});
            dot.setPosition(point);
            dot.setFillColor(sf::Color(
                colors[i].r,
                colors[i].g,
                colors[i].b,
                static_cast<std::uint8_t>(alpha)
            ));

            window.draw(dot);
            index++;
        }

        // Рисуем тело
        sf::CircleShape body(9.f);

        body.setOrigin({9.f, 9.f});
        body.setPosition({x, y});
        body.setFillColor(colors[i]);

        window.draw(body);
    }
}