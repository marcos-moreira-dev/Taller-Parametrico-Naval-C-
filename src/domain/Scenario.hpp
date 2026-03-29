#pragma once

/**
 * @file Scenario.hpp
 * @brief Definición del escenario/mapa de simulación
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Representa el mundo de simulación como una grille de celdas.
 * Cada celda puede ser agua, tierra u obstáculo.
 * 
 * @see SimulationCanvas para renderizado
 * @see CellType para tipos de celdas
 */

#include <vector>
#include <string>
#include "Vec2.hpp"
#include "Enums.hpp"

namespace tp::domain {

using namespace tp::shared;

class Scenario {
public:
    Scenario(int width, int height);
    
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    CellType getCell(int x, int y) const;
    void setCell(int x, int y, CellType type);
    bool isValidPosition(int x, int y) const;
    bool isNavigable(int x, int y) const;
    
    void fill(CellType type);
    void clear();
    
private:
    int width_;
    int height_;
    std::string name_;
    std::vector<CellType> cells_;
    
    size_t index(int x, int y) const { return y * width_ + x; }
};

}
