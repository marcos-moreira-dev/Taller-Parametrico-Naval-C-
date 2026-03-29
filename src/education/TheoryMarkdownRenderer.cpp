#include "TheoryMarkdownRenderer.hpp"

#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace tp::education {

namespace {

std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::vector<std::string> splitLines(const std::string& text) {
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
    }
    return lines;
}

std::string replaceAllCopy(std::string text, const std::string& from, const std::string& to) {
    size_t pos = 0;
    while ((pos = text.find(from, pos)) != std::string::npos) {
        text.replace(pos, from.size(), to);
        pos += to.size();
    }
    return text;
}

} // namespace

std::string TheoryMarkdownRenderer::toHtml(const std::string& markdownUtf8) const {
    const auto lines = splitLines(markdownUtf8);
    std::ostringstream html;

    bool inList = false;
    bool inMathBlock = false;
    std::ostringstream paragraph;
    std::ostringstream mathBuffer;

    auto flushParagraph = [&]() {
        const std::string text = trim(paragraph.str());
        if (!text.empty()) {
            html << "<p>" << processInlineMarkup(text) << "</p>\n";
        }
        paragraph.str({});
        paragraph.clear();
    };

    auto flushList = [&]() {
        if (inList) {
            html << "</ul>\n";
            inList = false;
        }
    };

    for (const std::string& rawLine : lines) {
        const std::string line = trim(rawLine);

        if (inMathBlock) {
            if (line == "$$") {
                html << "<div class='math-block'>" << escapeHtml(TheoryMarkdownRenderer::latexToPlainText(mathBuffer.str())) << "</div>\n";
                mathBuffer.str({});
                mathBuffer.clear();
                inMathBlock = false;
            } else {
                if (!mathBuffer.str().empty()) {
                    mathBuffer << "\n";
                }
                mathBuffer << rawLine;
            }
            continue;
        }

        if (line.empty()) {
            flushParagraph();
            flushList();
            continue;
        }

        if (line == "$$") {
            flushParagraph();
            flushList();
            inMathBlock = true;
            continue;
        }

        if (line.rfind("## ", 0) == 0) {
            flushParagraph();
            flushList();
            html << "<h2>" << processInlineMarkup(line.substr(3)) << "</h2>\n";
            continue;
        }

        if (line.rfind("### ", 0) == 0) {
            flushParagraph();
            flushList();
            html << "<h3>" << processInlineMarkup(line.substr(4)) << "</h3>\n";
            continue;
        }

        if (line.rfind("- ", 0) == 0) {
            flushParagraph();
            if (!inList) {
                html << "<ul>\n";
                inList = true;
            }
            html << "<li>" << processInlineMarkup(line.substr(2)) << "</li>\n";
            continue;
        }

        if (!paragraph.str().empty()) {
            paragraph << ' ';
        }
        paragraph << rawLine;
    }

    flushParagraph();
    flushList();
    return html.str();
}

std::string TheoryMarkdownRenderer::escapeHtml(const std::string& text) {
    std::string out = text;
    out = std::regex_replace(out, std::regex("&"), "&amp;");
    out = std::regex_replace(out, std::regex("<"), "&lt;");
    out = std::regex_replace(out, std::regex(">"), "&gt;");
    return out;
}

std::string TheoryMarkdownRenderer::processInlineMarkup(const std::string& text) {
    std::string output;
    size_t i = 0;

    auto appendEscaped = [&](const std::string& plain) {
        output += escapeHtml(plain);
    };

    while (i < text.size()) {
        if (i + 1 < text.size() && text[i] == '*' && text[i + 1] == '*') {
            size_t end = text.find("**", i + 2);
            if (end != std::string::npos) {
                output += "<strong>" + escapeHtml(text.substr(i + 2, end - i - 2)) + "</strong>";
                i = end + 2;
                continue;
            }
        }

        if (text[i] == '`') {
            size_t end = text.find('`', i + 1);
            if (end != std::string::npos) {
                output += "<code>" + escapeHtml(text.substr(i + 1, end - i - 1)) + "</code>";
                i = end + 1;
                continue;
            }
        }

        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '$') {
            size_t end = text.find("$$", i + 2);
            if (end != std::string::npos) {
                output += "<span class='math-inline'>" + escapeHtml(TheoryMarkdownRenderer::latexToPlainText(text.substr(i + 2, end - i - 2))) + "</span>";
                i = end + 2;
                continue;
            }
        }

        if (text[i] == '$') {
            size_t end = text.find('$', i + 1);
            if (end != std::string::npos) {
                output += "<span class='math-inline'>" + escapeHtml(TheoryMarkdownRenderer::latexToPlainText(text.substr(i + 1, end - i - 1))) + "</span>";
                i = end + 1;
                continue;
            }
        }

        appendEscaped(std::string(1, text[i]));
        ++i;
    }

    return output;
}

std::string TheoryMarkdownRenderer::latexToPlainText(std::string latex) {
    latex = trim(latex);
    latex = replaceAllCopy(latex, "\r", "");
    latex = replaceAllCopy(latex, "\n", " ");
    latex = replaceAllCopy(latex, "\\mathbf{", "");
    latex = replaceAllCopy(latex, "\\mathrm{", "");
    latex = replaceAllCopy(latex, "\\text{", "");
    latex = replaceAllCopy(latex, "\\operatorname{", "");
    latex = replaceAllCopy(latex, "\\big", "");
    latex = replaceAllCopy(latex, "\\Big", "");
    latex = replaceAllCopy(latex, "\\frac{", "frac(");
    latex = replaceAllCopy(latex, "\\sqrt{", "sqrt(");
    latex = replaceAllCopy(latex, "\\cdot", " * ");
    latex = replaceAllCopy(latex, "\\to", " -> ");
    latex = replaceAllCopy(latex, "\\qquad", "   ");
    latex = replaceAllCopy(latex, "\\quad", " ");
    latex = replaceAllCopy(latex, "\\,", " ");
    latex = replaceAllCopy(latex, "\\approx", " approx ");
    latex = replaceAllCopy(latex, "\\lVert", "|");
    latex = replaceAllCopy(latex, "\\rVert", "|");
    latex = replaceAllCopy(latex, "\\vec", "vec");
    latex = replaceAllCopy(latex, "\\nabla", "nabla");
    latex = replaceAllCopy(latex, "\\Gamma", "Gamma");
    latex = replaceAllCopy(latex, "\\pi", "pi");
    latex = replaceAllCopy(latex, "\\circ", "deg");
    latex = replaceAllCopy(latex, "\\left", "");
    latex = replaceAllCopy(latex, "\\right", "");
    latex = replaceAllCopy(latex, "\\begin{aligned}", "");
    latex = replaceAllCopy(latex, "\\end{aligned}", "");
    latex = replaceAllCopy(latex, "\\begin{array}{l}", "");
    latex = replaceAllCopy(latex, "\\end{array}", "");
    latex = replaceAllCopy(latex, "\\", "");
    latex = replaceAllCopy(latex, "{", "");
    latex = replaceAllCopy(latex, "}", "");
    latex = replaceAllCopy(latex, "&", "");
    latex = replaceAllCopy(latex, "^", "^");
    latex = replaceAllCopy(latex, "_", "_");
    latex = replaceAllCopy(latex, "  ", " ");
    latex = replaceAllCopy(latex, "  ", " ");
    latex = replaceAllCopy(latex, "frac(", "(");
    latex = replaceAllCopy(latex, ")(", ")/(");
    return trim(latex);
}

} // namespace tp::education
