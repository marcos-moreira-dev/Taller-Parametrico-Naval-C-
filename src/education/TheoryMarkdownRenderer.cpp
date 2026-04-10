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
                const std::string mathText = trim(mathBuffer.str());
                html << "<div class='math-block'><pre>" << escapeHtml(mathText)
                     << "</pre></div>\n";
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

        if (line.rfind("> ", 0) == 0) {
            flushParagraph();
            flushList();
            html << "<div class='note-box'>" << processInlineMarkup(line.substr(2)) << "</div>\n";
            continue;
        }

        static const std::regex imageRegex(R"(^!\[([^\]]*)\]\(([^\)]+)\)$)");
        std::smatch imageMatch;
        if (std::regex_match(line, imageMatch, imageRegex)) {
            flushParagraph();
            flushList();
            const auto alt = trim(imageMatch[1].str());
            const auto src = trim(imageMatch[2].str());
            const bool isMathImage = alt == "math" || src.find("math/block_") != std::string::npos;
            if (isMathImage) {
                html << "<div class='math-block math-image-block'><img class='block-math-img' src=\"" << escapeAttribute(src)
                     << "\" alt=\"bloque matematico\" /></div>\n";
            } else {
                html << "<figure class='theory-figure'><img src=\"" << escapeAttribute(src)
                     << "\" alt=\"" << escapeAttribute(alt) << "\" />";
                if (!alt.empty()) {
                    html << "<figcaption>" << escapeHtml(alt) << "</figcaption>";
                }
                html << "</figure>\n";
            }
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

std::string TheoryMarkdownRenderer::escapeAttribute(const std::string& text) {
    std::string out = escapeHtml(text);
    out = std::regex_replace(out, std::regex("\""), "&quot;");
    out = std::regex_replace(out, std::regex("'"), "&#39;");
    return out;
}

std::string TheoryMarkdownRenderer::processInlineMarkup(const std::string& text) {
    std::string output;
    size_t i = 0;

    auto appendEscaped = [&](const std::string& plain) {
        output += escapeHtml(plain);
    };

    while (i < text.size()) {
        if (text.compare(i, 10, "[[MATHIMG:") == 0) {
            const size_t sep = text.find('|', i + 10);
            const size_t end = text.find("]]", i + 10);
            if (sep != std::string::npos && end != std::string::npos && sep < end) {
                const std::string src = text.substr(i + 10, sep - (i + 10));
                const std::string alt = text.substr(sep + 1, end - (sep + 1));
                output += "<img class='inline-math-img' src=\"" + escapeAttribute(src)
                       + "\" alt=\"" + escapeAttribute(alt) + "\" />";
                i = end + 2;
                continue;
            }
        }

        if (i + 1 < text.size() && text[i] == '*' && text[i + 1] == '*') {
            const size_t end = text.find("**", i + 2);
            if (end != std::string::npos) {
                output += "<strong>" + escapeHtml(text.substr(i + 2, end - i - 2)) + "</strong>";
                i = end + 2;
                continue;
            }
        }

        if (text[i] == '`') {
            const size_t end = text.find('`', i + 1);
            if (end != std::string::npos) {
                output += "<code>" + escapeHtml(text.substr(i + 1, end - i - 1)) + "</code>";
                i = end + 1;
                continue;
            }
        }

        if (i + 1 < text.size() && text[i] == '$' && text[i + 1] == '$') {
            const size_t end = text.find("$$", i + 2);
            if (end != std::string::npos) {
                const std::string latex = text.substr(i + 2, end - i - 2);
                output += "<span class='math-inline'>$$" + latex + "$$</span>";
                i = end + 2;
                continue;
            }
        }

        if (text[i] == '$') {
            const size_t end = text.find('$', i + 1);
            if (end != std::string::npos) {
                const std::string latex = text.substr(i + 1, end - i - 1);
                output += "<span class='math-inline'>$" + latex + "$</span>";
                i = end + 1;
                continue;
            }
        }

        appendEscaped(std::string(1, text[i]));
        ++i;
    }

    return output;
}

} // namespace tp::education
