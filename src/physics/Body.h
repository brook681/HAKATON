#pragma once

#include <cmath>

struct Vector2D
{
    double x = 0.0;
    double y = 0.0;

    Vector2D operator+(const Vector2D& other) const
    {
        return {x + other.x, y + other.y};
    }

    Vector2D operator-(const Vector2D& other) const
    {
        return {x - other.x, y - other.y};
    }

    Vector2D operator*(double scalar) const
    {
        return {x * scalar, y * scalar};
    }

    Vector2D operator/(double scalar) const
    {
        return {x / scalar, y / scalar};
    }

    double lengthSquared() const
    {
        return x * x + y * y;
    }

    double length() const
    {
        return std::sqrt(lengthSquared());
    }
};

struct Body
{
    double mass = 1.0;

    Vector2D position;
    Vector2D velocity;
    Vector2D acceleration;

    double radius = 0.1;

    unsigned char r = 255;
    unsigned char g = 255;
    unsigned char b = 255;
};
