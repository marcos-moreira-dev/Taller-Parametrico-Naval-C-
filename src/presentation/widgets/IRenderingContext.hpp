#pragma once

/**
 * @file IRenderingContext.hpp
 * @brief Abstract rendering interface for polymorphic canvas rendering
 * @author Proyecto Taller Paramétrico
 * @version 0.3.0
 * 
 * Provides a Java-style interface for switching between 2D and 3D rendering modes.
 * Allows runtime switching between flat 2D rendering and 3D water simulation.
 */

#include <memory>
#include <vector>

// wxWidgets forward declarations
class wxWindow;
class wxTimer;

namespace tp::domain {
class Scenario;
class VectorField;
class Boat;
}

namespace tp::presentation {

// Forward declarations
namespace domain {
class Scenario;
class VectorField;
class Boat;
}

enum class RenderingMode {
    Mode2D,
    Mode3D
};

/**
 * @brief Abstract rendering context interface
 * 
 * Java-style interface (pure virtual class) for polymorphic rendering.
 * All rendering implementations must inherit from this and implement
 * all virtual methods.
 * 
 * Thread model: All methods must be called from the main wxWidgets thread.
 */
class IRenderingContext {
public:
    virtual ~IRenderingContext() = default;
    
    // ============================================================
    // LIFECYCLE
    // ============================================================
    
    /**
     * @brief Initialize the rendering context
     * Called after the widget is created and has a valid HWND
     */
    virtual void initialize() = 0;
    
    /**
     * @brief Clean up rendering resources
     * Called before the widget is destroyed
     */
    virtual void shutdown() = 0;
    
    // ============================================================
    // RESIZING
    // ============================================================
    
    /**
     * @brief Handle window resize
     * @param width New width in pixels
     * @param height New height in pixels
     */
    virtual void resize(int width, int height) = 0;
    
    // ============================================================
    // RENDERING
    // ============================================================
    
    /**
     * @brief Main render call
     * @param deltaTime Time since last frame in seconds
     * 
     * Called from paint handler or animation timer
     */
    virtual void render(double deltaTime) = 0;
    
    /**
     * @brief Request a redraw
     */
    virtual void refresh() = 0;
    
    // ============================================================
    // STATE MANAGEMENT
    // ============================================================
    
    /**
     * @brief Set the scenario to render
     * @param scenario Pointer to scenario (not owned)
     */
    virtual void setScenario(const domain::Scenario* scenario) = 0;
    
    /**
     * @brief Set the vector field
     * @param field Pointer to vector field (not owned)
     */
    virtual void setVectorField(const domain::VectorField* field) = 0;
    
    /**
     * @brief Set the boat to render
     * @param boat Pointer to boat (not owned)
     */
    virtual void setBoat(const domain::Boat* boat) = 0;
    
    /**
     * @brief Update boat position from simulation
     * @param x World X coordinate
     * @param y World Y coordinate
     */
    virtual void setBoatPosition(double x, double y) = 0;
    
    // ============================================================
    // VISIBILITY TOGGLES
    // ============================================================
    
    virtual void setShowGrid(bool show) = 0;
    virtual void setShowField(bool show) = 0;
    virtual void setShowTrajectory(bool show) = 0;
    virtual void setShowWaterFlow(bool show) = 0;
    
    // ============================================================
    // ANIMATION CONTROL
    // ============================================================
    
    virtual void startAnimation() = 0;
    virtual void stopAnimation() = 0;
    virtual void pauseAnimation() = 0;
    virtual void resumeAnimation() = 0;
    virtual bool isAnimating() const = 0;
    
    // ============================================================
    // TRAJECTORY
    // ============================================================
    
    struct TrajectoryPoint {
        double x, y, time;
    };
    
    virtual void setTrajectory(const std::vector<TrajectoryPoint>& trajectory) = 0;
    virtual void clearTrajectory() = 0;
    
    // ============================================================
    // MODE QUERY
    // ============================================================
    
    virtual RenderingMode getMode() const = 0;
};

/**
 * @brief Factory for creating rendering contexts
 * 
 * Creates the appropriate renderer based on requested mode.
 */
class RenderingFactory {
public:
    /**
     * @brief Create a 2D rendering context
     * @param parent wxWidgets parent window
     * @return Unique pointer to rendering context
     */
    static std::unique_ptr<IRenderingContext> create2D(wxWindow* parent);
    
    /**
     * @brief Create a 3D rendering context  
     * @param parent wxWidgets parent window
     * @return Unique pointer to rendering context
     */
    static std::unique_ptr<IRenderingContext> create3D(wxWindow* parent);
};

} // namespace tp::presentation
