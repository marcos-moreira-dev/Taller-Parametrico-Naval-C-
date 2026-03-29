#pragma once

#include <string>

namespace tp::education {

struct TheoryTopic {
    std::string id;
    std::string title;
    std::string category;
    std::string description;
    std::string formulaLatex;
    std::string bodyMarkdown;
    std::string relatedScenario;
    std::string graphId;
    int difficulty = 1;
};

} // namespace tp::education
