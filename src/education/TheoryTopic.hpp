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
    std::string exerciseMarkdown;
    std::string figureNoteMarkdown;
    std::string relatedScenario;
    std::string graphId;
    std::string figureMainPath;
    std::string formulaImagePath;
    std::string contentBasePath;
    int difficulty = 1;
};

} // namespace tp::education
