#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <array>
#include <cstdint>
#include "physics/Body.h"

enum class MenuScreen {
    Main,
    Controls,
    About,
    ScenarioSelect,
    CustomInput,
    Exit
};

enum class SelectedScenario {
    None,
    FigureEight,
    Chaotic,
    Hierarchical,
    Custom
};

class Menu {
public:
    Menu();
    
    SelectedScenario update(sf::RenderWindow& window);
    MenuScreen getCurrentScreen() const { return m_currentScreen; }
    void setMainScreen() { m_currentScreen = MenuScreen::Main; }
    void setCustomInputScreen() { m_currentScreen = MenuScreen::CustomInput; }

    void handleTextInput(std::uint32_t unicode);
    void removeLastChar();
    void moveFocus(int dx, int dy);
    
    std::array<std::string, 15> getCustomData() const { return m_customInputs; }

    void setError(const std::string& msg) { m_errorMessage = msg; }
    void clearError() { m_errorMessage.clear(); }

    void drawHUD(sf::RenderWindow& window, double timeScale, double simTime, bool isPaused);
    // Добавили totalBodies
    void drawEditPanel(sf::RenderWindow& window, int bodyIndex, int paramIndex, const Body& currentBody, int totalBodies);

private:
    MenuScreen m_currentScreen = MenuScreen::Main;

    sf::Font m_font;
    bool m_fontLoaded = false;

    std::array<std::string, 15> m_customInputs;
    int m_activeField = 0;

    std::string m_errorMessage;

    bool drawButton(sf::RenderWindow& window, const std::string& text, float yPos, float width, float height);
    void drawCustomInputForm(sf::RenderWindow& window);
};
