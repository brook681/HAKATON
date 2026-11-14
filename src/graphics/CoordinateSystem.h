#pragma once

#include "physics/Body.h" // Компилятор сам найдет этот файл через пути в CMake

class CoordinateSystem
{
public:
    CoordinateSystem(Vector2D worldCenter = {0.0, 0.0}, 
                     double scale = 300.0, 
                     Vector2D screenCenter = {500.0, 350.0})
        : m_worldCenter(worldCenter), 
          m_scale(scale), 
          m_screenCenter(screenCenter) {}

    // Мир (метры) -> Экран (пиксели)
    Vector2D toScreen(const Vector2D& worldPos) const
    {
        double screenX = m_screenCenter.x + (worldPos.x - m_worldCenter.x) * m_scale;
        double screenY = m_screenCenter.y - (worldPos.y - m_worldCenter.y) * m_scale;
        return {screenX, screenY};
    }

    // Экран (пиксели) -> Мир (метры) — понадобится для кликов мышкой
    Vector2D toWorld(const Vector2D& screenPos) const
    {
        double worldX = m_worldCenter.x + (screenPos.x - m_screenCenter.x) / m_scale;
        double worldY = m_worldCenter.y - (screenPos.y - m_screenCenter.y) / m_scale;
        return {worldX, worldY};
    }

    double getScale() const { return m_scale; }
    void setScale(double scale) { m_scale = scale; }

    Vector2D getScreenCenter() const { return m_screenCenter; }
    void setScreenCenter(Vector2D center) { m_screenCenter = center; }

private:
    Vector2D m_worldCenter;
    double m_scale;
    Vector2D m_screenCenter;
};
