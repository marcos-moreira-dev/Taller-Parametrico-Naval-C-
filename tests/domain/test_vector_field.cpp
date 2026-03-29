#include <iostream>
#include <cassert>
#include <cmath>
#include "VectorField.hpp"

using namespace tp::domain;
using namespace tp::shared;

void test_uniform_field() {
    auto field = VectorField::uniform(2.0, 3.0);
    
    Vec2d value = field.getValue(0.0, 0.0);
    assert(std::abs(value.x - 2.0) < 0.001);
    assert(std::abs(value.y - 3.0) < 0.001);
    
    // Should be the same everywhere
    Vec2d value2 = field.getValue(100.0, -50.0);
    assert(std::abs(value2.x - 2.0) < 0.001);
    assert(std::abs(value2.y - 3.0) < 0.001);
    
    std::cout << "✓ Uniform field test passed\n";
}

void test_radial_field() {
    auto field = VectorField::radial(0.0, 0.0, 1.0);
    
    Vec2d value = field.getValue(1.0, 0.0);
    assert(std::abs(value.x - 1.0) < 0.001);
    assert(std::abs(value.y - 0.0) < 0.001);
    
    std::cout << "✓ Radial field test passed\n";
}

void test_rotational_field() {
    auto field = VectorField::rotational(0.0, 0.0, 1.0);
    
    Vec2d value = field.getValue(1.0, 0.0);
    assert(std::abs(value.x - 0.0) < 0.001);
    assert(std::abs(value.y - 1.0) < 0.001);
    
    std::cout << "✓ Rotational field test passed\n";
}

int main() {
    std::cout << "Running Vector Field Tests...\n\n";
    
    test_uniform_field();
    test_radial_field();
    test_rotational_field();
    
    std::cout << "\nAll vector field tests passed!\n";
    return 0;
}
