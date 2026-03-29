#include "WaterParticleSystem.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace tp::presentation {

WaterParticleSystem::WaterParticleSystem(int maxParticles)
    : maxParticles_(maxParticles)
    , rng_(std::random_device{}())
{
    particles_.reserve(maxParticles);
}

void WaterParticleSystem::update(double dt) {
    // Actualizar todas las partículas
    for (auto it = particles_.begin(); it != particles_.end();) {
        it->life -= dt / it->maxLife;
        
        // Mover partícula
        it->position = it->position + it->velocity * dt;
        
        // Aplicar fricción
        it->velocity = it->velocity * 0.98;
        
        // Desvanecer espuma
        if (it->type == 1) { // espuma
            it->size *= 0.995;
        }
        
        // Eliminar partículas muertas
        if (it->life <= 0.0 || it->size < 0.5) {
            it = particles_.erase(it);
        } else {
            ++it;
        }
    }
}

void WaterParticleSystem::render(wxDC& dc, double scale, const wxRect& viewport) {
    for (const auto& p : particles_) {
        // Calcular posición en pantalla
        int screenX = viewport.x + static_cast<int>(p.position.x * scale);
        int screenY = viewport.y + static_cast<int>(p.position.y * scale);
        
        // Verificar si está en viewport
        if (screenX < viewport.x - 10 || screenX > viewport.x + viewport.width + 10 ||
            screenY < viewport.y - 10 || screenY > viewport.y + viewport.height + 10) {
            continue;
        }
        
        // Calcular alpha basado en vida
        int alpha = static_cast<int>(p.life * 200);
        if (alpha > 255) alpha = 255;
        if (alpha < 0) alpha = 0;
        
        wxColour color = p.color;
        wxColour drawColor(color.Red(), color.Green(), color.Blue(), alpha);
        
        dc.SetBrush(wxBrush(drawColor));
        dc.SetPen(wxPen(drawColor, 1));
        
        int drawSize = static_cast<int>(p.size * scale * 0.1);
        if (drawSize < 1) drawSize = 1;
        if (drawSize > 5) drawSize = 5;
        
        // Dibujar círculo suave
        dc.DrawCircle(screenX, screenY, drawSize);
    }
}

void WaterParticleSystem::emitWater(const Vec2d& pos, const Vec2d& velocity, int count) {
    std::uniform_real_distribution<double> angleDist(-M_PI, M_PI);
    std::uniform_real_distribution<double> speedDist(0.5, 2.0);
    std::uniform_real_distribution<double> lifeDist(1.0, 3.0);
    std::uniform_real_distribution<double> sizeDist(2.0, 4.0);
    
    for (int i = 0; i < count && particles_.size() < static_cast<size_t>(maxParticles_); ++i) {
        WaterParticle p;
        p.position = pos;
        
        double angle = angleDist(rng_);
        double speed = speedDist(rng_);
        p.velocity = Vec2d(std::cos(angle) * speed, std::sin(angle) * speed);
        p.velocity = p.velocity + velocity * 0.3; // Arrastre con el campo
        
        p.life = 1.0;
        p.maxLife = lifeDist(rng_);
        p.type = 0; // agua
        p.size = sizeDist(rng_);
        p.color = wxColour(100, 150, 200); // Azul agua
        
        particles_.push_back(p);
    }
}

void WaterParticleSystem::emitFoam(const Vec2d& pos, double intensity, int count) {
    std::uniform_real_distribution<double> angleDist(-M_PI, M_PI);
    std::uniform_real_distribution<double> speedDist(0.2, 1.0);
    std::uniform_real_distribution<double> lifeDist(5.0, 10.0);  // Vida más larga para espuma
    std::uniform_real_distribution<double> sizeDist(3.0, 6.0);
    
    int emitCount = static_cast<int>(count * intensity);
    
    for (int i = 0; i < emitCount && particles_.size() < static_cast<size_t>(maxParticles_); ++i) {
        WaterParticle p;
        p.position = pos;
        
        double angle = angleDist(rng_);
        double speed = speedDist(rng_);
        p.velocity = Vec2d(std::cos(angle) * speed, std::sin(angle) * speed);
        
        p.life = 1.0;
        p.maxLife = lifeDist(rng_);
        p.type = 1; // espuma
        p.size = sizeDist(rng_) * intensity;
        p.color = wxColour(240, 248, 255); // Blanco azulado
        
        particles_.push_back(p);
    }
}

void WaterParticleSystem::emitWake(const Vec2d& pos, const Vec2d& boatVelocity, double boatSpeed) {
    if (boatSpeed < 0.5) return; // No estela si está muy lento
    
    std::uniform_real_distribution<double> offsetDist(-2.0, 2.0);
    std::uniform_real_distribution<double> lifeDist(3.0, 6.0);
    std::uniform_real_distribution<double> sizeDist(4.0, 8.0);
    
    // Dirección opuesta a la velocidad del bote
    Vec2d wakeDir = boatVelocity * (-1.0 / boatSpeed);
    
    int particleCount = static_cast<int>(boatSpeed * 2.0);
    if (particleCount > 5) particleCount = 5;
    
    for (int i = 0; i < particleCount && particles_.size() < static_cast<size_t>(maxParticles_); ++i) {
        WaterParticle p;
        
        // Posición detrás del bote con dispersión lateral
        double offset = offsetDist(rng_);
        Vec2d lateral(-wakeDir.y, wakeDir.x); // Perpendicular
        p.position = pos + wakeDir * (2.0 + i * 0.5) + lateral * offset;
        
        // Velocidad en dirección de la estela
        p.velocity = wakeDir * (boatSpeed * 0.3) + lateral * (offset * 0.1);
        
        p.life = 1.0;
        p.maxLife = lifeDist(rng_);
        p.type = 2; // estela
        p.size = sizeDist(rng_);
        
        // Color más blanco en el centro, más azul en los bordes
        int whiteness = 230 + static_cast<int>(std::abs(offset) * 5);
        if (whiteness > 255) whiteness = 255;
        p.color = wxColour(whiteness, whiteness, 255);
        
        particles_.push_back(p);
    }
}

void WaterParticleSystem::emitSplash(const Vec2d& pos, const Vec2d& velocity, int count) {
    std::uniform_real_distribution<double> angleDist(-M_PI, M_PI);
    std::uniform_real_distribution<double> speedDist(1.0, 4.0);
    std::uniform_real_distribution<double> lifeDist(0.5, 1.5);
    std::uniform_real_distribution<double> sizeDist(1.5, 3.0);
    
    for (int i = 0; i < count && particles_.size() < static_cast<size_t>(maxParticles_); ++i) {
        WaterParticle p;
        p.position = pos;
        
        double angle = angleDist(rng_);
        double speed = speedDist(rng_);
        p.velocity = Vec2d(std::cos(angle) * speed, std::sin(angle) * speed);
        p.velocity = p.velocity + velocity;
        
        p.life = 1.0;
        p.maxLife = lifeDist(rng_);
        p.type = 0; // agua (splash)
        p.size = sizeDist(rng_);
        p.color = wxColour(200, 220, 255); // Azul claro
        
        particles_.push_back(p);
    }
}

void WaterParticleSystem::applyFieldEffect(const Vec2d& pos, const Vec2d& fieldVector) {
    // Emitir partículas donde el campo es fuerte (turbulencia)
    double fieldStrength = fieldVector.magnitude();
    if (fieldStrength > 3.0 && particles_.size() < static_cast<size_t>(maxParticles_)) {
        std::uniform_real_distribution<double> offsetDist(-3.0, 3.0);
        
        // Probabilidad de emitir espuma basada en la fuerza del campo
        if (static_cast<int>(fieldStrength * 10) % 10 == 0) {
            Vec2d emitPos = pos + Vec2d(offsetDist(rng_), offsetDist(rng_));
            emitFoam(emitPos, fieldStrength * 0.1, 1);
        }
    }
}

void WaterParticleSystem::clear() {
    particles_.clear();
}

}
