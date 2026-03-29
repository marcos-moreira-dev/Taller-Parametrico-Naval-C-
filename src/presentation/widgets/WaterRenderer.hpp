#pragma once

/**
 * @file WaterRenderer.hpp
 * @brief OpenGL water surface renderer with shaders
 * @author Proyecto Taller Paramétrico
 * @version 0.3.0
 * 
 * Renders animated water surface using OpenGL 3.3+ with:
 * - Vertex shader for wave displacement
 * - Fragment shader with Blinn-Phong lighting
 * - Overscan clipping for clean borders
 */

#include <memory>
#include <array>
#include <vector>

namespace tp::presentation {

class WaterSurface;
class Camera;
class LightConfig;

/**
 * @brief OpenGL water surface renderer
 * 
 * Manages OpenGL resources for rendering the water mesh:
 * - Vertex Array Object (VAO)
 * - Vertex Buffer Object (VBO)
 * - Element Buffer Object (EBO)
 * - Shader program
 */
class WaterRenderer {
public:
    WaterRenderer();
    ~WaterRenderer();
    
    /**
     * @brief Initialize OpenGL resources
     * @param surface Water surface to render
     * @return true if initialization succeeded
     */
    bool initialize(const WaterSurface* surface);
    
    /**
     * @brief Clean up OpenGL resources
     */
    void shutdown();
    
    /**
     * @brief Render the water surface
     * @param surface Water surface with current state
     * @param camera Camera for view/projection matrices
     * @param light Light configuration
     * @param time Current simulation time
     */
    void render(const WaterSurface* surface, const Camera* camera, 
                const LightConfig* light, double time);
    
    /**
     * @brief Check if renderer is initialized
     */
    bool isInitialized() const { return isInitialized_; }
    
    /**
     * @brief Update mesh data from surface
     */
    void updateMesh(const WaterSurface* surface);

private:
    bool isInitialized_;
    unsigned int vao_;
    unsigned int vbo_;
    unsigned int ebo_;
    unsigned int shaderProgram_;
    int vertexCount_;
    int indexCount_;
    
    static const char* vertexShaderSource();
    static const char* fragmentShaderSource();
    
    bool compileShaders();
    bool createBuffers(const WaterSurface* surface);
};

/**
 * @brief Directional light configuration
 * 
 * Implements Blinn-Phong directional lighting for water rendering.
 */
class LightConfig {
public:
    LightConfig();
    
    void setDirection(float x, float y, float z);
    void setColor(float r, float g, float b);
    void setAmbient(float ambient);
    void setDiffuse(float diffuse);
    void setSpecular(float specular, float shininess);
    
    std::array<float, 3> getDirection() const { return {dirX_, dirY_, dirZ_}; }
    std::array<float, 3> getColor() const { return {colorR_, colorG_, colorB_}; }
    float getAmbient() const { return ambient_; }
    float getDiffuse() const { return diffuse_; }
    float getSpecular() const { return specular_; }
    float getShininess() const { return shininess_; }
    
private:
    float dirX_, dirY_, dirZ_;
    float colorR_, colorG_, colorB_;
    float ambient_;
    float diffuse_;
    float specular_;
    float shininess_;
};

} // namespace tp::presentation
