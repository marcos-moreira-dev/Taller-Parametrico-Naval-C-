#pragma once

#include "Scenario.hpp"
#include "VectorField.hpp"

#include <vector>

namespace tp::presentation {

struct FieldArrowSample {
    double x;
    double y;
    tp::shared::Vec2d direction;
    double magnitude;
};

class FieldArrowSampler {
public:
    static std::vector<FieldArrowSample> sample(const tp::domain::Scenario* scenario,
                                                const tp::domain::VectorField* field,
                                                int step,
                                                double minMagnitude,
                                                double offset = 0.5);
};

} // namespace tp::presentation
