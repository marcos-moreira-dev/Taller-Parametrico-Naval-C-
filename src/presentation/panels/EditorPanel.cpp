#include "panels/EditorPanel.hpp"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>

namespace tp::presentation {

enum {
    ID_TOOL_SELECT = 100,
    ID_TOOL_WATER,
    ID_TOOL_LAND,
    ID_TOOL_OBSTACLE,
    ID_TOOL_ERASE,
    ID_TOOL_BOAT,
    ID_BRUSH_SIZE,
    ID_SNAP_TOGGLE,
    ID_NEW_SCENARIO,
    ID_RESIZE_SCENARIO,
    ID_CLEAR_SCENARIO,
    ID_FILL_WATER,
    ID_FILL_LAND,
    ID_UNDO,
    ID_REDO,
    ID_FIELD_TYPE,
    ID_FIELD_INTENSITY,
    ID_FIELD_CENTER_X,
    ID_FIELD_CENTER_Y
};

wxBEGIN_EVENT_TABLE(EditorPanel, wxPanel)
    EVT_BUTTON(ID_TOOL_SELECT, EditorPanel::onToolSelect)
    EVT_BUTTON(ID_TOOL_WATER, EditorPanel::onToolSelect)
    EVT_BUTTON(ID_TOOL_LAND, EditorPanel::onToolSelect)
    EVT_BUTTON(ID_TOOL_OBSTACLE, EditorPanel::onToolSelect)
    EVT_BUTTON(ID_TOOL_ERASE, EditorPanel::onToolSelect)
    EVT_BUTTON(ID_TOOL_BOAT, EditorPanel::onToolSelect)
    EVT_SLIDER(ID_BRUSH_SIZE, EditorPanel::onBrushSizeChange)
    EVT_CHECKBOX(ID_SNAP_TOGGLE, EditorPanel::onSnapToggle)
    EVT_BUTTON(ID_NEW_SCENARIO, EditorPanel::onNewScenario)
    EVT_BUTTON(ID_RESIZE_SCENARIO, EditorPanel::onResizeScenario)
    EVT_BUTTON(ID_CLEAR_SCENARIO, EditorPanel::onClearScenario)
    EVT_BUTTON(ID_FILL_WATER, EditorPanel::onFillWater)
    EVT_BUTTON(ID_FILL_LAND, EditorPanel::onFillLand)
    EVT_BUTTON(ID_UNDO, EditorPanel::onUndo)
    EVT_BUTTON(ID_REDO, EditorPanel::onRedo)
    EVT_CHOICE(ID_FIELD_TYPE, EditorPanel::onFieldTypeChange)
    EVT_SLIDER(ID_FIELD_INTENSITY, EditorPanel::onFieldParamChange)
    EVT_SPINCTRL(ID_FIELD_CENTER_X, EditorPanel::onSpinCtrlChange)
    EVT_SPINCTRL(ID_FIELD_CENTER_Y, EditorPanel::onSpinCtrlChange)
wxEND_EVENT_TABLE()

EditorPanel::EditorPanel(wxWindow* parent, MainWindow* mainWindow)
    : wxPanel(parent, wxID_ANY)
    , mainWindow_(mainWindow)
    , currentTool_(EditorTool::Select)
    , toolsToolbar_(nullptr)
    , btnSelect_(nullptr)
    , btnWater_(nullptr)
    , btnLand_(nullptr)
    , btnObstacle_(nullptr)
    , btnErase_(nullptr)
    , btnBoat_(nullptr)
    , brushSizeSlider_(nullptr)
    , snapCheck_(nullptr)
    , widthCtrl_(nullptr)
    , heightCtrl_(nullptr)
    , nameCtrl_(nullptr)
    , scenarioInfoText_(nullptr)
    , fieldTypeChoice_(nullptr)
    , fieldIntensitySlider_(nullptr)
    , fieldCenterXCtrl_(nullptr)
    , fieldCenterYCtrl_(nullptr)
{
    SetBackgroundColour(wxColour(245, 245, 245));
    setupUI();
    syncFromDocument();
    
    wxAcceleratorEntry entries[2];
    entries[0].Set(wxACCEL_CTRL, (int)'Z', ID_UNDO);
    entries[1].Set(wxACCEL_CTRL, (int)'Y', ID_REDO);
    wxAcceleratorTable accelTable(2, entries);
    SetAcceleratorTable(accelTable);
}

void EditorPanel::setupUI() {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Título
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("EDITOR DE ESCENARIOS"));
    title->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(title, 0, wxALL, 10);

    wxStaticText* subtitle = new wxStaticText(this, wxID_ANY,
        wxT("Edita el escenario directamente sobre el canvas: elige una herramienta y haz clic o arrastra sobre la escena."));
    subtitle->Wrap(360);
    subtitle->SetForegroundColour(wxColour(70, 80, 95));
    mainSizer->Add(subtitle, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    // Notebook para organizar secciones
    wxNotebook* notebook = new wxNotebook(this, wxID_ANY);
    
    // Pestaña: Herramientas
    wxPanel* toolsPanel = new wxPanel(notebook);
    wxBoxSizer* toolsSizer = new wxBoxSizer(wxVERTICAL);
    
    // Sección de herramientas de pintura
    wxStaticBoxSizer* paintBox = new wxStaticBoxSizer(wxVERTICAL, toolsPanel, wxT("Herramientas de Pintura"));
    
    wxGridSizer* toolGrid = new wxGridSizer(3, 5, 5);
    
    btnSelect_ = new wxButton(toolsPanel, ID_TOOL_SELECT, wxT("✥\nSelecciónar"), wxDefaultPosition, wxSize(70, 50));
    btnWater_ = new wxButton(toolsPanel, ID_TOOL_WATER, wxT("≈\nAgua"), wxDefaultPosition, wxSize(70, 50));
    btnWater_->SetBackgroundColour(wxColour(200, 230, 255));
    btnLand_ = new wxButton(toolsPanel, ID_TOOL_LAND, wxT("▩\nTierra"), wxDefaultPosition, wxSize(70, 50));
    btnLand_->SetBackgroundColour(wxColour(180, 160, 140));
    btnObstacle_ = new wxButton(toolsPanel, ID_TOOL_OBSTACLE, wxT("▣\nObstáculo"), wxDefaultPosition, wxSize(70, 50));
    btnObstacle_->SetBackgroundColour(wxColour(120, 120, 120));
    btnErase_ = new wxButton(toolsPanel, ID_TOOL_ERASE, wxT("⌫\nBorrar"), wxDefaultPosition, wxSize(70, 50));
    btnBoat_ = new wxButton(toolsPanel, ID_TOOL_BOAT, wxT("⛵\nBote"), wxDefaultPosition, wxSize(70, 50));
    
    toolGrid->Add(btnSelect_, 0, wxEXPAND);
    toolGrid->Add(btnWater_, 0, wxEXPAND);
    toolGrid->Add(btnLand_, 0, wxEXPAND);
    toolGrid->Add(btnObstacle_, 0, wxEXPAND);
    toolGrid->Add(btnErase_, 0, wxEXPAND);
    toolGrid->Add(btnBoat_, 0, wxEXPAND);
    
    paintBox->Add(toolGrid, 0, wxEXPAND | wxALL, 5);
    toolsSizer->Add(paintBox, 0, wxEXPAND | wxALL, 5);
    
    // Sección de pincel
    wxStaticBoxSizer* brushBox = new wxStaticBoxSizer(wxVERTICAL, toolsPanel, wxT("Pincel"));
    
    wxBoxSizer* brushSizer = new wxBoxSizer(wxHORIZONTAL);
    brushSizer->Add(new wxStaticText(toolsPanel, wxID_ANY, wxT("Tamaño:")), 0, wxALIGN_CENTER_VERTICAL);
    brushSizeSlider_ = new wxSlider(toolsPanel, ID_BRUSH_SIZE, 1, 1, 10, 
                                    wxDefaultPosition, wxSize(150, -1), wxSL_HORIZONTAL | wxSL_LABELS);
    brushSizer->Add(brushSizeSlider_, 1, wxEXPAND | wxLEFT, 5);
    brushBox->Add(brushSizer, 0, wxEXPAND | wxALL, 5);
    
    snapCheck_ = new wxCheckBox(toolsPanel, ID_SNAP_TOGGLE, wxT("Ajustar a grilla"));
    snapCheck_->SetValue(true);
    brushBox->Add(snapCheck_, 0, wxALL, 5);
    
    toolsSizer->Add(brushBox, 0, wxEXPAND | wxALL, 5);
    
    // Sección de acciones rápidas
    wxStaticBoxSizer* actionBox = new wxStaticBoxSizer(wxVERTICAL, toolsPanel, wxT("Acciones Rápidas"));
    
    wxGridSizer* actionGrid = new wxGridSizer(2, 5, 5);
    wxButton* btnFillWater = new wxButton(toolsPanel, ID_FILL_WATER, wxT("Llenar con Agua"));
    wxButton* btnFillLand = new wxButton(toolsPanel, ID_FILL_LAND, wxT("Llenar con Tierra"));
    wxButton* btnClear = new wxButton(toolsPanel, ID_CLEAR_SCENARIO, wxT("Limpiar Todo"));
    wxButton* btnUndo = new wxButton(toolsPanel, ID_UNDO, wxT("↶ Deshacer"));
    wxButton* btnRedo = new wxButton(toolsPanel, ID_REDO, wxT("↷ Rehacer"));
    
    actionGrid->Add(btnFillWater, 0, wxEXPAND);
    actionGrid->Add(btnFillLand, 0, wxEXPAND);
    actionGrid->Add(btnClear, 0, wxEXPAND);
    actionGrid->Add(btnUndo, 0, wxEXPAND);
    actionGrid->Add(btnRedo, 0, wxEXPAND);
    
    actionBox->Add(actionGrid, 0, wxEXPAND | wxALL, 5);
    toolsSizer->Add(actionBox, 0, wxEXPAND | wxALL, 5);
    
    toolsPanel->SetSizer(toolsSizer);
    notebook->AddPage(toolsPanel, wxT("Herramientas"));
    
    // Pestaña: Escenario
    wxPanel* scenarioPanel = new wxPanel(notebook);
    wxBoxSizer* scenarioSizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticBoxSizer* sizeBox = new wxStaticBoxSizer(wxVERTICAL, scenarioPanel, wxT("Dimensiones"));
    
    wxFlexGridSizer* sizeGrid = new wxFlexGridSizer(2, 5, 5);
    sizeGrid->AddGrowableCol(1);
    
    sizeGrid->Add(new wxStaticText(scenarioPanel, wxID_ANY, wxT("Nombre:")));
    nameCtrl_ = new wxTextCtrl(scenarioPanel, wxID_ANY, wxT("Mi Escenario"));
    nameCtrl_->Bind(wxEVT_TEXT, [this](wxCommandEvent& event) {
        if (!mainWindow_) {
            event.Skip();
            return;
        }

        auto& scenario = mainWindow_->getScenarioDocument().scenario();
        const std::string newName = std::string(event.GetString().ToUTF8());
        if (scenario.getName() != newName) {
            scenario.setName(newName);
            mainWindow_->setScenarioName(event.GetString());
            mainWindow_->getScenarioDocument().notifyChanged();
            mainWindow_->setModified(true);
        }
        event.Skip();
    });
    sizeGrid->Add(nameCtrl_, 1, wxEXPAND);
    
    sizeGrid->Add(new wxStaticText(scenarioPanel, wxID_ANY, wxT("Ancho:")));
    widthCtrl_ = new wxSpinCtrl(scenarioPanel, wxID_ANY, wxT("50"));
    widthCtrl_->SetRange(10, 200);
    sizeGrid->Add(widthCtrl_, 1, wxEXPAND);
    
    sizeGrid->Add(new wxStaticText(scenarioPanel, wxID_ANY, wxT("Alto:")));
    heightCtrl_ = new wxSpinCtrl(scenarioPanel, wxID_ANY, wxT("50"));
    heightCtrl_->SetRange(10, 200);
    sizeGrid->Add(heightCtrl_, 1, wxEXPAND);
    
    sizeBox->Add(sizeGrid, 0, wxEXPAND | wxALL, 5);
    
    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* btnNew = new wxButton(scenarioPanel, ID_NEW_SCENARIO, wxT("Nuevo"));
    wxButton* btnResize = new wxButton(scenarioPanel, ID_RESIZE_SCENARIO, wxT("Redimensionar"));
    btnSizer->Add(btnNew, 1, wxRIGHT, 5);
    btnSizer->Add(btnResize, 1);
    sizeBox->Add(btnSizer, 0, wxEXPAND | wxALL, 5);
    
    scenarioSizer->Add(sizeBox, 0, wxEXPAND | wxALL, 5);
    
    // Información del escenario
    wxStaticBoxSizer* infoBox = new wxStaticBoxSizer(wxVERTICAL, scenarioPanel, wxT("Información"));
    scenarioInfoText_ = new wxStaticText(scenarioPanel, wxID_ANY, wxEmptyString);
    infoBox->Add(scenarioInfoText_, 0, wxALL, 5);
    scenarioSizer->Add(infoBox, 0, wxEXPAND | wxALL, 5);
    
    scenarioPanel->SetSizer(scenarioSizer);
    notebook->AddPage(scenarioPanel, wxT("Escenario"));
    
    mainSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);
    
    SetSizer(mainSizer);
}

void EditorPanel::syncFromDocument() {
    auto* config = currentConfig();
    if (!config) {
        return;
    }

    if (widthCtrl_) widthCtrl_->SetValue(config->scenario.getWidth());
    if (heightCtrl_) heightCtrl_->SetValue(config->scenario.getHeight());
    if (nameCtrl_ && !nameCtrl_->HasFocus()) nameCtrl_->ChangeValue(wxString::FromUTF8(config->scenario.getName().c_str()));
    if (fieldTypeChoice_) fieldTypeChoice_->SetSelection(static_cast<int>(config->fieldView.type) + 1);
    if (fieldIntensitySlider_) fieldIntensitySlider_->SetValue(std::clamp(static_cast<int>(config->fieldView.intensity * 50.0), 0, 100));
    if (fieldCenterXCtrl_) fieldCenterXCtrl_->SetValue(static_cast<int>(config->fieldView.centerX));
    if (fieldCenterYCtrl_) fieldCenterYCtrl_->SetValue(static_cast<int>(config->fieldView.centerY));

    if (scenarioInfoText_) {
        int water = 0;
        int land = 0;
        int obstacles = 0;
        for (int y = 0; y < config->scenario.getHeight(); ++y) {
            for (int x = 0; x < config->scenario.getWidth(); ++x) {
                switch (config->scenario.getCell(x, y)) {
                    case tp::shared::CellType::Water: water++; break;
                    case tp::shared::CellType::Land: land++; break;
                    case tp::shared::CellType::Obstacle: obstacles++; break;
                }
            }
        }
        scenarioInfoText_->SetLabel(wxString::Format(
            wxT("Celdas de agua: %d\nCeldas de tierra: %d\nObstáculos: %d\nTamaño: %dx%d"),
            water, land, obstacles, config->scenario.getWidth(), config->scenario.getHeight()));
    }
}

void EditorPanel::setCurrentTool(EditorTool tool) {
    currentTool_ = tool;
    
    const wxColour neutralBg(245, 245, 245);
    const wxColour waterBg(214, 233, 248);
    const wxColour landBg(228, 215, 201);
    const wxColour obstacleBg(214, 214, 214);
    const wxColour accentBg(214, 240, 220);
    const wxColour normalFg(20, 20, 20);

    if (btnSelect_) { btnSelect_->SetBackgroundColour(neutralBg); btnSelect_->SetForegroundColour(normalFg); }
    if (btnWater_)  { btnWater_->SetBackgroundColour(waterBg); btnWater_->SetForegroundColour(normalFg); }
    if (btnLand_)   { btnLand_->SetBackgroundColour(landBg); btnLand_->SetForegroundColour(normalFg); }
    if (btnObstacle_) { btnObstacle_->SetBackgroundColour(obstacleBg); btnObstacle_->SetForegroundColour(normalFg); }
    if (btnErase_)  { btnErase_->SetBackgroundColour(neutralBg); btnErase_->SetForegroundColour(normalFg); }
    if (btnBoat_)   { btnBoat_->SetBackgroundColour(neutralBg); btnBoat_->SetForegroundColour(normalFg); }
    
    switch (tool) {
        case EditorTool::Select:
            if (btnSelect_) {
                btnSelect_->SetBackgroundColour(accentBg);
            }
            break;
        case EditorTool::PaintWater:
            if (btnWater_) {
                btnWater_->SetBackgroundColour(accentBg);
            }
            break;
        case EditorTool::PaintLand:
            if (btnLand_) {
                btnLand_->SetBackgroundColour(accentBg);
            }
            break;
        case EditorTool::PlaceObstacle:
            if (btnObstacle_) {
                btnObstacle_->SetBackgroundColour(accentBg);
            }
            break;
        case EditorTool::Erase:
            if (btnErase_) {
                btnErase_->SetBackgroundColour(accentBg);
            }
            break;
        case EditorTool::PlaceBoat:
            if (btnBoat_) {
                btnBoat_->SetBackgroundColour(accentBg);
            }
            break;
    }
    
    // Forzar actualizacion visual de los botones
    if (btnSelect_) btnSelect_->Refresh();
    if (btnWater_) btnWater_->Refresh();
    if (btnLand_) btnLand_->Refresh();
    if (btnObstacle_) btnObstacle_->Refresh();
    if (btnErase_) btnErase_->Refresh();
    if (btnBoat_) btnBoat_->Refresh();
}

int EditorPanel::getBrushSize() const {
    if (brushSizeSlider_) {
        return brushSizeSlider_->GetValue();
    }
    return 1;
}

bool EditorPanel::isSnapToGrid() const {
    if (snapCheck_) {
        return snapCheck_->GetValue();
    }
    return true;
}

void EditorPanel::beginStroke() {
    auto* config = currentConfig();
    if (!config) {
        return;
    }

    switch (currentTool_) {
        case EditorTool::PaintWater:
        case EditorTool::Erase:
        case EditorTool::PaintLand:
        case EditorTool::PlaceObstacle:
            activeStroke_ = std::make_unique<PaintStrokeCommand>(config->scenario);
            break;
        default:
            activeStroke_.reset();
            break;
    }
}

void EditorPanel::endStroke() {
    if (activeStroke_ && !activeStroke_->isEmpty()) {
        commandHistory_.pushExecutedCommand(std::move(activeStroke_));
        mainWindow_->getScenarioDocument().notifyChanged();
        mainWindow_->updateCanvas();
    }
    activeStroke_.reset();
}

void EditorPanel::applyToolAtCell(int x, int y) {
    auto* config = currentConfig();
    if (!config) {
        return;
    }

    if (!config->scenario.isValidPosition(x, y)) {
        return;
    }

    switch (currentTool_) {
        case EditorTool::PaintWater:
        case EditorTool::Erase:
        case EditorTool::PaintLand:
        case EditorTool::PlaceObstacle: {
            tp::shared::CellType newType = tp::shared::CellType::Water;
            if (currentTool_ == EditorTool::PaintLand) newType = tp::shared::CellType::Land;
            if (currentTool_ == EditorTool::PlaceObstacle) newType = tp::shared::CellType::Obstacle;

            const int radius = std::max(1, getBrushSize());
            bool changed = false;
            for (int cellY = y - radius + 1; cellY <= y + radius - 1; ++cellY) {
                for (int cellX = x - radius + 1; cellX <= x + radius - 1; ++cellX) {
                    if (!config->scenario.isValidPosition(cellX, cellY)) {
                        continue;
                    }
                    const double dx = static_cast<double>(cellX - x);
                    const double dy = static_cast<double>(cellY - y);
                    if ((dx * dx + dy * dy) > static_cast<double>(radius * radius)) {
                        continue;
                    }

                    const auto oldType = config->scenario.getCell(cellX, cellY);
                    if (oldType == newType) {
                        continue;
                    }

                    if (activeStroke_) {
                        activeStroke_->addCell(cellX, cellY, newType, oldType);
                        config->scenario.setCell(cellX, cellY, newType);
                    } else {
                        commandHistory_.executeCommand(
                            std::make_unique<PaintCellCommand>(config->scenario, cellX, cellY, newType, oldType));
                    }
                    changed = true;
                }
            }

            if (!changed) {
                return;
            }

            if (!activeStroke_) {
                mainWindow_->getScenarioDocument().notifyChanged();
            }
            mainWindow_->updateCanvas();
            break;
        }
        case EditorTool::PlaceBoat:
            if (mainWindow_->getScenarioDocument().placeBoat(tp::shared::Vec2d(x + 0.5, y + 0.5))) {
                mainWindow_->updateCanvas();
            }
            break;
        case EditorTool::Select:
        case EditorTool::Fill:
        case EditorTool::DefineField:
        default:
            break;
    }
}

void EditorPanel::onToolSelect(wxCommandEvent& event) {
    int id = event.GetId();
    switch (id) {
        case ID_TOOL_SELECT: setCurrentTool(EditorTool::Select); break;
        case ID_TOOL_WATER: setCurrentTool(EditorTool::PaintWater); break;
        case ID_TOOL_LAND: setCurrentTool(EditorTool::PaintLand); break;
        case ID_TOOL_OBSTACLE: setCurrentTool(EditorTool::PlaceObstacle); break;
        case ID_TOOL_ERASE: setCurrentTool(EditorTool::Erase); break;
        case ID_TOOL_BOAT: setCurrentTool(EditorTool::PlaceBoat); break;
    }
}

void EditorPanel::onBrushSizeChange(wxCommandEvent& event) {
    (void)event;
    // Actualizar tamaño del cursor en canvas
    int size = getBrushSize();
    if (mainWindow_) {
        mainWindow_->updateEditorBrushSize(size);
    }
}

void EditorPanel::onSnapToggle(wxCommandEvent& event) {
    (void)event;
    // Activar/desactivar snap en canvas
    bool snap = isSnapToGrid();
    if (mainWindow_) {
        mainWindow_->updateEditorSnap(snap);
    }
}

tp::application::ExperimentConfig* EditorPanel::currentConfig() const {
    if (!mainWindow_) {
        return nullptr;
    }
    return &mainWindow_->getScenarioDocument().config();
}

void EditorPanel::commitScenarioChange(const std::function<void(tp::application::ExperimentConfig&)>& mutation) {
    if (!mainWindow_) {
        return;
    }
    mutation(mainWindow_->getScenarioDocument().config());
    mainWindow_->getScenarioDocument().notifyChanged();
    mainWindow_->updateCanvas();
}

void EditorPanel::applyFieldControlsToConfig() {
    if (!mainWindow_) {
        return;
    }

    auto& fieldView = mainWindow_->getScenarioDocument().fieldView();
    const int selection = fieldTypeChoice_ ? fieldTypeChoice_->GetSelection() : 1;
    fieldView.type = static_cast<tp::application::FieldPresetType>(std::clamp(selection - 1, 0, 4));
    fieldView.intensity = fieldIntensitySlider_ ? fieldIntensitySlider_->GetValue() / 50.0 : fieldView.intensity;
    fieldView.centerX = fieldCenterXCtrl_ ? fieldCenterXCtrl_->GetValue() : fieldView.centerX;
    fieldView.centerY = fieldCenterYCtrl_ ? fieldCenterYCtrl_->GetValue() : fieldView.centerY;
    mainWindow_->getScenarioDocument().rebuildField();
    mainWindow_->updateCanvas();
}

void EditorPanel::onNewScenario(wxCommandEvent& event) {
    (void)event;
    int width = widthCtrl_ ? widthCtrl_->GetValue() : 50;
    int height = heightCtrl_ ? heightCtrl_->GetValue() : 50;
    wxString name = nameCtrl_ ? nameCtrl_->GetValue() : wxT("Nuevo Escenario");

    commitScenarioChange([&](tp::application::ExperimentConfig& config) {
        config.scenario = tp::domain::Scenario(width, height);
        config.scenario.setName(std::string(name.ToUTF8()));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                config.scenario.setCell(x, y, tp::shared::CellType::Water);
            }
        }
        config.boat.setPosition(tp::shared::Vec2d(width / 2.0, height / 2.0));
        config.boat.setOrientation(0.0);
    });

    if (mainWindow_) {
        mainWindow_->setScenarioName(name);
        mainWindow_->setModified(true);
        mainWindow_->requestSaveScenario();
    }
}

void EditorPanel::onResizeScenario(wxCommandEvent& event) {
    (void)event;
    int newWidth = widthCtrl_ ? widthCtrl_->GetValue() : 50;
    int newHeight = heightCtrl_ ? heightCtrl_->GetValue() : 50;

    commitScenarioChange([&](tp::application::ExperimentConfig& config) {
        tp::domain::Scenario newScenario(newWidth, newHeight);
        const wxString requestedName = nameCtrl_ ? nameCtrl_->GetValue() : wxString::FromUTF8(config.scenario.getName().c_str());
        newScenario.setName(std::string(requestedName.ToUTF8()));
        int copyWidth = std::min(newWidth, config.scenario.getWidth());
        int copyHeight = std::min(newHeight, config.scenario.getHeight());
        for (int y = 0; y < copyHeight; ++y) {
            for (int x = 0; x < copyWidth; ++x) {
                newScenario.setCell(x, y, config.scenario.getCell(x, y));
            }
        }
        for (int y = copyHeight; y < newHeight; ++y) {
            for (int x = 0; x < newWidth; ++x) {
                newScenario.setCell(x, y, tp::shared::CellType::Water);
            }
        }
        config.scenario = std::move(newScenario);
    });

    if (mainWindow_ && nameCtrl_) {
        mainWindow_->setScenarioName(nameCtrl_->GetValue());
        mainWindow_->setModified(true);
    }

    wxMessageBox(wxString::Format(wxT("Escenario redimensionado a %dx%d"), newWidth, newHeight),
                 wxT("Editor"), wxOK | wxICON_INFORMATION);
}

void EditorPanel::onClearScenario(wxCommandEvent& event) {
    (void)event;
    int result = wxMessageBox(wxT("¿Estás seguro de que quieres limpiar todo el escenario?"),
                              wxT("Confirmar"), wxYES_NO | wxICON_QUESTION);
    if (result == wxYES) {
        if (auto* config = currentConfig()) {
            commandHistory_.executeCommand(
                std::make_unique<ClearScenarioCommand>(config->scenario));
            mainWindow_->getScenarioDocument().notifyChanged();
            mainWindow_->updateCanvas();
        }
    }
}

void EditorPanel::onFillWater(wxCommandEvent& event) {
    (void)event;
    if (auto* config = currentConfig()) {
        commandHistory_.executeCommand(
            std::make_unique<FillScenarioCommand>(config->scenario, tp::shared::CellType::Water));
        mainWindow_->getScenarioDocument().notifyChanged();
        mainWindow_->updateCanvas();
    }
}

void EditorPanel::onFillLand(wxCommandEvent& event) {
    (void)event;
    if (auto* config = currentConfig()) {
        commandHistory_.executeCommand(
            std::make_unique<FillScenarioCommand>(config->scenario, tp::shared::CellType::Land));
        mainWindow_->getScenarioDocument().notifyChanged();
        mainWindow_->updateCanvas();
    }
}

void EditorPanel::onUndo(wxCommandEvent& event) {
    (void)event;
    if (commandHistory_.canUndo()) {
        commandHistory_.undo();
        if (mainWindow_) {
            mainWindow_->getScenarioDocument().notifyChanged();
            mainWindow_->updateCanvas();
        }
    }
}

void EditorPanel::onRedo(wxCommandEvent& event) {
    (void)event;
    if (commandHistory_.canRedo()) {
        commandHistory_.redo();
        if (mainWindow_) {
            mainWindow_->getScenarioDocument().notifyChanged();
            mainWindow_->updateCanvas();
        }
    }
}

void EditorPanel::onFieldTypeChange(wxCommandEvent& event) {
    (void)event;
    if (!mainWindow_ || !fieldTypeChoice_) return;
    applyFieldControlsToConfig();
}

void EditorPanel::onFieldParamChange(wxCommandEvent& event) {
    (void)event;
    applyFieldControlsToConfig();
}

void EditorPanel::onSpinCtrlChange(wxSpinEvent& event) {
    (void)event;
    applyFieldControlsToConfig();
}

}
