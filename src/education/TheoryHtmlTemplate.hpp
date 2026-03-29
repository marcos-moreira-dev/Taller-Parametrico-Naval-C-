#pragma once

#include "TheoryTopic.hpp"

#include <filesystem>
#include <string>

namespace tp::education {

class TheoryHtmlTemplate {
public:
    explicit TheoryHtmlTemplate(std::filesystem::path templatePath);

    std::string render(const TheoryTopic& topic, const std::string& bodyHtml) const;
    const std::filesystem::path& templatePath() const { return templatePath_; }

private:
    std::filesystem::path templatePath_;
    std::string templateHtml_;

    static std::string readUtf8File(const std::filesystem::path& path);
    static std::string replaceAll(std::string text, const std::string& needle, const std::string& replacement);
    static std::string escapeHtml(const std::string& text);
};

} // namespace tp::education
