
#include <SFML/Graphics.hpp>
#include <algorithm>

#include "src/physics/Scenarios.h"
#include "src/simulation/Simulation.h"
#include "src/simulation/SimulationConfig.h"
#include "src/graphics/Graphics.h"

int main()
{
    SimulationConfig config;
    config.initialBodies = Scenarios::createFigureEight();

    Simulation simulation(config);
    Graphics graphics;

    sf::RenderWindow window(
        sf::VideoMode({1000, 700}),
        "Three Body Simulation"
    );

    window.setFramerateLimit(60);

    simulation.start();

    sf::Clock clock;

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::KeyPressed>())
            {
                auto key = event->getIf<sf::Event::KeyPressed>();

                if (key->code == sf::Keyboard::Key::Space)
                {
                    if (simulation.isRunning())
                        simulation.pause();
                    else
                        simulation.start();
                }

                if (key->code == sf::Keyboard::Key::R)
                    simulation.reset();

                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();
            }
        }

        double deltaTime = clock.restart().asSeconds();
        deltaTime = std::min(deltaTime, 0.01);

        simulation.update(deltaTime);

        window.clear(sf::Color(8, 10, 22));

        graphics.draw(window, simulation.getBodies());

        window.display();
    }

    return 0;
}
