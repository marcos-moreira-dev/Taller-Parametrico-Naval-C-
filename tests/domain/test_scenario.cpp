#include <iostream>
#include <cassert>
#include "Scenario.hpp"
#include "Enums.hpp"

using namespace tp::domain;
using namespace tp::shared;

void test_scenario_creation() {
    Scenario scenario(50, 30);
    assert(scenario.getWidth() == 50);
    assert(scenario.getHeight() == 30);
    std::cout << "✓ Scenario creation test passed\n";
}

void test_scenario_cells() {
    Scenario scenario(10, 10);
    
    // Default should be water
    assert(scenario.getCell(5, 5) == CellType::Water);
    
    // Set and get cell
    scenario.setCell(5, 5, CellType::Land);
    assert(scenario.getCell(5, 5) == CellType::Land);
    
    // Check navigable
    assert(!scenario.isNavigable(5, 5));
    assert(scenario.isNavigable(0, 0));
    
    std::cout << "✓ Scenario cells test passed\n";
}

void test_scenario_fill() {
    Scenario scenario(10, 10);
    scenario.fill(CellType::Land);
    
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            assert(scenario.getCell(x, y) == CellType::Land);
        }
    }
    std::cout << "✓ Scenario fill test passed\n";
}

int main() {
    std::cout << "Running Domain Tests...\n\n";
    
    test_scenario_creation();
    test_scenario_cells();
    test_scenario_fill();
    
    std::cout << "\nAll domain tests passed!\n";
    return 0;
}
