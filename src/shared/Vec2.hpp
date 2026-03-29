#pragma once

/**
 * @file Vec2.hpp
 * @brief Template de vector 2D genérico
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Template básico para vectores 2D usado en todo el proyecto.
 * Especializado como Vec2d (double) para física y posiciones.
 * 
 * @note Template genérico - usar Vec2d para coordenadas físicas
 * @note Lanzada excepción en división por cero
 */

#include <cmath>
#include <stdexcept>

namespace tp::shared {

template <typename T>
struct Vec2 {
    T x;
    T y;
    
    Vec2() : x(0), y(0) {}
    Vec2(T x_val, T y_val) : x(x_val), y(y_val) {}
    
    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }
    
    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }
    
    Vec2 operator*(T scalar) const {
        return Vec2(x * scalar, y * scalar);
    }
    
    Vec2 operator/(T scalar) const {
        if (scalar == 0) throw std::runtime_error("Division by zero");
        return Vec2(x / scalar, y / scalar);
    }
    
    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    
    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    
    T dot(const Vec2& other) const {
        return x * other.x + y * other.y;
    }
    
    T magnitude() const {
        return std::sqrt(x * x + y * y);
    }
    
    T magnitudeSquared() const {
        return x * x + y * y;
    }
    
    Vec2 normalized() const {
        T mag = magnitude();
        if (mag == 0) return Vec2(0, 0);
        return *this / mag;
    }
    
    T distanceTo(const Vec2& other) const {
        return (*this - other).magnitude();
    }
};

using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;

}
