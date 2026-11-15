#include <cmath>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <limits>

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
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return 1;
    }

    if (choice == 0) return -1;

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
                return 1;
            }

            if (!std::isfinite(mass) || mass <= 0.0 ||
                !std::isfinite(px) || !std::isfinite(py) ||
                !std::isfinite(vx) || !std::isfinite(vy))
            {
                std::cerr << "\nInvalid parameters (mass must be > 0).\n";
                return 1;
            }

            config.initialBodies[i].mass = mass;
            config.initialBodies[i].position = {px, py};
            config.initialBodies[i].velocity = {vx, vy};
            config.initialBodies[i].radius = 0.05;
        }
        break;
    default:
        std::cerr << "Invalid choice. Try again.\n";
        return 1;
    }

    return 0;
}

int main()
{
    while (true)
    {
        SimulationConfig config;
        
        int loadResult = loadConfig(config);
        if (loadResult == -1) break;
        if (loadResult == 1) continue;

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
                    return 0;
                }

                if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                {
                    if (key->code == sf::Keyboard::Key::Space) {
                        if (simulation.isRunning()) simulation.pause();
                        else if (simulation.getState() == SimulationState::Paused) simulation.start();
                    }
                    if (key->code == sf::Keyboard::Key::R) {
                        simulation.reset();
                        graphics.resetTrails();
                    }
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

            // --- ПРОВЕРКА НА ВЫЛЕТ ТЕЛ ЗА ГРАНИЦЫ ---
            auto bodies = simulation.snapshot();
            const double ESCAPE_RADIUS = 10.0;
            
            bool allEscaped = true;
            for (const auto& body : bodies) {
                if (body.position.length() <= ESCAPE_RADIUS) {
                    allEscaped = false;
                    break;
                }
            }

            if (allEscaped) {
                std::cout << "\n[!] All bodies have escaped the simulation area (distance > " 
                          << ESCAPE_RADIUS << " m).\n";
                returnToMenu = true;
            }

            // Если симуляция завершилась (коллизия)
            if (simulation.getState() == SimulationState::Finished) {
                sf::sleep(sf::seconds(1.5f));
                returnToMenu = true;
            }
        }
    }

    return 0;
}