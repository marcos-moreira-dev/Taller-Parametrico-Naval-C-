#include "Rendering3D.hpp"
#include "WaterSurface.hpp"
#include "WaterRenderer.hpp"
#include "Camera.hpp"

namespace tp::presentation {

Rendering3DContext::Rendering3DContext(wxWindow* parent)
    : parent_(parent),
      isInitialized_(false),
      isAnimating_(false),
      isPaused_(false),
      simulationTime_(0.0),
      lastFrameTime_(0.0),
      scenario_(nullptr),
      vectorField_(nullptr),
      boat_(nullptr),
      boatX_(0.0),
      boatY_(0.0),
      showGrid_(true),
      showField_(false),
      showTrajectory_(true),
      showWaterFlow_(false) {
}

Rendering3DContext::~Rendering3DContext() {
    shutdown();
}

void Rendering3DContext::initialize() {
    if (isInitialized_) return;
    isInitialized_ = true;
}

void Rendering3DContext::shutdown() {
    isInitialized_ = false;
}

void Rendering3DContext::resize(int width, int height) {
    (void)width;
    (void)height;
}

void Rendering3DContext::render(double deltaTime) {
    (void)deltaTime;
}

void Rendering3DContext::refresh() {
}

void Rendering3DContext::setScenario(const domain::Scenario* scenario) {
    scenario_ = scenario;
}

void Rendering3DContext::setVectorField(const domain::VectorField* field) {
    vectorField_ = field;
}

void Rendering3DContext::setBoat(const domain::Boat* boat) {
    boat_ = boat;
}

void Rendering3DContext::setBoatPosition(double x, double y) {
    boatX_ = x;
    boatY_ = y;
}

void Rendering3DContext::setShowGrid(bool show) {
    showGrid_ = show;
}

void Rendering3DContext::setShowField(bool show) {
    showField_ = show;
}

void Rendering3DContext::setShowTrajectory(bool show) {
    showTrajectory_ = show;
}

void Rendering3DContext::setShowWaterFlow(bool show) {
    showWaterFlow_ = show;
}

void Rendering3DContext::startAnimation() {
    isAnimating_ = true;
}

void Rendering3DContext::stopAnimation() {
    isAnimating_ = false;
    isPaused_ = false;
}

void Rendering3DContext::pauseAnimation() {
    if (!isAnimating_) return;
    isPaused_ = true;
}

void Rendering3DContext::resumeAnimation() {
    if (!isAnimating_) return;
    isPaused_ = false;
}

bool Rendering3DContext::isAnimating() const {
    return isAnimating_;
}

void Rendering3DContext::setTrajectory(const std::vector<TrajectoryPoint>& trajectory) {
    (void)trajectory;
}

void Rendering3DContext::clearTrajectory() {
}

} // namespace tp::presentation
