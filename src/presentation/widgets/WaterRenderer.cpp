#include "WaterRenderer.hpp"
#include "WaterSurface.hpp"
#include "Camera.hpp"
#include <cmath>

namespace tp::presentation {

LightConfig::LightConfig()
    : dirX_(0.5f), dirY_(1.0f), dirZ_(0.3f),
      colorR_(1.0f), colorG_(1.0f), colorB_(1.0f),
      ambient_(0.3f), diffuse_(0.7f), specular_(0.5f), shininess_(32.0f) {
    float len = std::sqrt(dirX_ * dirX_ + dirY_ * dirY_ + dirZ_ * dirZ_);
    if (len > 0.0f) {
        dirX_ /= len; dirY_ /= len; dirZ_ /= len;
    }
}

void LightConfig::setDirection(float x, float y, float z) {
    dirX_ = x; dirY_ = y; dirZ_ = z;
    float len = std::sqrt(dirX_ * dirX_ + dirY_ * dirY_ + dirZ_ * dirZ_);
    if (len > 0.0f) {
        dirX_ /= len; dirY_ /= len; dirZ_ /= len;
    }
}

void LightConfig::setColor(float r, float g, float b) {
    colorR_ = r; colorG_ = g; colorB_ = b;
}

void LightConfig::setAmbient(float ambient) { ambient_ = ambient; }
void LightConfig::setDiffuse(float diffuse) { diffuse_ = diffuse; }
void LightConfig::setSpecular(float specular, float shininess) {
    specular_ = specular;
    shininess_ = shininess;
}

WaterRenderer::WaterRenderer()
    : isInitialized_(false), vao_(0), vbo_(0), ebo_(0),
      shaderProgram_(0), vertexCount_(0), indexCount_(0) {
}

WaterRenderer::~WaterRenderer() {
    shutdown();
}

bool WaterRenderer::initialize(const WaterSurface* surface) {
    (void)surface;
    isInitialized_ = true;
    return true;
}

void WaterRenderer::shutdown() {
    isInitialized_ = false;
}

void WaterRenderer::updateMesh(const WaterSurface* surface) {
    (void)surface;
}

void WaterRenderer::render(const WaterSurface* surface, const Camera* camera,
                          const LightConfig* light, double time) {
    (void)surface;
    (void)camera;
    (void)light;
    (void)time;
}

const char* WaterRenderer::vertexShaderSource() {
    return "";
}

const char* WaterRenderer::fragmentShaderSource() {
    return "";
}

bool WaterRenderer::compileShaders() {
    return true;
}

bool WaterRenderer::createBuffers(const WaterSurface* surface) {
    (void)surface;
    return true;
}

} // namespace tp::presentation
