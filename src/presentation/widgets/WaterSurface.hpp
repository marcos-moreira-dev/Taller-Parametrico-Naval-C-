#pragma once

/**
 * @file WaterSurface.hpp
 * @brief Water surface mesh and height-field simulation
 * @author Proyecto Taller Paramétrico
 * @version 0.3.0
 * 
 * Implements a 3D water surface using height-field approach:
 * - Regular grid mesh with overscan border
 * - Time-varying wave height function
 * - Normal computation for lighting
 * - Vector field influence on wave parameters
 * 
 * Mathematical model:
 * h(x,z,t) = Σ Aᵢ * sin(kᵢ(dᵢ·p) - ωᵢt + φᵢ)
 * where p = (x, z) in world coordinates
 */

#include <vector>
#include <array>

namespace tp::domain {
class VectorField;
class Scenario;
}

namespace tp::presentation {

/**
 * @brief Single wave component
 */
struct WaveComponent {
    float amplitude;   // Aᵢ - wave height
    float waveNumber;  // kᵢ - spatial frequency  
    float frequency;   // ωᵢ - temporal frequency
    float phase;       // φᵢ - phase offset
    float dirX;       // dᵢx - wave direction X (normalized)
    float dirZ;       // dᵢz - wave direction Z (normalized)
    
    WaveComponent(float A = 0.1f, float k = 2.0f, float w = 1.0f, 
                  float p = 0.0f, float dx = 1.0f, float dz = 0.0f)
        : amplitude(A), waveNumber(k), frequency(w), phase(p), dirX(dx), dirZ(dz) {}
};

/**
 * @brief Vertex in the water mesh
 */
struct WaterVertex {
    float x, y, z;     // Position (y = height, computed in shader)
    float u, v;         // Texture coordinates
    float nx, ny, nz;  // Normal (computed in shader)
};

/**
 * @brief Water surface domain model
 * 
 * Generates and manages the water mesh grid with animated height.
 * This is the CPU-side state - rendering is handled by WaterRenderer.
 */
class WaterSurface {
public:
    /**
     * @brief Construct water surface
     * @param visibleWidth Width of visible water area in world units
     * @param visibleHeight Height of visible water area in world units
     * @param meshResolution Number of vertices per unit (e.g., 10 = 10 vertices per meter)
     * @param overscan Percentage of overscan (0.1 = 10% extra)
     */
    WaterSurface(float visibleWidth = 50.0f, float visibleHeight = 50.0f,
                 int meshResolution = 20, float overscan = 0.1f);
    
    ~WaterSurface() = default;
    
    // ============================================================
    // CONFIGURATION
    // ============================================================
    
    /**
     * @brief Set wave components
     * @param waves Vector of wave components
     */
    void setWaves(const std::vector<WaveComponent>& waves);
    
    /**
     * @brief Add a wave component
     */
    void addWave(const WaveComponent& wave);
    
    /**
     * @brief Clear all waves
     */
    void clearWaves();
    
    // ============================================================
    // VECTOR FIELD COUPLING (Phase 2)
    // ============================================================
    
    /**
     * @brief Set vector field for visual coupling
     * @param field Pointer to vector field (not owned)
     */
    void setVectorField(const domain::VectorField* field);
    
    /**
     * @brief Set coupling parameters
     * @param amplitudeAlpha α - how much field magnitude affects amplitude
     * @param phaseBeta β - how much field direction affects phase
     */
    void setCouplingParameters(float amplitudeAlpha, float phaseBeta);
    
    // ============================================================
    // MESH DATA
    // ============================================================
    
    /**
     * @brief Get total vertex count
     */
    int getVertexCount() const { return static_cast<int>(vertices_.size()); }
    
    /**
     * @brief Get index count for drawing
     */
    int getIndexCount() const { return static_cast<int>(indices_.size()); }
    
    /**
     * @brief Get raw vertex data
     */
    const float* getVertexData() const { return vertexData_.data(); }
    
    /**
     * @brief Get raw index data
     */
    const unsigned int* getIndexData() const { return indexData_.data(); }
    
    // ============================================================
    // VISIBLE REGION (for clipping)
    // ============================================================
    
    void setVisibleRect(float minX, float minZ, float maxX, float maxZ);
    
    float getVisibleMinX() const { return visibleMinX_; }
    float getVisibleMinZ() const { return visibleMinZ_; }
    float getVisibleMaxX() const { return visibleMaxX_; }
    float getVisibleMaxZ() const { return visibleMaxZ_; }
    
    // ============================================================
    // ANIMATION
    // ============================================================
    
    /**
     * @brief Update simulation time
     * @param time Current simulation time
     */
    void setTime(double time) { simulationTime_ = static_cast<float>(time); }
    
    float getTime() const { return simulationTime_; }
    
    // ============================================================
    // HEIGHT SAMPLING (for boat positioning)
    // ============================================================
    
    /**
     * @brief Sample water height at world position
     * @param worldX World X coordinate
     * @param worldZ World Z coordinate
     * @return Water height at position
     */
    float sampleHeight(float worldX, float worldZ) const;

private:
    // Mesh configuration
    float visibleWidth_;
    float visibleHeight_;
    float overscanWidth_;
    float overscanHeight_;
    int meshResolution_;
    
    // Mesh data
    std::vector<WaterVertex> vertices_;
    std::vector<unsigned int> indices_;
    std::vector<float> vertexData_;  // Interleaved for OpenGL
    std::vector<unsigned int> indexData_;
    
    // Mesh dimensions
    int gridWidth_;
    int gridHeight_;
    float cellSize_;
    
    // Wave components
    std::vector<WaveComponent> waves_;
    
    // Vector field coupling
    const domain::VectorField* vectorField_;
    float couplingAlpha_;  // Amplitude modulation
    float couplingBeta_;  // Phase modulation
    
    // Animation
    float simulationTime_;
    
    // Visible region for clipping
    float visibleMinX_, visibleMinZ_;
    float visibleMaxX_, visibleMaxZ_;
    
    // Generate the mesh grid
    void generateMesh();
    
    // Update vertex data for OpenGL
    void updateVertexData();
    
    // Evaluate height function at position
    float evaluateHeight(float x, float z) const;
};

} // namespace tp::presentation
