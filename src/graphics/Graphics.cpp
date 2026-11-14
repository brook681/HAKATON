#include "graphics/Graphics.h"

Graphics::Graphics()
{
    // Инициализируем камеру: 
    // scale = 100 (как было у тебя), screenCenter = {500, 350} (для окна 1000x700)
    // worldCenter = {0, 0}
    m_coords = CoordinateSystem({0.0, 0.0}, 100.0, {500.0, 350.0});
}

void Graphics::draw(
    sf::RenderWindow& window,
    const std::array<Body, 3>& bodies
)
{
    auto size = window.getSize();
    
    // Если окно изменило размер — обновляем центр экрана в системе координат
    m_coords.setScreenCenter({size.x / 2.0, size.y / 2.0});

    const sf::Color colors[3] = {
        sf::Color(255, 90, 90),
        sf::Color(80, 160, 255),
        sf::Color(255, 210, 70)
    };

    for (int i = 0; i < 3; i++)
    {
        // Переводим физическую позицию в пиксели экрана
        Vector2D screenPos = m_coords.toScreen(bodies[i].position);
        float x = static_cast<float>(screenPos.x);
        float y = static_cast<float>(screenPos.y);

        // Записываем точку в траекторию
        trails[i].push_back({x, y});

        if (trails[i].size() > 250)
            trails[i].pop_front();

        // Рисуем траекторию
        int index = 0;
        for (const auto& point : trails[i])
        {
            float alpha = 30.f + 180.f * index / trails[i].size();

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

        // Рисуем тело (радиус берем из физики и умножаем на масштаб камеры)
        float screenRadius = static_cast<float>(bodies[i].radius * m_coords.getScale());
        
        // Защита от слишком маленьких или слишком больших кругов
        if (screenRadius < 2.f) screenRadius = 2.f;

        sf::CircleShape bodyShape(screenRadius);
        bodyShape.setOrigin({screenRadius, screenRadius});
        bodyShape.setPosition({x, y});
        bodyShape.setFillColor(colors[i]);

        window.draw(bodyShape);
    }
}