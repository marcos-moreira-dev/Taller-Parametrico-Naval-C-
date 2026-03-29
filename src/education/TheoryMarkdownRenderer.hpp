#pragma once

#include <string>

namespace tp::education {

class TheoryMarkdownRenderer {
public:
    std::string toHtml(const std::string& markdownUtf8) const;

private:
    static std::string escapeHtml(const std::string& text);
    static std::string processInlineMarkup(const std::string& text);
    static std::string latexToPlainText(std::string latex);
};

} // namespace tp::education
