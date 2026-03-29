#pragma once

#include "TheoryTopic.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace tp::education {

class TheoryRepository {
public:
    TheoryRepository();

    std::vector<TheoryTopic> loadAllTopics() const;
    std::optional<TheoryTopic> loadTopic(const std::string& id) const;
    const std::filesystem::path& theoryRoot() const { return theoryRoot_; }
    const std::filesystem::path& webRoot() const { return webRoot_; }
    std::filesystem::path theoryTemplatePath() const { return webRoot_ / "theory_base.html"; }

private:
    std::filesystem::path theoryRoot_;
    std::filesystem::path webRoot_;

    std::vector<std::string> loadTopicOrder() const;
    TheoryTopic parseTopic(const std::string& id, const std::filesystem::path& topicDir) const;

    static std::string readUtf8File(const std::filesystem::path& path);
    static std::string extractJsonString(const std::string& json, const std::string& key);
    static int extractJsonInt(const std::string& json, const std::string& key, int fallback);
};

} // namespace tp::education
