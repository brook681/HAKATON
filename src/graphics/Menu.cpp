
#include "graphics/Menu.h"

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <string>

Menu::Menu()
{
    m_fontLoaded = false;

    const char* fontPaths[] = {
        "assets/fonts/arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    };

    for (const char* path : fontPaths)
    {
        if (m_font.openFromFile(path))
        {
            m_fontLoaded = true;
            std::cout << "Font loaded: " << path << '\n';
            break;
        }
    }

    if (!m_fontLoaded)
    {
        std::cerr << "ERROR: Could not load any font!\n";
    }

    for (auto& input : m_customInputs)
        input = "0";
}

bool Menu::drawButton(
    sf::RenderWindow& window,
    const std::string& text,
    float yPos,
    float width,
    float height)
{
    sf::Vector2f mousePos =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));

    sf::RectangleShape button({ width, height });

    button.setOrigin({ width / 2.f, height / 2.f });

    float yOffset = (window.getSize().y - 700.f) / 2.f;

    button.setPosition({
        window.getSize().x / 2.f,
        yPos + yOffset
    });

    button.setFillColor(sf::Color(15, 18, 25, 200));
    button.setOutlineThickness(2.f);
    button.setOutlineColor(sf::Color(200, 160, 255));

    bool hovered = button.getGlobalBounds().contains(mousePos);

    if (hovered)
    {
        button.setFillColor(sf::Color(35, 25, 50, 220));
        button.setOutlineColor(sf::Color(230, 200, 255));
        button.setOutlineThickness(3.f);
    }

    window.draw(button);

    if (m_fontLoaded)
    {
        sf::Text label(m_font, text, 24);

        label.setFillColor(
            hovered ? sf::Color(240, 220, 255) : sf::Color::White
        );

        sf::FloatRect bounds = label.getLocalBounds();

        label.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
        });

        label.setPosition(button.getPosition());

        window.draw(label);
    }

    if (hovered &&
        sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        static sf::Clock clickClock;

        if (clickClock.getElapsedTime().asMilliseconds() > 200)
        {
            clickClock.restart();
            return true;
        }
    }

    return false;
}

void Menu::handleTextInput(std::uint32_t unicode)
{
    if (m_activeField < 0 || m_activeField >= 15)
        return;

    std::string& value = m_customInputs[m_activeField];

    if (unicode >= '0' && unicode <= '9')
    {
        if (value == "0")
            value.clear();

        value += static_cast<char>(unicode);
    }
    else if (unicode == '.')
    {
        if (value.find('.') == std::string::npos)
        {
            if (value.empty() || value == "-")
                value += "0";

            value += '.';
        }
    }
    else if (unicode == '-')
    {
        if (value.empty() || value == "0")
            value = "-";
    }
}

void Menu::removeLastChar()
{
    if (m_activeField < 0 || m_activeField >= 15)
        return;

    std::string& value = m_customInputs[m_activeField];

    if (!value.empty())
        value.pop_back();

    if (value.empty() || value == "-")
        value = "0";
}

void Menu::moveFocus(int dx, int dy)
{
    int row = m_activeField / 5;
    int col = m_activeField % 5;

    row = std::clamp(row + dy, 0, 2);
    col = std::clamp(col + dx, 0, 4);

    m_activeField = row * 5 + col;
}

void Menu::drawCustomInputForm(sf::RenderWindow& window)
{
    if (!m_fontLoaded)
        return;

    float tableWidth = 5 * 170.f;
    float tableHeight = 3 * 80.f;

    float startX =
        (window.getSize().x - tableWidth) / 2.f + 40.f;

    float startY =
        (window.getSize().y - tableHeight) / 2.f - 50.f;

    sf::Text title(m_font, "Enter parameters for 3 bodies", 28);

    sf::FloatRect titleBounds = title.getLocalBounds();

    title.setOrigin({
        titleBounds.position.x + titleBounds.size.x / 2.f,
        titleBounds.position.y + titleBounds.size.y / 2.f
    });

    title.setPosition({
        window.getSize().x / 2.f,
        startY - 120.f
    });

    window.draw(title);

    sf::Text hint(
        m_font,
        "Arrows - move, Backspace - delete, Esc - cancel",
        18
    );

    sf::FloatRect hintBounds = hint.getLocalBounds();

    hint.setOrigin({
        hintBounds.position.x + hintBounds.size.x / 2.f,
        hintBounds.position.y + hintBounds.size.y / 2.f
    });

    hint.setFillColor(sf::Color(180, 180, 180));

    hint.setPosition({
        window.getSize().x / 2.f,
        startY - 90.f
    });

    window.draw(hint);

    const char* paramNames[] = {
        "Mass", "Pos X", "Pos Y", "Vel X", "Vel Y"
    };

    for (int p = 0; p < 5; ++p)
    {
        sf::Text colLabel(m_font, paramNames[p], 18);

        colLabel.setFillColor(sf::Color(200, 160, 255));

        colLabel.setPosition({
            startX + p * 170.f,
            startY - 30.f
        });

        window.draw(colLabel);
    }

    sf::Vector2f mousePos =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));

    for (int i = 0; i < 15; ++i)
    {
        int bodyIndex = i / 5;
        int paramIndex = i % 5;

        float x = startX + paramIndex * 170.f;
        float y = startY + bodyIndex * 80.f;

        if (paramIndex == 0)
        {
            sf::Text bodyLabel(
                m_font,
                "Body " + std::to_string(bodyIndex + 1),
                20
            );

            bodyLabel.setPosition({ x - 80.f, y + 8.f });
            bodyLabel.setFillColor(sf::Color(200, 200, 200));

            window.draw(bodyLabel);
        }

        sf::RectangleShape field({ 160.f, 40.f });

        field.setPosition({ x, y });
        field.setFillColor(sf::Color(30, 30, 40));
        field.setOutlineThickness(2.f);

        field.setOutlineColor(
            i == m_activeField
                ? sf::Color(200, 160, 255)
                : sf::Color(100, 100, 100)
        );

        if (field.getGlobalBounds().contains(mousePos) &&
            sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            m_activeField = i;
        }

        window.draw(field);

        sf::Text value(m_font, m_customInputs[i], 20);

        value.setPosition({ x + 10.f, y + 8.f });

        window.draw(value);
    }

    sf::Text tips(
        m_font,
        "Tips: Mass ~ 1.0 (avoid 0 or huge numbers)\n"
        "Pos X/Y: -3.0 to 3.0 (to stay on screen)\n"
        "Vel X/Y: -1.0 to 1.0 (0 means no movement)",
        16
    );

    tips.setFillColor(sf::Color(150, 150, 150));
    tips.setPosition({ startX - 40.f, startY + 260.f });

    window.draw(tips);

    if (!m_errorMessage.empty())
    {
        sf::Text errorText(m_font, m_errorMessage, 20);

        sf::FloatRect bounds = errorText.getLocalBounds();

        errorText.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
        });

        errorText.setFillColor(sf::Color(255, 100, 100));

        errorText.setPosition({
            window.getSize().x / 2.f,
            startY + 340.f
        });

        window.draw(errorText);
    }
}

SelectedScenario Menu::update(sf::RenderWindow& window)
{
    if (m_currentScreen == MenuScreen::Main)
    {
        if (drawButton(window, "Start Game", 250, 300, 60))
            m_currentScreen = MenuScreen::ScenarioSelect;

        else if (drawButton(window, "Controls", 350, 300, 60))
            m_currentScreen = MenuScreen::Controls;

        else if (drawButton(window, "About", 450, 300, 60))
            m_currentScreen = MenuScreen::About;

        else if (drawButton(window, "Exit", 550, 300, 60))
        {
            m_currentScreen = MenuScreen::Exit;
            return SelectedScenario::None;
        }
    }
    else if (m_currentScreen == MenuScreen::Controls)
    {
        if (m_fontLoaded)
        {
            sf::Text text(
                m_font,
                "Space - Pause / Resume\n"
                "R - Reset\n"
                "Esc - Return to Menu\n"
                "1-5 - Time Scale\n"
                "E - Edit Mode",
                24
            );

            text.setPosition({ 280.f, 250.f });
            window.draw(text);
        }

        if (drawButton(window, "Back", 550, 200, 60))
            m_currentScreen = MenuScreen::Main;
    }
    else if (m_currentScreen == MenuScreen::About)
    {
        if (m_fontLoaded)
        {
            sf::Text text(
                m_font,
                "Three Body Simulation\n\n"
                "This is a simulation of gravitational\n"
                "interaction between three bodies.",
                24
            );

            text.setPosition({ 280.f, 250.f });
            window.draw(text);
        }

        if (drawButton(window, "Back", 550, 200, 60))
            m_currentScreen = MenuScreen::Main;
    }
    else if (m_currentScreen == MenuScreen::ScenarioSelect)
    {
        if (drawButton(window, "Figure Eight", 180, 300, 50))
            return SelectedScenario::FigureEight;

        if (drawButton(window, "Chaotic", 260, 300, 50))
            return SelectedScenario::Chaotic;

        if (drawButton(window, "Hierarchical", 340, 300, 50))
            return SelectedScenario::Hierarchical;

        if (drawButton(window, "Custom", 420, 300, 50))
        {
            m_currentScreen = MenuScreen::CustomInput;
            m_activeField = 0;
            clearError();

            return SelectedScenario::None;
        }

        if (drawButton(window, "Back", 520, 200, 50))
            m_currentScreen = MenuScreen::Main;
    }
    else if (m_currentScreen == MenuScreen::CustomInput)
    {
        drawCustomInputForm(window);

        if (drawButton(window, "Start Simulation", 600, 300, 50))
            return SelectedScenario::Custom;

        if (drawButton(window, "Cancel", 670, 200, 40))
        {
            m_currentScreen = MenuScreen::ScenarioSelect;
            clearError();
        }
    }

    return SelectedScenario::None;
}

void Menu::drawHUD(
    sf::RenderWindow& window,
    double timeScale,
    double simTime,
    bool isPaused)
{
    if (!m_fontLoaded)
        return;

    sf::RectangleShape hudBg({ 300.f, 90.f });

    hudBg.setPosition({ 10.f, 10.f });
    hudBg.setFillColor(sf::Color(0, 0, 0, 150));
    hudBg.setOutlineThickness(1.f);
    hudBg.setOutlineColor(sf::Color(200, 160, 255));

    window.draw(hudBg);

    sf::Text timeText(
        m_font,
        "Sim Time: " + std::to_string(simTime).substr(0, 5) + " s",
        18
    );

    timeText.setPosition({ 20.f, 20.f });
    window.draw(timeText);

    sf::Text speedText(
        m_font,
        "Speed: " + std::to_string(timeScale).substr(0, 4) + "x",
        18
    );

    speedText.setPosition({ 20.f, 45.f });
    window.draw(speedText);

    sf::Text statusText(
        m_font,
        isPaused ? "State: PAUSED" : "State: RUNNING",
        18
    );

    statusText.setFillColor(
        isPaused
            ? sf::Color(255, 200, 100)
            : sf::Color(100, 255, 100)
    );

    statusText.setPosition({ 20.f, 70.f });
    window.draw(statusText);

    sf::Text helpText(
        m_font,
        "Space-Pause | R-Reset | E-Edit | Esc-Menu",
        14
    );

    helpText.setFillColor(sf::Color(180, 180, 180));

    helpText.setPosition({
        10.f,
        static_cast<float>(window.getSize().y) - 30.f
    });

    window.draw(helpText);
}

void Menu::drawEditPanel(
    sf::RenderWindow& window,
    int bodyIndex,
    int paramIndex,
    const Body& currentBody,
    int totalBodies)
{
    if (!m_fontLoaded)
        return;

    sf::RectangleShape panel({ 350.f, 450.f });

    panel.setPosition({
        static_cast<float>(window.getSize().x) - 370.f,
        20.f
    });

    panel.setFillColor(sf::Color(0, 0, 0, 200));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(200, 160, 255));

    window.draw(panel);

    sf::Text title(m_font, "EDIT BODY MODE", 22);

    title.setPosition({
        static_cast<float>(window.getSize().x) - 350.f,
        30.f
    });

    title.setFillColor(sf::Color(255, 200, 100));

    window.draw(title);

    for (int i = 0; i < totalBodies; ++i)
    {
        sf::Text bodyText(
            m_font,
            "Body " + std::to_string(i + 1),
            20
        );

        bodyText.setPosition({
            static_cast<float>(window.getSize().x) - 350.f,
            80.f + i * 40.f
        });

        bodyText.setFillColor(
            i == bodyIndex
                ? sf::Color(200, 160, 255)
                : sf::Color::White
        );

        window.draw(bodyText);
    }

    if (bodyIndex >= 0 && bodyIndex < totalBodies)
    {
        const char* params[] = {
            "Mass", "Pos X", "Pos Y",
            "Vel X", "Vel Y", "Radius"
        };

        double values[] = {
            currentBody.mass,
            currentBody.position.x,
            currentBody.position.y,
            currentBody.velocity.x,
            currentBody.velocity.y,
            currentBody.radius
        };

        for (int i = 0; i < 6; ++i)
        {
            sf::Text paramText(m_font, params[i], 20);

            paramText.setPosition({
                static_cast<float>(window.getSize().x) - 350.f,
                200.f + i * 35.f
            });

            paramText.setFillColor(
                i == paramIndex
                    ? sf::Color(200, 160, 255)
                    : sf::Color(200, 200, 200)
            );

            window.draw(paramText);

            char buffer[32];

            std::snprintf(
                buffer,
                sizeof(buffer),
                "%.2f",
                values[i]
            );

            sf::Text valueText(m_font, buffer, 20);

            valueText.setPosition({
                static_cast<float>(window.getSize().x) - 200.f,
                200.f + i * 35.f
            });

            valueText.setFillColor(
                i == paramIndex
                    ? sf::Color(200, 160, 255)
                    : sf::Color::White
            );

            window.draw(valueText);
        }
    }

    sf::Text help(
        m_font,
        "Up/Down - Body\n"
        "Left/Right - Param\n"
        "+/- - Change\n"
        "E - Close",
        16
    );

    help.setPosition({
        static_cast<float>(window.getSize().x) - 350.f,
        420.f
    });

    help.setFillColor(sf::Color(180, 180, 180));

    window.draw(help);
}
