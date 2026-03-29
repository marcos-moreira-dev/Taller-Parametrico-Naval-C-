#pragma once

/**
 * @file Rendering2D.hpp
 * @brief 2D rendering context implementation
 * @author Proyecto Taller Paramétrico
 * @version 0.3.0
 * 
 * Wraps the existing 2D canvas rendering in the IRenderingContext interface.
 */

#include "IRenderingContext.hpp"
#include <wx/wx.h>
#include <vector>
#include <memory>

namespace tp::presentation {

// Forward declaration to avoid circular dependency
class SimulationCanvas2D;

/**
 * @brief 2D rendering context implementation
 * 
 * Wraps SimulationCanvas2D (to be created) to implement IRenderingContext.
 * Uses the existing 2D rendering logic with flat colors and shapes.
 */
class Rendering2DContext : public IRenderingContext {
public:
    explicit Rendering2DContext(wxWindow* parent);
    ~Rendering2DContext() override;
    
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
    
    RenderingMode getMode() const override { return RenderingMode::Mode2D; }

private:
    wxWindow* parent_;
    wxWindow* canvasWindow_;
    void* canvasImpl_;  // Actual canvas implementation (wxPanel or similar)
};

} // namespace tp::presentation
