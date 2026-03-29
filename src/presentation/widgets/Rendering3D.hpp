#pragma once

/**
 * @file Rendering3D.hpp
 * @brief 3D rendering context with OpenGL 3.3+
 * @author Proyecto Taller Paramétrico
 * @version 0.3.0
 * 
 * 3D rendering context implementing IRenderingContext using OpenGL 3.3+
 * with animated water surface using height-field simulation.
 */

#include "IRenderingContext.hpp"
#include <memory>

// Forward declarations
class wxWindow;
class wxGLCanvas;
class wxGLContext;
class wxTimer;

namespace tp::presentation {

// Forward declarations for internal 3D implementation
class WaterSurface;
class WaterRenderer;
class Camera;
class LightConfig;

/**
 * @brief 3D rendering context with animated water
 * 
 * Uses OpenGL 3.3+ to render a stylized 3D water surface with:
 * - Animated waves using height-field equation
 * - Directional lighting (Blinn-Phong)
 * - Overscan mesh with fragment clipping
 * - Flat 2D boat rendering on top
 */
class Rendering3DContext : public IRenderingContext {
public:
    explicit Rendering3DContext(wxWindow* parent);
    ~Rendering3DContext() override;
    
    // IRenderingContext implementation
    void initialize() override;
    void shutdown() override;
    
    void resize(int width, int height) override;
    void render(double deltaTime) override;
    void refresh() override;
    
    void setScenario(const domain::Scenario* scenario) override;
    void setVectorField(const domain::VectorField* field) override;
    void setBoat(const domain::Boat* boat) override;
    void setBoatPosition(double x, double y) override;
    
    void setShowGrid(bool show) override;
    void setShowField(bool show) override;
    void setShowTrajectory(bool show) override;
    void setShowWaterFlow(bool show) override;
    
    void startAnimation() override;
    void stopAnimation() override;
    void pauseAnimation() override;
    void resumeAnimation() override;
    bool isAnimating() const override;
    
    void setTrajectory(const std::vector<TrajectoryPoint>& trajectory) override;
    void clearTrajectory() override;
    
    RenderingMode getMode() const override { return RenderingMode::Mode3D; }

private:
    wxWindow* parent_;
    
    // 3D rendering components
    std::unique_ptr<WaterSurface> waterSurface_;
    std::unique_ptr<WaterRenderer> renderer_;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<LightConfig> light_;
    
    // State
    bool isInitialized_;
    bool isAnimating_;
    bool isPaused_;
    double simulationTime_;
    double lastFrameTime_;
    
    // Scene state
    const domain::Scenario* scenario_;
    const domain::VectorField* vectorField_;
    const domain::Boat* boat_;
    double boatX_, boatY_;
    
    // Visibility flags
    bool showGrid_, showField_, showTrajectory_, showWaterFlow_;
};

} // namespace tp::presentation
