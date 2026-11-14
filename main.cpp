#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>

#include "src/physics/Scenarios.h"
#include "src/simulation/Simulation.h"
#include "src/simulation/SimulationConfig.h"
#include "src/graphics/Graphics.h"

int main()
{
    SimulationConfig config;
    
    std::cout << "Выберите пресет (1 - Восьмерка, 2 - Произвольный ввод): ";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        config.initialBodies = Scenarios::createFigureEight();
    } else {
        std::cout << "Введите параметры для 3-х тел.\n";
        std::cout << "Формат: mass posX posY velX velY\n";
        std::cout << "Совет: координаты от -1.0 до 1.0, скорости от -1.0 до 1.0.\n";
        
        for (int i = 0; i < 3; ++i) {
            std::cout << "Тело " << i + 1 << ": ";
            double m, px, py, vx, vy;
            std::cin >> m >> px >> py >> vx >> vy;
            
            config.initialBodies[i].mass = m;
            config.initialBodies[i].position.x = px;
            config.initialBodies[i].position.y = py;
            config.initialBodies[i].velocity.x = vx;
            config.initialBodies[i].velocity.y = vy;
            config.initialBodies[i].radius = 0.05; 
        }
    }

    // Создаем симуляцию. Она сама возьмет fixedTimeStep и timeScale из конфига
    Simulation simulation(config);
    Graphics graphics;

    sf::RenderWindow window(sf::VideoMode({1000, 700}), "Three Body Simulation");
    window.setFramerateLimit(60);

    simulation.start();
    sf::Clock clock;

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                // Пауза / Продолжить
                if (key->code == sf::Keyboard::Key::Space) {
                    if (simulation.isRunning()) simulation.pause();
                    else simulation.start();
                }
                
                // Сброс
                if (key->code == sf::Keyboard::Key::R) simulation.reset();
                
                // Выход
                if (key->code == sf::Keyboard::Key::Escape) window.close();
                
                // Управление скоростью времени
                if (key->code == sf::Keyboard::Key::Num1) simulation.setTimeScale(0.25);
                if (key->code == sf::Keyboard::Key::Num2) simulation.setTimeScale(0.5);
                if (key->code == sf::Keyboard::Key::Num3) simulation.setTimeScale(1.0);
                if (key->code == sf::Keyboard::Key::Num4) simulation.setTimeScale(2.0);
                if (key->code == sf::Keyboard::Key::Num5) simulation.setTimeScale(5.0);
            }
        }

        double deltaTime = clock.restart().asSeconds();
        deltaTime = std::min(deltaTime, 0.01);
        
        // Обновляем физику
        simulation.update(deltaTime);

        window.clear(sf::Color(8, 10, 22));
        graphics.draw(window, simulation.getBodies());
        window.display();
    }

    return 0;
}