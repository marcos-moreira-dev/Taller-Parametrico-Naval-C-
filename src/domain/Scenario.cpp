#include "Scenario.hpp"
#include <stdexcept>

namespace tp::domain {

Scenario::Scenario(int width, int height) 
    : width_(width), height_(height), cells_(width * height, CellType::Water) {
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Scenario dimensions must be positive");
    }
}

CellType Scenario::getCell(int x, int y) const {
    if (!isValidPosition(x, y)) {
        throw std::out_of_range("Cell position out of bounds");
    }
    return cells_[index(x, y)];
}

void Scenario::setCell(int x, int y, CellType type) {
    if (!isValidPosition(x, y)) {
        throw std::out_of_range("Cell position out of bounds");
    }
    cells_[index(x, y)] = type;
}

bool Scenario::isValidPosition(int x, int y) const {
    return x >= 0 && x < width_ && y >= 0 && y < height_;
}

bool Scenario::isNavigable(int x, int y) const {
    if (!isValidPosition(x, y)) return false;
    return getCell(x, y) == CellType::Water;
}

void Scenario::fill(CellType type) {
    std::fill(cells_.begin(), cells_.end(), type);
}

void Scenario::clear() {
    fill(CellType::Water);
}

}
