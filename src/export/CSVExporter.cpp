#include "CSVExporter.hpp"
#include <iomanip>
#include <sstream>
#include <chrono>

namespace tp::exporting {

bool CSVExporter::exportTrajectory(const std::string& filepath,
                                    const std::vector<tp::simulation::PhysicsState>& states) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Header
    file << "time,position_x,position_y,velocity_x,velocity_y,"
         << "speed,kinetic_energy,total_energy,accumulated_work,collision\n";
    
    // Data
    for (const auto& state : states) {
        file << std::fixed << std::setprecision(6)
             << state.time << ","
             << state.position.x << ","
             << state.position.y << ","
             << state.velocity.x << ","
             << state.velocity.y << ","
             << state.velocity.magnitude() << ","
             << state.kineticEnergy << ","
             << state.totalEnergy << ","
             << state.accumulatedWork << ","
             << (state.collision ? "1" : "0") << "\n";
    }
    
    return true;
}

bool CSVExporter::exportSummary(const std::string& filepath,
                                 const tp::simulation::SimulationResult& result) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Metric,Value\n";
    file << "Final Time," << result.finalTime << "\n";
    file << "Total Work," << result.totalWork << "\n";
    file << "Max Speed," << result.maxSpeed << "\n";
    file << "Total Distance," << result.totalDistance << "\n";
    file << "Ended By Collision," << (result.endedByCollision ? "Yes" : "No") << "\n";
    file << "Number of States," << result.states.size() << "\n";
    
    return true;
}

std::string CSVExporter::generateFilename(const std::string& prefix) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << prefix << "_" 
       << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S")
       << ".csv";
    return ss.str();
}

}
