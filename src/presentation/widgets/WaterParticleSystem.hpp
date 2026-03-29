#pragma once

// Definir ssize_t si no está definido
#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include <wx/wx.h>
#include <vector>
#include <random>
#include "Vec2.hpp"

namespace tp::presentation {

using namespace tp::shared;

// Partícula de agua/espuma
struct WaterParticle {
    Vec2d position;
    Vec2d velocity;
    double life;           // 0.0 a 1.0
    double maxLife;
    int type;              // 0=agua, 1=espuma, 2=estela
    double size;
    wxColour color;
};

// Sistema de partículas para efectos de agua
class WaterParticleSystem {
public:
    WaterParticleSystem(int maxParticles = 1000);
    
    void update(double dt);
    void render(wxDC& dc, double scale, const wxRect& viewport);
    
    // Emitir partículas
    void emitWater(const Vec2d& pos, const Vec2d& velocity, int count = 1);
    void emitFoam(const Vec2d& pos, double intensity, int count = 1);
    void emitWake(const Vec2d& pos, const Vec2d& boatVelocity, double boatSpeed);
    void emitSplash(const Vec2d& pos, const Vec2d& velocity, int count = 5);
    
    // Efectos de campo
    void applyFieldEffect(const Vec2d& pos, const Vec2d& fieldVector);
    
    void clear();
    int getParticleCount() const { return static_cast<int>(particles_.size()); }
    
private:
    std::vector<WaterParticle> particles_;
    int maxParticles_;
    std::mt19937 rng_;
    
    void spawnParticle(const Vec2d& pos, const Vec2d& vel, int type);
    wxColour getParticleColor(const WaterParticle& p) const;
};

}
