#pragma once

/**
 * @file EditorCommand.hpp
 * @brief Sistema de comandos para undo/redo del editor
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Implementa el patrón Command para undo/redo:
 * - EditorCommand: interfaz abstracta
 * - PaintCellCommand: pintar una celda
 * - PaintStrokeCommand: pintar múltiples celdas
 * - ClearScenarioCommand: limpiar escenario
 * - FillScenarioCommand: llenar con tipo
 * - CommandHistory: pila de comandos (max 50)
 * 
 * @see EditorPanel para uso en UI
 */

#include <vector>
#include <memory>
#include <stack>
#include "../../domain/Scenario.hpp"
#include "../../shared/Enums.hpp"

namespace tp::presentation {

// Base class for editor commands
class EditorCommand {
public:
    virtual ~EditorCommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual const char* getDescription() const = 0;
};

// Command to paint a single cell
class PaintCellCommand : public EditorCommand {
public:
    PaintCellCommand(tp::domain::Scenario& scenario, int x, int y, 
                     tp::shared::CellType newType, tp::shared::CellType oldType)
        : scenario_(scenario), x_(x), y_(y), newType_(newType), oldType_(oldType) {}
    
    void execute() override {
        scenario_.setCell(x_, y_, newType_);
    }
    
    void undo() override {
        scenario_.setCell(x_, y_, oldType_);
    }
    
    const char* getDescription() const override {
        return "Pintar celda";
    }
    
private:
    tp::domain::Scenario& scenario_;
    int x_, y_;
    tp::shared::CellType newType_;
    tp::shared::CellType oldType_;
};

// Command to paint multiple cells (brush stroke)
class PaintStrokeCommand : public EditorCommand {
public:
    PaintStrokeCommand(tp::domain::Scenario& scenario)
        : scenario_(scenario) {}
    
    void addCell(int x, int y, tp::shared::CellType newType, tp::shared::CellType oldType) {
        cells_.push_back({x, y, newType, oldType});
    }
    
    bool isEmpty() const {
        return cells_.empty();
    }
    
    void execute() override {
        for (const auto& cell : cells_) {
            scenario_.setCell(cell.x, cell.y, cell.newType);
        }
    }
    
    void undo() override {
        for (const auto& cell : cells_) {
            scenario_.setCell(cell.x, cell.y, cell.oldType);
        }
    }
    
    const char* getDescription() const override {
        return "Pintar trazo";
    }
    
private:
    struct CellChange {
        int x, y;
        tp::shared::CellType newType;
        tp::shared::CellType oldType;
    };
    
    tp::domain::Scenario& scenario_;
    std::vector<CellChange> cells_;
};

// Command to clear scenario
class ClearScenarioCommand : public EditorCommand {
public:
    ClearScenarioCommand(tp::domain::Scenario& scenario)
        : scenario_(scenario) {
        // Store old state
        int w = scenario.getWidth();
        int h = scenario.getHeight();
        oldCells_.reserve(w * h);
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                oldCells_.push_back(scenario.getCell(x, y));
            }
        }
    }
    
    void execute() override {
        scenario_.clear();
    }
    
    void undo() override {
        int w = scenario_.getWidth();
        int h = scenario_.getHeight();
        for (int y = 0; y < h && y * w < static_cast<int>(oldCells_.size()); ++y) {
            for (int x = 0; x < w && y * w + x < static_cast<int>(oldCells_.size()); ++x) {
                scenario_.setCell(x, y, oldCells_[y * w + x]);
            }
        }
    }
    
    const char* getDescription() const override {
        return "Limpiar escenario";
    }
    
private:
    tp::domain::Scenario& scenario_;
    std::vector<tp::shared::CellType> oldCells_;
};

// Command to fill with specific cell type
class FillScenarioCommand : public EditorCommand {
public:
    FillScenarioCommand(tp::domain::Scenario& scenario, tp::shared::CellType fillType)
        : scenario_(scenario), fillType_(fillType) {
        // Store old state
        int w = scenario.getWidth();
        int h = scenario.getHeight();
        oldCells_.reserve(w * h);
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                oldCells_.push_back(scenario.getCell(x, y));
            }
        }
    }
    
    void execute() override {
        scenario_.fill(fillType_);
    }
    
    void undo() override {
        int w = scenario_.getWidth();
        int h = scenario_.getHeight();
        for (int y = 0; y < h && y * w < static_cast<int>(oldCells_.size()); ++y) {
            for (int x = 0; x < w && y * w + x < static_cast<int>(oldCells_.size()); ++x) {
                scenario_.setCell(x, y, oldCells_[y * w + x]);
            }
        }
    }
    
    const char* getDescription() const override {
        return fillType_ == tp::shared::CellType::Water ? "Llenar con agua" : "Llenar con tierra";
    }
    
private:
    tp::domain::Scenario& scenario_;
    tp::shared::CellType fillType_;
    std::vector<tp::shared::CellType> oldCells_;
};

// Command history manager
class CommandHistory {
public:
    static constexpr size_t MAX_HISTORY = 50;
    
    void executeCommand(std::unique_ptr<EditorCommand> command) {
        command->execute();
        
        // Clear redo stack when new command is executed
        while (!redoStack_.empty()) {
            redoStack_.pop();
        }
        
        // Add to undo stack
        undoStack_.push(std::move(command));
        
        // Limit history size
        while (undoStack_.size() > MAX_HISTORY) {
            // Remove oldest command (bottom of stack)
            std::stack<std::unique_ptr<EditorCommand>> temp;
            while (undoStack_.size() > 1) {
                temp.push(std::move(undoStack_.top()));
                undoStack_.pop();
            }
            undoStack_.pop(); // Remove oldest
            while (!temp.empty()) {
                undoStack_.push(std::move(temp.top()));
                temp.pop();
            }
        }
    }
    
    bool canUndo() const {
        return !undoStack_.empty();
    }
    
    bool canRedo() const {
        return !redoStack_.empty();
    }
    
    void undo() {
        if (!undoStack_.empty()) {
            auto command = std::move(undoStack_.top());
            undoStack_.pop();
            command->undo();
            redoStack_.push(std::move(command));
        }
    }
    
    void redo() {
        if (!redoStack_.empty()) {
            auto command = std::move(redoStack_.top());
            redoStack_.pop();
            command->execute();
            undoStack_.push(std::move(command));
        }
    }
    
    const char* getUndoDescription() const {
        if (!undoStack_.empty()) {
            return undoStack_.top()->getDescription();
        }
        return "";
    }
    
    const char* getRedoDescription() const {
        if (!redoStack_.empty()) {
            return redoStack_.top()->getDescription();
        }
        return "";
    }
    
    void clear() {
        while (!undoStack_.empty()) {
            undoStack_.pop();
        }
        while (!redoStack_.empty()) {
            redoStack_.pop();
        }
    }
    
private:
    std::stack<std::unique_ptr<EditorCommand>> undoStack_;
    std::stack<std::unique_ptr<EditorCommand>> redoStack_;
};

} // namespace tp::presentation
