// WaterAnimationSystem.cpp - Implementación del sistema de animación de agua

#include "WaterAnimationSystem.hpp"
#include <cmath>
#include <random>
#include <chrono>

namespace tp::presentation {

WaterAnimationSystem::WaterAnimationSystem()
    : isRunning_(false)
    , isPaused_(false)
    , scenario_(nullptr)
    , field_(nullptr)
    , maxParticles_(500)
    , density_(100)
    , intensity_(0.8)
    , time_(0.0)
{
}

WaterAnimationSystem::~WaterAnimationSystem() {
    stop();
}

void WaterAnimationSystem::start(domain::Scenario* scenario, const domain::VectorField* field) {
    if (isRunning_) return;
    
    scenario_ = scenario;
    field_ = field;
    
    isRunning_ = true;
    isPaused_ = false;
    particles_.clear();
    
    // Spawn initial particles
    spawnParticles();
    
    // Start animation thread
    animationThread_ = std::thread([this]() {
        animationLoop();
    });
}

void WaterAnimationSystem::stop() {
    isRunning_ = false;
    if (animationThread_.joinable()) {
        animationThread_.join();
    }
    particles_.clear();
}

void WaterAnimationSystem::animationLoop() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (isRunning_) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double dt = std::chrono::duration<double>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        if (!isPaused_) {
            // Update time for animations
            time_ += dt;
            
            // Spawn new particles if needed
            spawnParticles();
            
            // Update existing particles
            updateParticles(dt);
        }
        
        // Sleep to maintain ~60 FPS
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void WaterAnimationSystem::spawnParticles() {
    if (!scenario_) return;
    
    std::lock_guard<std::mutex> lock(particleMutex_);
    
    int targetCount = density_;
    int currentCount = (int)particles_.size();
    
    if (currentCount >= targetCount) return;
    
    int toSpawn = std::min(10, targetCount - currentCount);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distX(0, scenario_->getWidth());
    std::uniform_real_distribution<> distY(0, scenario_->getHeight());
    
    for (int i = 0; i < toSpawn; ++i) {
        double x = distX(gen);
        double y = distY(gen);
        
        // Only spawn in water cells
        if (scenario_->getCell(static_cast<int>(x), static_cast<int>(y)) == CellType::Water) {
            particles_.push_back(createParticle());
            particles_.back().position = Vec2d(x, y);
        }
    }
}

FlowParticle WaterAnimationSystem::createParticle() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distLife(1.0, 3.0);
    std::uniform_real_distribution<> distSize(1.5, 4.0);
    
    FlowParticle p;
    p.velocity = Vec2d(0, 0);
    p.alpha = distLife(gen);
    p.size = distSize(gen);
    return p;
}

void WaterAnimationSystem::updateParticles(double dt) {
    if (!scenario_ || !field_) return;
    
    std::lock_guard<std::mutex> lock(particleMutex_);
    std::lock_guard<std::mutex> fieldLock(fieldMutex_);
    
    double fieldScale = intensity_ * PARTICLE_SPEED * 50.0;
    
    for (auto it = particles_.begin(); it != particles_.end();) {
        // Get field vector at particle position
        Vec2d fieldVec = field_->getValue(it->position.x, it->position.y);
        
        // Apply field to velocity with more smoothing for water effect
        it->velocity = it->velocity * 0.9 + fieldVec * fieldScale * 0.1;
        
        // Clamp velocity to prevent particles from flying too fast
        double speed = std::sqrt(it->velocity.x * it->velocity.x + it->velocity.y * it->velocity.y);
        if (speed > 10.0) {
            it->velocity = it->velocity * (10.0 / speed);
        }
        
        // Update position
        it->position = it->position + it->velocity * dt;
        
        // Check bounds and cell type
        int cx = static_cast<int>(it->position.x);
        int cy = static_cast<int>(it->position.y);
        
        bool outOfBounds = cx < 0 || cx >= scenario_->getWidth() || 
                          cy < 0 || cy >= scenario_->getHeight();
        
        bool notWater = outOfBounds || 
                       (scenario_->getCell(cx, cy) != CellType::Water);
        
        // Decay alpha - slower for water effect
        it->alpha -= 0.15 * dt;
        
        // Remove dead or out-of-bounds particles
        if (it->alpha <= 0.0 || notWater) {
            it = particles_.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace tp::presentation