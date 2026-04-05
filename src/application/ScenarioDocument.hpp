#pragma once

#include "ExperimentService.hpp"

#include <string>

namespace tp::application {

class ScenarioDocument {
public:
    explicit ScenarioDocument(ExperimentService& service);

    ExperimentConfig& config() { return service_.getConfig(); }
    const ExperimentConfig& config() const { return service_.getConfig(); }

    tp::domain::Scenario& scenario() { return service_.getConfig().scenario; }
    tp::domain::Boat& boat() { return service_.getConfig().boat; }
    FieldConfigView& fieldView() { return service_.getConfig().fieldView; }

    void notifyChanged();
    bool setCell(int x, int y, tp::shared::CellType type);
    bool placeBoat(const tp::shared::Vec2d& position);

    void createScenario(int width, int height, const tp::shared::Vec2d& boatPosition, const std::string& name = "Nuevo Escenario");
    void resizeScenario(int width, int height);
    void fill(tp::shared::CellType type);
    void clear();
    bool rebuildField(std::string* errorMessage = nullptr);

private:
    ExperimentService& service_;
};

} // namespace tp::application
