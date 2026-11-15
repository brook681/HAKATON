#include "graphics/Menu.h"
#include <iostream>
#include <algorithm>
#include <cstdio>

Menu::Menu() {
    if (!m_font.openFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Warning: Could not load font for Menu!\n";
    } else {
        m_fontLoaded = true;
    }
    for (auto& s : m_customInputs) s = "0";
}

bool Menu::drawButton(sf::RenderWindow& window, const std::string& text, float yPos, float width, float height) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    sf::RectangleShape button({width, height});
    button.setOrigin({width / 2.f, height / 2.f});
    
    // Адаптация под полноэкранный режим: центрируем по вертикали,
    // отталкиваясь от базового разрешения 700px
    float yOffset = (window.getSize().y - 700.f) / 2.f;
    
    button.setPosition({window.getSize().x / 2.f, yPos + yOffset});
    button.setFillColor(sf::Color(40, 45, 70));
    button.setOutlineThickness(2.f);
    button.setOutlineColor(sf::Color(100, 120, 200));

    bool hovered = button.getGlobalBounds().contains(mousePos);
    if (hovered) button.setFillColor(sf::Color(60, 70, 110));
    window.draw(button);

    if (m_fontLoaded) {
        sf::Text label(m_font, text, 24);
        label.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = label.getLocalBounds();
        label.setOrigin({textBounds.position.x + textBounds.size.x / 2.f, textBounds.position.y + textBounds.size.y / 2.f});
        label.setPosition(button.getPosition());
        window.draw(label);
    }

    if (hovered && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        static sf::Clock clickClock;
        if (clickClock.getElapsedTime().asMilliseconds() > 200) {
            clickClock.restart();
            return true;
        }
    }
    return false;
}

void Menu::handleTextInput(std::uint32_t unicode) {
    if (m_activeField >= 0 && m_activeField < 15) {
        if (unicode >= '0' && unicode <= '9') {
            if (m_customInputs[m_activeField] == "0") m_customInputs[m_activeField] = "";
            m_customInputs[m_activeField] += static_cast<char>(unicode);
        }
        else if (unicode == '.') {
            if (m_customInputs[m_activeField].find('.') == std::string::npos) {
                m_customInputs[m_activeField] += ".";
            }
        }
        else if (unicode == '-') {
            if (m_customInputs[m_activeField].empty() || m_customInputs[m_activeField] == "0") {
                m_customInputs[m_activeField] = "-";
            }
        }
    }
}

void Menu::removeLastChar() {
    if (m_activeField >= 0 && m_activeField < 15) {
        if (!m_customInputs[m_activeField].empty()) {
            m_customInputs[m_activeField].pop_back();
        }
        if (m_customInputs[m_activeField].empty()) {
            m_customInputs[m_activeField] = "0";
        }
    }
}

void Menu::moveFocus(int dx, int dy) {
    int row = m_activeField / 5;
    int col = m_activeField % 5;

    row = std::clamp(row + dy, 0, 2);
    col = std::clamp(col + dx, 0, 4);

    m_activeField = row * 5 + col;
}

void Menu::drawCustomInputForm(sf::RenderWindow& window) {
    if (!m_fontLoaded) return;

    sf::Text title(m_font, "Enter parameters for 3 bodies", 28);
    title.setPosition({50, 30});
    window.draw(title);

    sf::Text hint(m_font, "Arrows - move, Backspace - delete, Esc - cancel", 18);
    hint.setFillColor(sf::Color(180, 180, 180));
    hint.setPosition({50, 70});
    window.draw(hint);

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    for (int i = 0; i < 15; ++i) {
        int bodyIndex = i / 5;
        int paramIndex = i % 5;

        float x = 50.f + paramIndex * 170.f;
        float y = 140.f + bodyIndex * 80.f;

        sf::RectangleShape field({160.f, 40.f});
        field.setPosition({x, y});
        field.setFillColor(sf::Color(30, 30, 40));
        field.setOutlineThickness(2.f);
        field.setOutlineColor(i == m_activeField ? sf::Color(100, 200, 255) : sf::Color(100, 100, 100));

        if (field.getGlobalBounds().contains(mousePos) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            m_activeField = i;
        }

        window.draw(field);

        sf::Text value(m_font, m_customInputs[i], 20);
        value.setPosition({x + 10.f, y + 8.f});
        window.draw(value);
        
        if (paramIndex == 0) {
            sf::Text bodyLabel(m_font, "Body " + std::to_string(bodyIndex + 1), 20);
            bodyLabel.setPosition({x, y - 30.f});
            window.draw(bodyLabel);
        }
    }

    if (!m_errorMessage.empty()) {
        sf::Text errorText(m_font, m_errorMessage, 20);
        errorText.setFillColor(sf::Color(255, 100, 100));
        errorText.setPosition({50.f, 530.f});
        window.draw(errorText);
    }
}

SelectedScenario Menu::update(sf::RenderWindow& window) {
    if (m_currentScreen == MenuScreen::Main) {
        if (drawButton(window, "Start Game", 250, 300, 60)) m_currentScreen = MenuScreen::ScenarioSelect;
        else if (drawButton(window, "Controls", 350, 300, 60)) m_currentScreen = MenuScreen::Controls;
        else if (drawButton(window, "About", 450, 300, 60)) m_currentScreen = MenuScreen::About;
        else if (drawButton(window, "Exit", 550, 300, 60)) { m_currentScreen = MenuScreen::Exit; return SelectedScenario::None; }
    }
    else if (m_currentScreen == MenuScreen::Controls) {
        if (m_fontLoaded) {
            sf::Text text(m_font, "Space - Pause / Resume\nR - Reset\nEsc - Return to Menu\n1-5 - Time Scale\nE - Edit Mode", 24);
            text.setPosition({280, 250});
            window.draw(text);
        }
        if (drawButton(window, "Back", 550, 200, 60)) m_currentScreen = MenuScreen::Main;
    }
    else if (m_currentScreen == MenuScreen::About) {
        if (m_fontLoaded) {
            sf::Text text(m_font, "Three Body Simulation\n\nThis is a simulation of gravitational\ninteraction between three bodies.", 24);
            text.setPosition({280, 250});
            window.draw(text);
        }
        if (drawButton(window, "Back", 550, 200, 60)) m_currentScreen = MenuScreen::Main;
    }
    else if (m_currentScreen == MenuScreen::ScenarioSelect) {
        if (drawButton(window, "Figure Eight", 180, 300, 50)) return SelectedScenario::FigureEight;
        if (drawButton(window, "Chaotic", 260, 300, 50)) return SelectedScenario::Chaotic;
        if (drawButton(window, "Hierarchical", 340, 300, 50)) return SelectedScenario::Hierarchical;
        if (drawButton(window, "Custom", 420, 300, 50)) { 
            m_currentScreen = MenuScreen::CustomInput; 
            m_activeField = 0; 
            clearError();
            return SelectedScenario::None; 
        }
        if (drawButton(window, "Back", 520, 200, 50)) m_currentScreen = MenuScreen::Main;
    }
    else if (m_currentScreen == MenuScreen::CustomInput) {
        drawCustomInputForm(window);
        
        if (drawButton(window, "Start Simulation", 600, 300, 50)) {
            return SelectedScenario::Custom; 
        }
        if (drawButton(window, "Cancel", 670, 200, 40)) { 
            m_currentScreen = MenuScreen::ScenarioSelect; 
            clearError();
        }
    }

    return SelectedScenario::None;
}

void Menu::drawHUD(sf::RenderWindow& window, double timeScale, double simTime, bool isPaused) {
    if (!m_fontLoaded) return;

    sf::RectangleShape hudBg({300.f, 90.f});
    hudBg.setPosition({10.f, 10.f});
    hudBg.setFillColor(sf::Color(0, 0, 0, 150));
    hudBg.setOutlineThickness(1.f);
    hudBg.setOutlineColor(sf::Color(100, 100, 100));
    window.draw(hudBg);

    sf::Text timeText(m_font, "Sim Time: " + std::to_string(simTime).substr(0, 5) + " s", 18);
    timeText.setPosition({20.f, 20.f});
    window.draw(timeText);
    
    sf::Text speedText(m_font, "Speed: " + std::to_string(timeScale).substr(0, 4) + "x", 18);
    speedText.setPosition({20.f, 45.f});
    window.draw(speedText);

    sf::Text statusText(m_font, isPaused ? "State: PAUSED" : "State: RUNNING", 18);
    statusText.setFillColor(isPaused ? sf::Color(255, 200, 100) : sf::Color(100, 255, 100));
    statusText.setPosition({20.f, 70.f});
    window.draw(statusText);

    sf::Text helpText(m_font, "Space-Pause | R-Reset | E-Edit | Esc-Menu", 14);
    helpText.setFillColor(sf::Color(180, 180, 180));
    helpText.setPosition({10.f, window.getSize().y - 30.f});
    window.draw(helpText);
}

void Menu::drawEditPanel(sf::RenderWindow& window, int bodyIndex, int paramIndex, const Body& currentBody) {
    if (!m_fontLoaded) return;

    sf::RectangleShape panel({350.f, 450.f});
    panel.setPosition({window.getSize().x - 370.f, 20.f});
    panel.setFillColor(sf::Color(0, 0, 0, 200));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(100, 200, 255));
    window.draw(panel);

    sf::Text title(m_font, "EDIT BODY MODE", 22);
    title.setPosition({window.getSize().x - 350.f, 30.f});
    title.setFillColor(sf::Color(255, 200, 100));
    window.draw(title);

    // Выбор тела
    for (int i = 0; i < 3; ++i) {
        sf::Text bodyText(m_font, "Body " + std::to_string(i + 1), 20);
        bodyText.setPosition({window.getSize().x - 350.f, 80.f + i * 40.f});
        bodyText.setFillColor(i == bodyIndex ? sf::Color(100, 255, 100) : sf::Color::White);
        window.draw(bodyText);
    }

    // Параметры и их значения
    const char* params[] = {"Mass", "Pos X", "Pos Y", "Vel X", "Vel Y", "Radius"};
    double values[] = {
        currentBody.mass,
        currentBody.position.x,
        currentBody.position.y,
        currentBody.velocity.x,
        currentBody.velocity.y,
        currentBody.radius
    };

    for (int i = 0; i < 6; ++i) {
        sf::Text paramText(m_font, std::string(params[i]), 20);
        paramText.setPosition({window.getSize().x - 350.f, 200.f + i * 35.f});
        paramText.setFillColor(i == paramIndex ? sf::Color(100, 255, 100) : sf::Color(200, 200, 200));
        window.draw(paramText);

        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.2f", values[i]);
        sf::Text valueText(m_font, buffer, 20);
        valueText.setPosition({window.getSize().x - 200.f, 200.f + i * 35.f});
        valueText.setFillColor(i == paramIndex ? sf::Color(100, 255, 100) : sf::Color::White);
        window.draw(valueText);
    }

    sf::Text help(m_font, "Up/Down - Body\nLeft/Right - Param\n+/- - Change\nE - Close", 16);
    help.setPosition({window.getSize().x - 350.f, 420.f});
    help.setFillColor(sf::Color(180, 180, 180));
    window.draw(help);
}