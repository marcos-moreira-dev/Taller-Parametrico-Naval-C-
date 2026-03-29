// WaterSurface.cpp - Implementation of water height-field surface

#include "WaterSurface.hpp"
#include "VectorField.hpp"
#include <cmath>
#include <algorithm>

namespace tp::presentation {

WaterSurface::WaterSurface(float visibleWidth, float visibleHeight, 
                           int meshResolution, float overscan)
    : visibleWidth_(visibleWidth)
    , visibleHeight_(visibleHeight)
    , meshResolution_(meshResolution)
    , vectorField_(nullptr)
    , couplingAlpha_(0.0f)
    , couplingBeta_(0.0f)
    , simulationTime_(0.0f)
{
    // Calculate overscan dimensions
    overscanWidth_ = visibleWidth * overscan;
    overscanHeight_ = visibleHeight * overscan;
    
    // Calculate visible region (centered)
    visibleMinX_ = -overscanWidth_ / 2;
    visibleMinZ_ = -overscanHeight_ / 2;
    visibleMaxX_ = visibleWidth_ + overscanWidth_ / 2;
    visibleMaxZ_ = visibleHeight_ + overscanHeight_ / 2;
    
    // Calculate grid dimensions
    gridWidth_ = static_cast<int>((visibleWidth_ + overscanWidth_) * meshResolution);
    gridHeight_ = static_cast<int>((visibleHeight_ + overscanHeight_) * meshResolution);
    cellSize_ = 1.0f / meshResolution_;
    
    // Generate initial mesh
    generateMesh();
    
    // Add default waves
    addWave(WaveComponent(0.15f, 1.5f, 2.0f, 0.0f, 1.0f, 0.0f));   // Main wave
    addWave(WaveComponent(0.08f, 3.0f, 3.5f, 1.57f, 0.7f, 0.7f));  // Cross wave
    addWave(WaveComponent(0.05f, 5.0f, 5.0f, 0.5f, 0.0f, 1.0f));   // Diagonal wave
}

void WaterSurface::setWaves(const std::vector<WaveComponent>& waves) {
    waves_ = waves;
    updateVertexData();
}

void WaterSurface::addWave(const WaveComponent& wave) {
    waves_.push_back(wave);
    updateVertexData();
}

void WaterSurface::clearWaves() {
    waves_.clear();
    updateVertexData();
}

void WaterSurface::setVectorField(const domain::VectorField* field) {
    vectorField_ = field;
}

void WaterSurface::setCouplingParameters(float alpha, float beta) {
    couplingAlpha_ = alpha;
    couplingBeta_ = beta;
}

void WaterSurface::setVisibleRect(float minX, float minZ, float maxX, float maxZ) {
    visibleMinX_ = minX;
    visibleMinZ_ = minZ;
    visibleMaxX_ = maxX;
    visibleMaxZ_ = maxZ;
}

void WaterSurface::generateMesh() {
    vertices_.clear();
    indices_.clear();
    
    float meshWidth = visibleWidth_ + overscanWidth_;
    float meshHeight = visibleHeight_ + overscanHeight_;
    float startX = -overscanWidth_ / 2;
    float startZ = -overscanHeight_ / 2;
    
    // Generate vertices
    for (int j = 0; j <= gridHeight_; ++j) {
        for (int i = 0; i <= gridWidth_; ++i) {
            WaterVertex v;
            
            // Position (y will be animated)
            v.x = startX + i * cellSize_;
            v.y = 0.0f;  // Will be displaced in shader
            v.z = startZ + j * cellSize_;
            
            // Texture coordinates
            v.u = static_cast<float>(i) / gridWidth_;
            v.v = static_cast<float>(j) / gridHeight_;
            
            // Initial normal (up)
            v.nx = 0.0f;
            v.ny = 1.0f;
            v.nz = 0.0f;
            
            vertices_.push_back(v);
        }
    }
    
    // Generate indices (two triangles per quad)
    for (int j = 0; j < gridHeight_; ++j) {
        for (int i = 0; i < gridWidth_; ++i) {
            unsigned int topLeft = j * (gridWidth_ + 1) + i;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (j + 1) * (gridWidth_ + 1) + i;
            unsigned int bottomRight = bottomLeft + 1;
            
            // First triangle
            indices_.push_back(topLeft);
            indices_.push_back(bottomLeft);
            indices_.push_back(topRight);
            
            // Second triangle
            indices_.push_back(topRight);
            indices_.push_back(bottomLeft);
            indices_.push_back(bottomRight);
        }
    }
    
    // Update interleaved data
    updateVertexData();
}

void WaterSurface::updateVertexData() {
    vertexData_.clear();
    
    for (const auto& v : vertices_) {
        vertexData_.push_back(v.x);
        vertexData_.push_back(v.y);
        vertexData_.push_back(v.z);
        vertexData_.push_back(v.u);
        vertexData_.push_back(v.v);
        vertexData_.push_back(v.nx);
        vertexData_.push_back(v.ny);
        vertexData_.push_back(v.nz);
    }
    
    indexData_ = indices_;
}

float WaterSurface::evaluateHeight(float x, float z) const {
    if (waves_.empty()) return 0.0f;
    
    float height = 0.0f;
    
    // Sample vector field influence if available
    float fieldMagnitude = 0.0f;
    float fieldDirX = 0.0f;
    float fieldDirZ = 0.0f;
    
    if (vectorField_ != nullptr && (couplingAlpha_ > 0.0f || couplingBeta_ > 0.0f)) {
        auto field = vectorField_->getValue(static_cast<double>(x), static_cast<double>(z));
        fieldMagnitude = static_cast<float>(std::sqrt(field.x * field.x + field.y * field.y));
        if (fieldMagnitude > 0.001f) {
            fieldDirX = static_cast<float>(field.x) / fieldMagnitude;
            fieldDirZ = static_cast<float>(field.y) / fieldMagnitude;
        }
    }
    
    // Sum wave components
    for (const auto& wave : waves_) {
        // Phase with wave direction
        float phase = wave.waveNumber * (wave.dirX * x + wave.dirZ * z) 
                    - wave.frequency * simulationTime_ 
                    + wave.phase;
        
        // Vector field modulation
        if (fieldMagnitude > 0.001f) {
            float fieldPhaseMod = wave.waveNumber * (fieldDirX * x + fieldDirZ * z) * couplingBeta_;
            phase += fieldPhaseMod * fieldMagnitude;
        }
        
        float amplitudeMod = 1.0f + couplingAlpha_ * fieldMagnitude;
        
        height += wave.amplitude * amplitudeMod * std::sin(phase);
    }
    
    return height;
}

float WaterSurface::sampleHeight(float worldX, float worldZ) const {
    return evaluateHeight(worldX, worldZ);
}

} // namespace tp::presentation
