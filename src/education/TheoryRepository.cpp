#include "TheoryRepository.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace tp::education {

namespace {

std::filesystem::path buildRootPath() {
#ifdef TP_SOURCE_DIR
    return std::filesystem::path(TP_SOURCE_DIR);
#else
    return std::filesystem::current_path();
#endif
}

} // namespace

TheoryRepository::TheoryRepository()
    : theoryRoot_(buildRootPath() / "assets" / "theory")
    , webRoot_(buildRootPath() / "assets" / "web") {
}

std::vector<TheoryTopic> TheoryRepository::loadAllTopics() const {
    std::vector<TheoryTopic> topics;
    for (const std::string& id : loadTopicOrder()) {
        if (auto topic = loadTopic(id)) {
            topics.push_back(*topic);
        }
    }
    return topics;
}

std::optional<TheoryTopic> TheoryRepository::loadTopic(const std::string& id) const {
    const std::filesystem::path topicDir = theoryRoot_ / id;
    if (!std::filesystem::exists(topicDir)) {
        return std::nullopt;
    }
    return parseTopic(id, topicDir);
}

std::vector<std::string> TheoryRepository::loadTopicOrder() const {
    const std::filesystem::path indexPath = theoryRoot_ / "index.json";
    const std::string json = readUtf8File(indexPath);
    std::vector<std::string> order;

    const auto topicsPos = json.find("\"topics\"");
    if (topicsPos == std::string::npos) {
        return order;
    }

    const auto openBracket = json.find('[', topicsPos);
    const auto closeBracket = json.find(']', openBracket);
    if (openBracket == std::string::npos || closeBracket == std::string::npos) {
        return order;
    }

    const std::string body = json.substr(openBracket + 1, closeBracket - openBracket - 1);
    const std::regex itemRegex("\"([^\"]+)\"");
    for (auto it = std::sregex_iterator(body.begin(), body.end(), itemRegex); it != std::sregex_iterator(); ++it) {
        order.push_back((*it)[1].str());
    }

    return order;
}

TheoryTopic TheoryRepository::parseTopic(const std::string& id, const std::filesystem::path& topicDir) const {
    const std::string meta = readUtf8File(topicDir / "meta.json");

    TheoryTopic topic;
    topic.id = id;
    topic.title = extractJsonString(meta, "title");
    topic.category = extractJsonString(meta, "category");
    topic.description = extractJsonString(meta, "description");
    topic.relatedScenario = extractJsonString(meta, "relatedScenario");
    topic.graphId = extractJsonString(meta, "graphId");
    topic.difficulty = extractJsonInt(meta, "difficulty", 1);
    topic.contentBasePath = topicDir.generic_string();
    topic.formulaLatex = readUtf8File(topicDir / "formula.tex");
    topic.bodyMarkdown = readUtf8File(topicDir / "body.md");

    const auto exercisePath = topicDir / "exercise.md";
    if (std::filesystem::exists(exercisePath)) {
        topic.exerciseMarkdown = readUtf8File(exercisePath);
    }

    const auto figurePath = topicDir / "figure_main.png";
    if (std::filesystem::exists(figurePath)) {
        topic.figureMainPath = figurePath.generic_string();
    }

    const auto formulaImagePath = topicDir / "formula_main.png";
    if (std::filesystem::exists(formulaImagePath)) {
        topic.formulaImagePath = formulaImagePath.generic_string();
    }

    const auto figureNotePath = topicDir / "figure_note.md";
    if (std::filesystem::exists(figureNotePath)) {
        topic.figureNoteMarkdown = readUtf8File(figureNotePath);
    }

    return topic;
}

std::string TheoryRepository::readUtf8File(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("No se pudo abrir el archivo: " + path.string());
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

std::string TheoryRepository::extractJsonString(const std::string& json, const std::string& key) {
    const std::regex pattern("\\\"" + key + "\\\"\\s*:\\s*\\\"([^\\\"]*)\\\"");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        return match[1].str();
    }
    return {};
}

int TheoryRepository::extractJsonInt(const std::string& json, const std::string& key, int fallback) {
    const std::regex pattern("\\\"" + key + "\\\"\\s*:\\s*([0-9]+)");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        return std::stoi(match[1].str());
    }
    return fallback;
}

} // namespace tp::education
