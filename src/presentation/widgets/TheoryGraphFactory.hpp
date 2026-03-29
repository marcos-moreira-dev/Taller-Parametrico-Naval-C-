#pragma once

#include "widgets/MathGraph.hpp"

#include <string>

namespace tp::presentation {

class TheoryGraphFactory {
public:
    static void configure(MathGraph& graph, const std::string& graphId);
};

} // namespace tp::presentation
