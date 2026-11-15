#include <cmath>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <limits> // Добавили для очистки потока ввода

#include "src/physics/Scenarios.h"
#include "src/simulation/Simulation.h"
#include "src/simulation/SimulationConfig.h"
#include "src/graphics/Graphics.h"

// Возвращает: 0 - успех, 1 - ошибка (повторить меню), -1 - выход
int loadConfig(SimulationConfig& config)
{
    std::cout << "\n=== THREE BODY SIMULATION ===\n";
    std::cout << "1. Figure Eight\n";
    std::cout << "2. Chaotic\n";
    std::cout << "3. Hierarchical\n";
    std::cout << "4. Custom\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose scenario: ";

    int choice;

    if (!(std::cin >> choice))
    {
        std::cerr << "Invalid input.\n";
        std::cin.clear(); // Сбрасываем флаг ошибки
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очищаем буфер
        return 1; // Возвращаемся в меню
    }

    if (choice == 0) {
        return -1; // Выход из игры
    }

    switch (choice)
    {
    case 1:
        config.initialBodies = Scenarios::createFigureEight();
        break;

    case 2:
        config.initialBodies = Scenarios::createChaotic();
        break;

    case 3:
        config.initialBodies = Scenarios::createHierarchical();
        break;

    case 4:
        std::cout << "\nEnter parameters for 3 bodies:\n";
        std::cout << "Format: mass posX posY velX velY\n";

        config.initialBodies.resize(3);

        for (int i = 0; i < 3; ++i)
        {
            std::cout << "Body " << i + 1 << ": ";

            double mass, px, py, vx, vy;

            if (!(std::cin >> mass >> px >> py >> vx >> vy))
            {
                std::cerr << "\nInvalid parameters (non-numeric input).\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return 1; // Ошибка ввода
            }

            if (!std::isfinite(mass) || mass <= 0.0 ||
                !std::isfinite(px) || !std::isfinite(py) ||
                !std::isfinite(vx) || !std::isfinite(vy))
            {
                std::cerr << "\nInvalid parameters (mass must be > 0, others finite).\n";
                return 1; // Ошибка ввода
            }

            config.initialBodies[i].mass = mass;
            config.initialBodies[i].position = {px, py};
            config.initialBodies[i].velocity = {vx, vy};
            config.initialBodies[i].radius = 0.05;
        }
        break;

    default:
        std::cerr << "Invalid choice. Try again.\n";
        return 1; // Неверный номер пункта
    }

    return 0; // Успех
}

int main()
{
    while (true)
    {
        SimulationConfig config;
        
        // Запускаем загрузку конфига
        int loadResult = loadConfig(config);
        
        if (loadResult == -1) {
            break; // Выходим из главного цикла
        }
        if (loadResult == 1) {
            continue; // Возвращаемся в начало цикла (показываем меню заново)
        }

        // Если loadResult == 0, значит всё успешно
        Simulation simulation(config);
        Graphics graphics;

        sf::RenderWindow window(
            sf::VideoMode({1000, 700}),
            "Three Body Simulation"
        );

        window.setFramerateLimit(60);
        simulation.start();

        sf::Clock clock;
        bool returnToMenu = false;

        while (window.isOpen() && !returnToMenu)
        {
            while (const auto event = window.pollEvent())
            {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                    return 0; // Полный выход из программы
                }

                if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                {
                    if (key->code == sf::Keyboard::Key::Space)
                    {
                        if (simulation.isRunning())
                            simulation.pause();
                        else if (simulation.getState() == SimulationState::Paused)
                            simulation.start();
                    }

                    if (key->code == sf::Keyboard::Key::R)
                    {
                        simulation.reset();
                        graphics.resetTrails();
                    }

                    // Esc возвращает в меню
                    if (key->code == sf::Keyboard::Key::Escape) {
                        returnToMenu = true;
                        break;
                    }

                    if (key->code == sf::Keyboard::Key::Num1) simulation.setTimeScale(0.25);
                    if (key->code == sf::Keyboard::Key::Num2) simulation.setTimeScale(0.5);
                    if (key->code == sf::Keyboard::Key::Num3) simulation.setTimeScale(1.0);
                    if (key->code == sf::Keyboard::Key::Num4) simulation.setTimeScale(2.0);
                    if (key->code == sf::Keyboard::Key::Num5) simulation.setTimeScale(5.0);
                }
            }

            if (returnToMenu) break;

            double deltaTime = clock.restart().asSeconds();
            deltaTime = std::min(deltaTime, 0.01);

            simulation.update(deltaTime);

            window.clear(sf::Color(8, 10, 22));
            graphics.draw(window, simulation.snapshot());
            window.display();

            // Если симуляция завершилась (коллизия)
            if (simulation.getState() == SimulationState::Finished) {
                sf::sleep(sf::seconds(1.5f));
                returnToMenu = true;
            }
        }
    }

    return 0;
}