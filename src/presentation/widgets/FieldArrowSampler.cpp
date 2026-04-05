#include "widgets/FieldArrowSampler.hpp"

#include <algorithm>

namespace tp::presentation {

std::vector<FieldArrowSample> FieldArrowSampler::sample(const tp::domain::Scenario* scenario,
                                                        const tp::domain::VectorField* field,
                                                        int step,
                                                        double minMagnitude,
                                                        double offset) {
    std::vector<FieldArrowSample> samples;
    if (!scenario || !field || step <= 0) {
        return samples;
    }

    const int width = scenario->getWidth();
    const int height = scenario->getHeight();

    for (int y = 1; y < height; y += step) {
        for (int x = 1; x < width; x += step) {
            if (scenario->getCell(x, y) != tp::shared::CellType::Water) {
                continue;
            }

            const auto vec = field->getValue(static_cast<double>(x), static_cast<double>(y));
            const double magnitude = vec.magnitude();
            if (magnitude < minMagnitude) {
                continue;
            }

            const auto direction = vec.normalized();
            samples.push_back(FieldArrowSample{
                static_cast<double>(x) + offset,
                static_cast<double>(y) + offset,
                direction,
                magnitude
            });
        }
    }

    return samples;
}

} // namespace tp::presentation
