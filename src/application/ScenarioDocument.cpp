#include "ScenarioDocument.hpp"

namespace tp::application {

ScenarioDocument::ScenarioDocument(ExperimentService& service)
    : service_(service) {
}

void ScenarioDocument::notifyChanged() {
    service_.notifyConfigChange();
}

bool ScenarioDocument::setCell(int x, int y, tp::shared::CellType type) {
    auto& sc = service_.getConfig().scenario;
    if (!sc.isValidPosition(x, y)) {
        return false;
    }
    if (sc.getCell(x, y) == type) {
        return false;
    }
    sc.setCell(x, y, type);
    notifyChanged();
    return true;
}

bool ScenarioDocument::placeBoat(const tp::shared::Vec2d& position) {
    auto& sc = service_.getConfig().scenario;
    const int cellX = static_cast<int>(position.x);
    const int cellY = static_cast<int>(position.y);
    if (!sc.isNavigable(cellX, cellY)) {
        return false;
    }
    service_.getConfig().boat.setPosition(position);
    notifyChanged();
    return true;
}

void ScenarioDocument::createScenario(int width, int height, const tp::shared::Vec2d& boatPosition, const std::string& name) {
    auto& cfg = service_.getConfig();
    cfg.scenario = tp::domain::Scenario(width, height);
    cfg.scenario.setName(name);
    cfg.scenario.fill(tp::shared::CellType::Water);
    cfg.boat.setPosition(boatPosition);
    cfg.boat.setOrientation(0.0);
    notifyChanged();
}

void ScenarioDocument::resizeScenario(int width, int height) {
    auto& cfg = service_.getConfig();
    tp::domain::Scenario newScenario(width, height);
    newScenario.setName(cfg.scenario.getName());
    newScenario.fill(tp::shared::CellType::Water);

    const int copyWidth = std::min(width, cfg.scenario.getWidth());
    const int copyHeight = std::min(height, cfg.scenario.getHeight());
    for (int y = 0; y < copyHeight; ++y) {
        for (int x = 0; x < copyWidth; ++x) {
            newScenario.setCell(x, y, cfg.scenario.getCell(x, y));
        }
    }

    cfg.scenario = std::move(newScenario);
    notifyChanged();
}

void ScenarioDocument::fill(tp::shared::CellType type) {
    service_.getConfig().scenario.fill(type);
    notifyChanged();
}

void ScenarioDocument::clear() {
    service_.getConfig().scenario.clear();
    notifyChanged();
}

bool ScenarioDocument::rebuildField(std::string* errorMessage) {
    return service_.rebuildFieldFromView(errorMessage);
}

} // namespace tp::application
