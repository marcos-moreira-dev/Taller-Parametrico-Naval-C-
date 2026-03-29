#include "Rendering2D.hpp"
#include "SimulationCanvas.hpp"
#include <wx/wx.h>

namespace tp::presentation {

Rendering2DContext::Rendering2DContext(wxWindow* parent)
    : parent_(parent),
      canvasWindow_(nullptr),
      canvasImpl_(nullptr) {
}

Rendering2DContext::~Rendering2DContext() {
    shutdown();
}

void Rendering2DContext::initialize() {
    if (canvasWindow_) return;
    
    canvasWindow_ = new wxPanel(parent_, wxID_ANY);
    canvasImpl_ = new SimulationCanvas(parent_);
}

void Rendering2DContext::shutdown() {
    if (canvasWindow_) {
        delete static_cast<SimulationCanvas*>(canvasImpl_);
        canvasImpl_ = nullptr;
        canvasWindow_ = nullptr;
    }
}

void Rendering2DContext::resize(int width, int height) {
    if (canvasWindow_) {
        canvasWindow_->SetSize(width, height);
    }
}

void Rendering2DContext::render(double deltaTime) {
    if (canvasImpl_) {
        auto* canvas = static_cast<SimulationCanvas*>(canvasImpl_);
        canvas->Update();
    }
}

void Rendering2DContext::refresh() {
    if (canvasWindow_) {
        canvasWindow_->Refresh();
    }
}

void Rendering2DContext::setScenario(const domain::Scenario* scenario) {
    if (canvasImpl_) {
        auto* canvas = static_cast<SimulationCanvas*>(canvasImpl_);
        canvas->setScenario(scenario);
    }
}

void Rendering2DContext::setVectorField(const domain::VectorField* field) {
    if (canvasImpl_) {
        auto* canvas = static_cast<SimulationCanvas*>(canvasImpl_);
        canvas->setField(field);
    }
}

void Rendering2DContext::setBoat(const domain::Boat* boat) {
    if (canvasImpl_) {
        auto* canvas = static_cast<SimulationCanvas*>(canvasImpl_);
        canvas->setBoat(boat);
    }
}

void Rendering2DContext::setBoatPosition(double x, double y) {
    if (canvasImpl_) {
        auto* canvas = static_cast<SimulationCanvas*>(canvasImpl_);
        canvas->updateBoatPosition(x, y);
    }
}

void Rendering2DContext::setShowGrid(bool show) {
    if (canvasImpl_) {
        auto* canvas = static_cast<SimulationCanvas*>(canvasImpl_);
        canvas->showGrid(show);
    }
}

void Rendering2DContext::setShowField(bool show) {
    if (canvasImpl_) {
        auto* canvas = static_cast<SimulationCanvas*>(canvasImpl_);
        canvas->showField(show);
    }
}

void Rendering2DContext::setShowTrajectory(bool show) {
    if (canvasImpl_) {
        auto* canvas = static_cast<SimulationCanvas*>(canvasImpl_);
        canvas->showTrajectory(show);
    }
}

void Rendering2DContext::setShowWaterFlow(bool show) {
    if (canvasImpl_) {
        auto* canvas = static_cast<SimulationCanvas*>(canvasImpl_);
        canvas->showWaterEffects(show);
    }
}

void Rendering2DContext::startAnimation() {
}

void Rendering2DContext::stopAnimation() {
}

void Rendering2DContext::pauseAnimation() {
}

void Rendering2DContext::resumeAnimation() {
}

bool Rendering2DContext::isAnimating() const {
    return false;
}

void Rendering2DContext::setTrajectory(const std::vector<TrajectoryPoint>& trajectory) {
    if (canvasImpl_) {
        auto* canvas = static_cast<SimulationCanvas*>(canvasImpl_);
        std::vector<tp::presentation::TrajectoryPoint> converted;
        converted.reserve(trajectory.size());
        for (const auto& p : trajectory) {
            converted.push_back({p.x, p.y, p.time});
        }
        canvas->setTrajectory(converted);
    }
}

void Rendering2DContext::clearTrajectory() {
    if (canvasImpl_) {
        auto* canvas = static_cast<SimulationCanvas*>(canvasImpl_);
        canvas->clearTrajectory();
    }
}

} // namespace tp::presentation
