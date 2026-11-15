#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>

#include "graphics/Menu.h"
#include "graphics/Graphics.h"
#include "simulation/Simulation.h"
#include "physics/Scenarios.h"

int main()
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Three Body Simulation", sf::State::Fullscreen);
    window.setFramerateLimit(60);

    Menu menu;
    bool inGame = false;
    SimulationConfig currentConfig;

    // Создаем графику заранее для звезд в меню
    Graphics menuGraphics(window.getSize());

    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) window.close();

            if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                if (!inGame && menu.getCurrentScreen() == MenuScreen::CustomInput) {
                    if (textEvent->unicode == 8) { menu.removeLastChar(); } 
                    else { menu.handleTextInput(textEvent->unicode); }
                }
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (!inGame && menu.getCurrentScreen() == MenuScreen::CustomInput) {
                    if (key->code == sf::Keyboard::Key::Up)    menu.moveFocus(0, -1);
                    if (key->code == sf::Keyboard::Key::Down)  menu.moveFocus(0, 1);
                    if (key->code == sf::Keyboard::Key::Left)  menu.moveFocus(-1, 0);
                    if (key->code == sf::Keyboard::Key::Right) menu.moveFocus(1, 0);
                    if (key->code == sf::Keyboard::Key::Escape) menu.setMainScreen();
                }
            }
        }

        window.clear(sf::Color::Black);

        if (!inGame) {
            // Рисуем звезды в меню
            menuGraphics.drawStars(window);

            SelectedScenario choice = menu.update(window);
            
            if (choice != SelectedScenario::None) {
                try {
                    if (choice == SelectedScenario::FigureEight) {
                        currentConfig = SimulationConfig();
                        currentConfig.initialBodies = Scenarios::createFigureEight();
                        inGame = true;
                    }
                    else if (choice == SelectedScenario::Chaotic) {
                        currentConfig = SimulationConfig();
                        currentConfig.initialBodies = Scenarios::createChaotic();
                        inGame = true;
                    }
                    else if (choice == SelectedScenario::Hierarchical) {
                        currentConfig = SimulationConfig();
                        currentConfig.initialBodies = Scenarios::createHierarchical();
                        inGame = true;
                    }
                    else if (choice == SelectedScenario::Custom) {
                        currentConfig = SimulationConfig();
                        currentConfig.initialBodies.resize(3);
                        
                        auto data = menu.getCustomData();
                        for (int i = 0; i < 3; ++i) {
                            currentConfig.initialBodies[i].mass = std::stod(data[i * 5 + 0]);
                            currentConfig.initialBodies[i].position.x = std::stod(data[i * 5 + 1]);
                            currentConfig.initialBodies[i].position.y = std::stod(data[i * 5 + 2]);
                            currentConfig.initialBodies[i].velocity.x = std::stod(data[i * 5 + 3]);
                            currentConfig.initialBodies[i].velocity.y = std::stod(data[i * 5 + 4]);
                            currentConfig.initialBodies[i].radius = 0.05;
                            
                            if (i == 0) { currentConfig.initialBodies[i].r = 140; currentConfig.initialBodies[i].g = 200; currentConfig.initialBodies[i].b = 255; }
                            if (i == 1) { currentConfig.initialBodies[i].r = 255; currentConfig.initialBodies[i].g = 160; currentConfig.initialBodies[i].b = 122; }
                            if (i == 2) { currentConfig.initialBodies[i].r = 255; currentConfig.initialBodies[i].g = 240; currentConfig.initialBodies[i].b = 180; }
                        }
                        
                        Simulation simulation(currentConfig); 
                        inGame = true;
                        menu.clearError();
                    }
                } 
                catch (const std::exception& e) {
                    menu.setError("Error: " + std::string(e.what()));
                    if (choice == SelectedScenario::Custom) {
                        menu.setCustomInputScreen();
                    }
                }
            }
            
            if (menu.getCurrentScreen() == MenuScreen::Exit) window.close();
        } 
        else {
            Simulation simulation(currentConfig);
            simulation.start();
            sf::Clock clock;
            
            bool isEditing = false;
            int editBody = 0;
            int editParam = 0; 

            while (window.isOpen() && inGame) {
                while (const auto event = window.pollEvent()) {
                    if (event->is<sf::Event::Closed>()) window.close();
                    
                    if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                        if (key->code == sf::Keyboard::Key::Escape) {
                            if (isEditing) isEditing = false;
                            else { inGame = false; menu.setMainScreen(); }
                            break;
                        }
                        
                        if (key->code == sf::Keyboard::Key::E) {
                            isEditing = !isEditing;
                            if (isEditing) {
                                simulation.pause();
                                editBody = 0;
                            }
                            else simulation.start();
                        }
                        
                        if (!isEditing) {
                            if (key->code == sf::Keyboard::Key::Space) {
                                if (simulation.isRunning()) simulation.pause();
                                else if (simulation.getState() == SimulationState::Paused) simulation.start();
                            }
                            if (key->code == sf::Keyboard::Key::R) { simulation.reset(); menuGraphics.resetTrails(); }
                            if (key->code == sf::Keyboard::Key::Num1) simulation.setTimeScale(0.25);
                            if (key->code == sf::Keyboard::Key::Num2) simulation.setTimeScale(0.5);
                            if (key->code == sf::Keyboard::Key::Num3) simulation.setTimeScale(1.0);
                            if (key->code == sf::Keyboard::Key::Num4) simulation.setTimeScale(2.0);
                            if (key->code == sf::Keyboard::Key::Num5) simulation.setTimeScale(5.0);
                        } 
                        else {
                            auto currentBodies = simulation.snapshot();
                            int bodyCount = static_cast<int>(currentBodies.size());

                            if (bodyCount == 0) continue;

                            if (editBody >= bodyCount) editBody = 0;

                            if (key->code == sf::Keyboard::Key::Up)    editBody = (editBody + bodyCount - 1) % bodyCount;
                            if (key->code == sf::Keyboard::Key::Down)  editBody = (editBody + 1) % bodyCount;
                            if (key->code == sf::Keyboard::Key::Left)  editParam = (editParam + 5) % 6;
                            if (key->code == sf::Keyboard::Key::Right) editParam = (editParam + 1) % 6;
                            
                            double delta = 0.1;
                            
                            if (editBody < bodyCount) {
                                auto& currentBody = currentBodies[editBody];
                                
                                if (key->code == sf::Keyboard::Key::Add || key->code == sf::Keyboard::Key::Equal) {
                                    if (editParam == 0) simulation.setBodyMass(editBody, std::max(0.1, currentBody.mass + delta));
                                    if (editParam == 1) simulation.setBodyPosition(editBody, {currentBody.position.x + delta, currentBody.position.y});
                                    if (editParam == 2) simulation.setBodyPosition(editBody, {currentBody.position.x, currentBody.position.y + delta});
                                    if (editParam == 3) simulation.setBodyVelocity(editBody, {currentBody.velocity.x + delta, currentBody.velocity.y});
                                    if (editParam == 4) simulation.setBodyVelocity(editBody, {currentBody.velocity.x, currentBody.velocity.y + delta});
                                    if (editParam == 5) simulation.setBodyRadius(editBody, std::max(0.01, currentBody.radius + delta));
                                }
                                if (key->code == sf::Keyboard::Key::Subtract || key->code == sf::Keyboard::Key::Hyphen) {
                                    if (editParam == 0) simulation.setBodyMass(editBody, std::max(0.1, currentBody.mass - delta));
                                    if (editParam == 1) simulation.setBodyPosition(editBody, {currentBody.position.x - delta, currentBody.position.y});
                                    if (editParam == 2) simulation.setBodyPosition(editBody, {currentBody.position.x, currentBody.position.y - delta});
                                    if (editParam == 3) simulation.setBodyVelocity(editBody, {currentBody.velocity.x - delta, currentBody.velocity.y});
                                    if (editParam == 4) simulation.setBodyVelocity(editBody, {currentBody.velocity.x, currentBody.velocity.y - delta});
                                    if (editParam == 5) simulation.setBodyRadius(editBody, std::max(0.01, currentBody.radius - delta));
                                }
                            }
                        }
                    }
                }
                if (!inGame) break;

                double dt = clock.restart().asSeconds();
                dt = std::min(dt, 0.01);
                
                if (!isEditing) simulation.update(dt);
                
                window.clear(sf::Color::Black);
                menuGraphics.draw(window, simulation.snapshot());
                
                menu.drawHUD(window, simulation.getTimeScale(), simulation.getSimulationTime(), !simulation.isRunning());
                
                if (isEditing) {
                    auto bodies = simulation.snapshot();
                    if (!bodies.empty()) {
                        int safeEditBody = std::min(editBody, static_cast<int>(bodies.size()) - 1);
                        menu.drawEditPanel(window, safeEditBody, editParam, bodies[safeEditBody], static_cast<int>(bodies.size()));
                    }
                }
                
                window.display();
                
                if (simulation.getState() == SimulationState::Finished) { 
                    inGame = false; 
                    menu.setMainScreen(); 
                }
            }
        }
        window.display();
    }
    return 0;
}