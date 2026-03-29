#pragma once

/**
 * @file WaterAnimationSystem.hpp
 * @brief Sistema de animación de agua en tiempo real
 * @author Proyecto Taller Paramétrico
 * @version 0.2.4
 * 
 * Simula el movimiento del agua basado en el campo vectorial:
 * - Hilo separado para animación fluida
 * - Partículas que fluyen según el campo vectorial
 * - Renderizado con efecto de corriente
 */

#include <thread>
#include <atomic>
#include <vector>
#include <mutex>
#include "Vec2.hpp"
#include "Scenario.hpp"
#include "VectorField.hpp"

namespace tp::presentation {

using namespace tp::shared;

/**
 * @brief Partícula de agua animada para el flujo
 */
struct FlowParticle {
    Vec2d position;
    Vec2d velocity;
    double alpha;  // Transparencia
    double size;
};

/**
 * @brief Sistema de animación de agua con hilo separado
 */
class WaterAnimationSystem {
public:
    WaterAnimationSystem();
    ~WaterAnimationSystem();
    
    /**
     * @brief Inicia la animación
     * @param scenario Escenario con las células de agua
     * @param field Campo vectorial para el flujo
     */
    void start(domain::Scenario* scenario, const domain::VectorField* field);
    
    /**
     * @brief Detiene la animación
     */
    void stop();
    
    /**
     * @brief Pausa la animación sin destruir
     */
    void pause() { isPaused_ = true; }
    
    /**
     * @brief Reanuda la animación
     */
    void resume() { isPaused_ = false; }
    
    /**
     * @brief Actualiza el campo vectorial
     */
    void updateField(const domain::VectorField* field) {
        std::lock_guard<std::mutex> lock(fieldMutex_);
        field_ = field;
    }
    
    /**
     * @brief Verifica si está corriendo
     */
    bool isRunning() const { return isRunning_; }
    
    /**
     * @brief Obtiene las partículas para renderizar
     */
    const std::vector<FlowParticle>& getParticles() const { return particles_; }
    
    /**
     * @brief Intensidad del flujo (0-1)
     */
    void setIntensity(double intensity) { intensity_ = intensity; }
    double getIntensity() const { return intensity_; }
    
    /**
     * @brief Densidad de partículas
     */
    void setDensity(int density) { density_ = density; }
    int getDensity() const { return density_; }

private:
    void animationLoop();
    void spawnParticles();
    void updateParticles(double dt);
    FlowParticle createParticle() const;
    
    std::atomic<bool> isRunning_;
    std::atomic<bool> isPaused_;
    std::thread animationThread_;
    mutable std::mutex particleMutex_;
    mutable std::mutex fieldMutex_;
    
    domain::Scenario* scenario_;
    const domain::VectorField* field_;
    
    std::vector<FlowParticle> particles_;
    int maxParticles_;
    int density_;
    double intensity_;
    double time_;
    
    static constexpr double PARTICLE_SPEED = 0.5;
    static constexpr double ALPHA_DECAY = 0.3;
};

} // namespace tp::presentation