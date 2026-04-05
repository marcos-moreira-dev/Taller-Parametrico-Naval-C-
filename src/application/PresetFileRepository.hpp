#pragma once

#include "ExperimentService.hpp"

#include <filesystem>
#include <vector>

namespace tp::application {

class PresetFileRepository {
public:
    explicit PresetFileRepository(std::filesystem::path rootDirectory);

    std::vector<ScenarioPreset> loadPresets() const;

private:
    std::filesystem::path rootDirectory_;
};

} // namespace tp::application
