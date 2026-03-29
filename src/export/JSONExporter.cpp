#include "JSONExporter.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include "Scenario.hpp"

namespace tp::exporting {

std::string JSONExporter::escapeJson(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c;
        }
    }
    
    return result;
}

std::string JSONExporter::formatDouble(double value, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

bool JSONExporter::exportTrajectory(const std::string& filepath,
                                     const std::vector<tp::simulation::PhysicsState>& states,
                                     const std::string& scenarioName) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    auto now = std::time(nullptr);
    auto tm = std::localtime(&now);
    char timestamp[30];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", tm);
    
    file << "{\n";
    file << "  \"metadata\": {\n";
    file << "    \"version\": \"1.0\",\n";
    file << "    \"created\": \"" << timestamp << "\",\n";
    file << "    \"type\": \"trajectory\",\n";
    if (!scenarioName.empty()) {
        file << "    \"scenario\": \"" << escapeJson(scenarioName) << "\",\n";
    }
    file << "    \"count\": " << states.size() << "\n";
    file << "  },\n";
    
    file << "  \"states\": [\n";
    
    for (size_t i = 0; i < states.size(); ++i) {
        const auto& s = states[i];
        file << "    {\n";
        file << "      \"time\": " << formatDouble(s.time) << ",\n";
        file << "      \"position\": {\n";
        file << "        \"x\": " << formatDouble(s.position.x) << ",\n";
        file << "        \"y\": " << formatDouble(s.position.y) << "\n";
        file << "      },\n";
        file << "      \"velocity\": {\n";
        file << "        \"x\": " << formatDouble(s.velocity.x) << ",\n";
        file << "        \"y\": " << formatDouble(s.velocity.y) << ",\n";
        file << "        \"magnitude\": " << formatDouble(s.velocity.magnitude()) << "\n";
        file << "      },\n";
        file << "      \"orientation\": " << formatDouble(s.orientation) << ",\n";
        file << "      \"energy\": {\n";
        file << "        \"kinetic\": " << formatDouble(s.kineticEnergy) << ",\n";
        file << "        \"total\": " << formatDouble(s.totalEnergy) << "\n";
        file << "      },\n";
        file << "      \"work\": " << formatDouble(s.accumulatedWork) << ",\n";
        file << "      \"collision\": " << (s.collision ? "true" : "false") << "\n";
        file << "    }";
        
        if (i < states.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    return true;
}

bool JSONExporter::exportSummary(const std::string& filepath,
                                  const tp::simulation::SimulationResult& result) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    auto now = std::time(nullptr);
    auto tm = std::localtime(&now);
    char timestamp[30];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", tm);
    
    file << "{\n";
    file << "  \"metadata\": {\n";
    file << "    \"version\": \"1.0\",\n";
    file << "    \"created\": \"" << timestamp << "\",\n";
    file << "    \"type\": \"summary\"\n";
    file << "  },\n";
    
    file << "  \"results\": {\n";
    file << "    \"finalTime\": " << formatDouble(result.finalTime) << ",\n";
    file << "    \"totalWork\": " << formatDouble(result.totalWork) << ",\n";
    file << "    \"maxSpeed\": " << formatDouble(result.maxSpeed) << ",\n";
    file << "    \"totalDistance\": " << formatDouble(result.totalDistance) << ",\n";
    file << "    \"endedByCollision\": " << (result.endedByCollision ? "true" : "false") << ",\n";
    file << "    \"stateCount\": " << result.states.size() << ",\n";
    file << "    \"method\": \"";
    
    switch (result.methodUsed) {
        case tp::shared::IntegrationMethod::Euler:
            file << "Euler\"\n";
            break;
        case tp::shared::IntegrationMethod::ImprovedEuler:
            file << "Improved Euler\"\n";
            break;
        case tp::shared::IntegrationMethod::RK4:
            file << "RK4\"\n";
            break;
        default:
            file << "Unknown\"\n";
    }
    
    file << "  }\n";
    file << "}\n";
    
    return true;
}

std::string JSONExporter::generateFilename(const std::string& prefix) {
    auto now = std::time(nullptr);
    auto tm = std::localtime(&now);
    
    std::ostringstream ss;
    ss << prefix << "_"
       << (tm->tm_year + 1900)
       << std::setfill('0') << std::setw(2) << (tm->tm_mon + 1)
       << std::setfill('0') << std::setw(2) << tm->tm_mday
       << "_"
       << std::setfill('0') << std::setw(2) << tm->tm_hour
       << std::setfill('0') << std::setw(2) << tm->tm_min
       << std::setfill('0') << std::setw(2) << tm->tm_sec
       << ".json";
    
    return ss.str();
}

} // namespace tp::exporting
