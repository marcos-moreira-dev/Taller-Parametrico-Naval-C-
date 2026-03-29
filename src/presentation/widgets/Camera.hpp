#pragma once

/**
 * @file Camera.hpp
 * @brief Camera with view and projection matrices
 * @author Proyecto Taller Paramétrico
 * @version 0.3.0
 * 
 * Provides a simple perspective camera for 3D water rendering.
 * Uses a tilted top-down view to show water depth while
 * maintaining the simulation's mostly top-down feel.
 */

#include <array>

namespace tp::presentation {

/**
 * @brief 3D Camera for water rendering
 * 
 * Implements a simple perspective camera with:
 * - Position (eye point)
 * - Target (look-at point)
 * - Up vector
 * - Field of view
 * - Near/far planes
 */
class Camera {
public:
    /**
     * @brief Construct camera with default top-down tilted view
     * @param aspectRatio Width/height of viewport
     */
    explicit Camera(float aspectRatio = 16.0f / 9.0f);
    
    // ============================================================
    // MATRIX ACCESS
    // ============================================================
    
    /**
     * @brief Get view matrix
     * @return 4x4 view matrix (column-major for OpenGL)
     */
    std::array<float, 16> getViewMatrix() const;
    
    /**
     * @brief Get projection matrix
     * @return 4x4 projection matrix (column-major for OpenGL)
     */
    std::array<float, 16> getProjectionMatrix() const;
    
    // ============================================================
    // VIEW CONTROL
    // ============================================================
    
    /**
     * @brief Set camera position
     * @param x, y, z Position in world coordinates
     */
    void setPosition(float x, float y, float z);
    
    /**
     * @brief Set look-at target
     * @param x, y, z Target position
     */
    void setTarget(float x, float y, float z);
    
    /**
     * @brief Set camera up vector
     * @param x, y, z Up direction (typically 0, 1, 0)
     */
    void setUp(float x, float y, float z);
    
    /**
     * @brief Set field of view (degrees)
     */
    void setFOV(float fovDegrees);
    
    /**
     * @brief Set aspect ratio
     */
    void setAspectRatio(float aspect);
    
    /**
     * @brief Set near/far clipping planes
     */
    void setClipping(float nearPlane, float farPlane);
    
    /**
     * @brief Get camera position
     */
    std::array<float, 3> getPosition() const { return {posX_, posY_, posZ_}; }
    
    // ============================================================
    // PRESETS
    // ============================================================
    
    /**
     * @brief Set to default tilted top-down view
     * Suitable for water simulation
     */
    void setTiltedTopDownView();
    
    /**
     * @brief Set to straight top-down (orthographic-like)
     */
    void setTopDownView();
    
    /**
     * @brief Set to angled perspective view
     */
    void setAngledView(float angleDegrees);

private:
    // Camera parameters
    float posX_, posY_, posZ_;      // Eye position
    float targetX_, targetY_, targetZ_; // Look-at target
    float upX_, upY_, upZ_;         // Up vector
    
    // Projection parameters
    float fov_;           // Field of view in degrees
    float aspect_;        // Aspect ratio
    float nearClip_;      // Near clipping plane
    float farClip_;       // Far clipping plane
    
    // Compute matrices
    void computeViewMatrix();
    void computeProjectionMatrix();
    
    std::array<float, 16> viewMatrix_;
    std::array<float, 16> projMatrix_;
    bool matricesDirty_;
};

} // namespace tp::presentation
