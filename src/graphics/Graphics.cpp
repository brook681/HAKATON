#include "graphics/Graphics.h"

#include <cstdint>
#include <algorithm>


static void drawBeautifulPlanet(sf::RenderWindow& window,
    sf::Vector2f center,
    float radius,
    sf::Color baseColor)
{
    // 1. ТЕНЬ
    sf::CircleShape shadow(radius * 1.05f);
    shadow.setPointCount(100);
    shadow.setOrigin({ radius * 1.05f, radius * 1.05f });
    float shadowOffset = radius * 0.3f;
    shadow.setPosition({ center.x + shadowOffset, center.y + shadowOffset });
    shadow.setFillColor(sf::Color(0, 0, 0, 120));
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

    // 3. Сама планета (тёмная основа)
    sf::CircleShape planet(radius);
    planet.setPointCount(100);
    planet.setOrigin({ radius, radius });
    planet.setPosition(center);

    sf::Color dark = sf::Color(
        static_cast<std::uint8_t>(baseColor.r * 0.3f),
        static_cast<std::uint8_t>(baseColor.g * 0.3f),
        static_cast<std::uint8_t>(baseColor.b * 0.3f)
    );
    planet.setFillColor(dark);
    window.draw(planet);

    // 4. АТМОСФЕРА
    float atmoRadius = radius * 0.95f;
    sf::CircleShape atmosphere(atmoRadius);
    atmosphere.setPointCount(100);
    atmosphere.setOrigin({ atmoRadius, atmoRadius });
    atmosphere.setPosition(center); 

    sf::Color atmoColor = baseColor;
    atmoColor.a = 80;
    atmoColor.r = std::min(255, static_cast<int>(atmoColor.r + 60));
    atmoColor.g = std::min(255, static_cast<int>(atmoColor.g + 60));
    atmoColor.b = std::min(255, static_cast<int>(atmoColor.b + 60));

    atmosphere.setFillColor(atmoColor);
    atmosphere.setOutlineThickness(0);
    window.draw(atmosphere);

    // 5. Блик
    float highlightRadius = radius * 0.8f;
    sf::CircleShape highlight(highlightRadius);
    highlight.setPointCount(100);
    highlight.setOrigin({ highlightRadius, highlightRadius });
    highlight.setPosition({ center.x - radius * 0.2f, center.y - radius * 0.2f });

    sf::Color light = sf::Color(
        std::min(255, static_cast<int>(baseColor.r + 100)),
        std::min(255, static_cast<int>(baseColor.g + 100)),
        std::min(255, static_cast<int>(baseColor.b + 100)),
        200
    );
    highlight.setFillColor(light);
    window.draw(highlight);
}

Graphics::Graphics()
{
    m_coords = CoordinateSystem(
        { 0.0, 0.0 },
        100.0,
        { 500.0, 350.0 }
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

    if (trails.size() != bodies.size())
    {
        trails.clear();
        trails.resize(bodies.size());
    }

    // Статический массив, чтобы хранить предыдущие цвета для вспышек
    static std::vector<sf::Color> previousColors;

    // Базовые цвета для новых планет
    const sf::Color defaultColors[3] = {
        sf::Color(255, 90, 90),
        sf::Color(80, 160, 255),
        sf::Color(255, 210, 70)
    };

    if (previousColors.size() != bodies.size())
    {
        previousColors.resize(bodies.size(), sf::Color(0, 0, 0, 0));
    }

    for (std::size_t i = 0; i < bodies.size(); ++i)
    {
        // Если планета только что создалась (цвет 255, 255, 255), даем ей базовый цвет
        sf::Color color(bodies[i].r, bodies[i].g, bodies[i].b);
        if (bodies[i].r == 255 && bodies[i].g == 255 && bodies[i].b == 255) {
            color = defaultColors[i % 3];
        }

        Vector2D screenPos = m_coords.toScreen(bodies[i].position);

        float x = static_cast<float>(screenPos.x);
        float y = static_cast<float>(screenPos.y);

        trails[i].push_back({ x, y });
        if (trails[i].size() > 250)
            trails[i].pop_front();

        int index = 0;
        for (const auto& point : trails[i])
        {
            float alpha = 30.f + 180.f * index / trails[i].size();
            sf::CircleShape dot(2.f);
            dot.setPointCount(20);
            dot.setOrigin({ 2.f, 2.f });
            dot.setPosition(point);
            dot.setFillColor(sf::Color(
                color.r, color.g, color.b,
                static_cast<std::uint8_t>(alpha)
            ));
            window.draw(dot);
            index++;
        }

        float screenRadius = static_cast<float>(
            bodies[i].radius * m_coords.getScale()
            );
        if (screenRadius < 2.f)
            screenRadius = 2.f;

        // ВСПЫШКА (если цвет изменился)
        if (previousColors[i].r != 0 && previousColors[i] != color) {

            // 1. Огромное мягкое свечение
            sf::CircleShape bigGlow(screenRadius * 15.0f);
            bigGlow.setPointCount(100);
            bigGlow.setOrigin({ screenRadius * 15.0f, screenRadius * 15.0f });
            bigGlow.setPosition({ x, y });
            bigGlow.setFillColor(sf::Color(color.r, color.g, color.b, 40));
            window.draw(bigGlow);

            // 2. Средний ореол
            sf::CircleShape midGlow(screenRadius * 6.0f);
            midGlow.setPointCount(100);
            midGlow.setOrigin({ screenRadius * 6.0f, screenRadius * 6.0f });
            midGlow.setPosition({ x, y });
            midGlow.setFillColor(sf::Color(color.r, color.g, color.b, 100));
            window.draw(midGlow);

            // 3. Яркое белое ядро
            sf::CircleShape core(screenRadius * 2.0f);
            core.setPointCount(100);
            core.setOrigin({ screenRadius * 2.0f, screenRadius * 2.0f });
            core.setPosition({ x, y });
            core.setFillColor(sf::Color(255, 255, 255, 230));
            window.draw(core);
        }
        previousColors[i] = color;

        // Вызов нашей новой функции для отрисовки планеты
        drawBeautifulPlanet(window, { x, y }, screenRadius, color);
    }
}