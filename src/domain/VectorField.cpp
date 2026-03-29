#include "VectorField.hpp"
#include <cmath>

// Definir M_PI si no está definido (Windows no lo incluye por defecto)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace tp::domain {

VectorField::VectorField(FieldFunction func) : function_(std::move(func)) {}

Vec2d VectorField::getValue(double x, double y) const {
    if (function_) {
        return function_(x, y);
    }
    return Vec2d(0.0, 0.0);
}

Vec2d VectorField::getValue(const Vec2d& pos) const {
    return getValue(pos.x, pos.y);
}

void VectorField::setFunction(FieldFunction func) {
    function_ = std::move(func);
}

bool VectorField::isValid() const {
    return static_cast<bool>(function_);
}

VectorField VectorField::uniform(double vx, double vy) {
    return VectorField([vx, vy](double, double) {
        return Vec2d(vx, vy);
    });
}

VectorField VectorField::linear(double a, double b, double c, double d) {
    return VectorField([a, b, c, d](double x, double y) {
        return Vec2d(a * x + b * y, c * x + d * y);
    });
}

VectorField VectorField::radial(double cx, double cy, double k) {
    return VectorField([cx, cy, k](double x, double y) {
        double dx = x - cx;
        double dy = y - cy;
        return Vec2d(k * dx, k * dy);
    });
}

VectorField VectorField::rotational(double cx, double cy, double strength) {
    return VectorField([cx, cy, strength](double x, double y) {
        double dx = x - cx;
        double dy = y - cy;
        return Vec2d(-strength * dy, strength * dx);
    });
}

VectorField VectorField::vortex(double cx, double cy, double strength, double decay) {
    return VectorField([cx, cy, strength, decay](double x, double y) {
        double dx = x - cx;
        double dy = y - cy;
        double r = std::sqrt(dx * dx + dy * dy);
        double factor = strength * std::exp(-decay * r);
        return Vec2d(-factor * dy / (r + 0.001), factor * dx / (r + 0.001));
    });
}

VectorField VectorField::wave(double amplitude, double frequency, int direction) {
    return VectorField([amplitude, frequency, direction](double x, double y) {
        if (direction == 0) {
            // Onda horizontal
            double vy = amplitude * std::sin(frequency * x);
            return Vec2d(1.0, vy);
        } else {
            // Onda vertical
            double vx = amplitude * std::sin(frequency * y);
            return Vec2d(vx, 1.0);
        }
    });
}

VectorField VectorField::turbulence(int seed, double scale, double intensity) {
    // Generador simple de números pseudoaleatorios basado en seed
    return VectorField([seed, scale, intensity](double x, double y) {
        double nx = x / scale;
        double ny = y / scale;
        
        // Función de ruido simple basada en senos
        double angle1 = std::sin(nx * 12.9898 + ny * 78.233 + seed) * 43758.5453;
        double angle2 = std::sin(nx * 43.1234 + ny * 12.654 + seed * 2) * 23421.1234;
        
        double vx = intensity * std::sin(angle1 - std::floor(angle1));
        double vy = intensity * std::sin(angle2 - std::floor(angle2));
        
        return Vec2d(vx, vy);
    });
}

VectorField VectorField::doubleVortex(double cx1, double cy1, double cx2, double cy2, double strength) {
    return VectorField([cx1, cy1, cx2, cy2, strength](double x, double y) {
        // Primer vórtice
        double dx1 = x - cx1;
        double dy1 = y - cy1;
        double r1 = std::sqrt(dx1 * dx1 + dy1 * dy1) + 0.001;
        Vec2d v1(-strength * dy1 / r1, strength * dx1 / r1);
        
        // Segundo vórtice
        double dx2 = x - cx2;
        double dy2 = y - cy2;
        double r2 = std::sqrt(dx2 * dx2 + dy2 * dy2) + 0.001;
        Vec2d v2(-strength * dy2 / r2, strength * dx2 / r2);
        
        return Vec2d(v1.x + v2.x, v1.y + v2.y);
    });
}

VectorField VectorField::channelWithObstacle(double centerY, double width, double obstacleX, double flowSpeed) {
    return VectorField([centerY, width, obstacleX, flowSpeed](double x, double y) {
        // Flujo base del canal
        double distFromCenter = std::abs(y - centerY);
        double profile = 1.0 - std::pow(2.0 * distFromCenter / width, 2);
        profile = std::max(0.0, profile);
        
        Vec2d velocity(flowSpeed * profile, 0.0);
        
        // Obstáculo circular
        double obsY = centerY;
        double dx = x - obstacleX;
        double dy = y - obsY;
        double dist = std::sqrt(dx * dx + dy * dy);
        double obsRadius = width * 0.15;
        
        if (dist < obsRadius * 2.0 && dist > 0.001) {
            // Desviación alrededor del obstáculo
            double deflection = flowSpeed * 2.0 * std::exp(-dist / obsRadius);
            double angle = std::atan2(dy, dx);
            velocity.x += deflection * std::cos(angle + M_PI / 2);
            velocity.y += deflection * std::sin(angle + M_PI / 2);
        }
        
        return velocity;
    });
}

}
