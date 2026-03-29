#include "TheoryHtmlTemplate.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace tp::education {

TheoryHtmlTemplate::TheoryHtmlTemplate(std::filesystem::path templatePath)
    : templatePath_(std::move(templatePath))
    , templateHtml_(readUtf8File(templatePath_)) {
}

std::string TheoryHtmlTemplate::render(const TheoryTopic& topic, const std::string& bodyHtml) const {
    std::string html = templateHtml_;
    html = replaceAll(html, "{{body_html}}", bodyHtml);
    return html;
}

std::string TheoryHtmlTemplate::readUtf8File(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("No se pudo abrir el template: " + path.string());
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    std::string data = buffer.str();
    if (data.size() >= 3 && static_cast<unsigned char>(data[0]) == 0xEF &&
        static_cast<unsigned char>(data[1]) == 0xBB && static_cast<unsigned char>(data[2]) == 0xBF) {
        data.erase(0, 3);
    }
    return data;
}

std::string TheoryHtmlTemplate::replaceAll(std::string text, const std::string& needle, const std::string& replacement) {
    size_t pos = 0;
    while ((pos = text.find(needle, pos)) != std::string::npos) {
        text.replace(pos, needle.length(), replacement);
        pos += replacement.length();
    }
    return text;
}

std::string TheoryHtmlTemplate::escapeHtml(const std::string& text) {
    std::string out = text;
    out = replaceAll(out, "&", "&amp;");
    out = replaceAll(out, "<", "&lt;");
    out = replaceAll(out, ">", "&gt;");
    return out;
}

} // namespace tp::education
