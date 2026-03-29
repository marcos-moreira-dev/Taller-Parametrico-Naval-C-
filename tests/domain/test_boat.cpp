#include <iostream>
#include <cassert>
#include "Boat.hpp"

using namespace tp::domain;
using namespace tp::shared;

void test_boat_defaults() {
    Boat boat;
    
    assert(boat.getMass() == 1.0);
    assert(boat.getSize() == 1.0);
    assert(boat.getDragCoefficient() == 0.1);
    
    Vec2d pos = boat.getPosition();
    assert(pos.x == 0.0);
    assert(pos.y == 0.0);
    
    std::cout << "✓ Boat defaults test passed\n";
}

void test_boat_setters() {
    Boat boat;
    
    boat.setPosition(Vec2d(10.0, 20.0));
    boat.setVelocity(Vec2d(5.0, 3.0));
    boat.setOrientation(1.57);
    boat.setMass(2.5);
    
    Vec2d pos = boat.getPosition();
    assert(pos.x == 10.0);
    assert(pos.y == 20.0);
    
    assert(boat.getMass() == 2.5);
    
    std::cout << "✓ Boat setters test passed\n";
}

void test_boat_reset() {
    Boat boat;
    boat.setPosition(Vec2d(10.0, 20.0));
    boat.setVelocity(Vec2d(5.0, 3.0));
    
    boat.reset();
    
    Vec2d pos = boat.getPosition();
    assert(pos.x == 0.0);
    assert(pos.y == 0.0);
    
    std::cout << "✓ Boat reset test passed\n";
}

int main() {
    std::cout << "Running Boat Tests...\n\n";
    
    test_boat_defaults();
    test_boat_setters();
    test_boat_reset();
    
    std::cout << "\nAll boat tests passed!\n";
    return 0;
}
