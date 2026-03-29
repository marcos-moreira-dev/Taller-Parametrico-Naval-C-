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
    , fieldTypeChoice_(nullptr)
    , fieldIntensitySlider_(nullptr)
    , fieldCenterXCtrl_(nullptr)
    , fieldCenterYCtrl_(nullptr)
{
    SetBackgroundColour(wxColour(245, 245, 245));
    setupUI();
    
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
    infoBox->Add(new wxStaticText(scenarioPanel, wxID_ANY, 
        wxT("Celdas de agua: 0\nCeldas de tierra: 0\nObstáculos: 0\nTamaño: 50x50")), 
        0, wxALL, 5);
    scenarioSizer->Add(infoBox, 0, wxEXPAND | wxALL, 5);
    
    scenarioPanel->SetSizer(scenarioSizer);
    notebook->AddPage(scenarioPanel, wxT("Escenario"));
    
    // Pestaña: Campo
    wxPanel* fieldPanel = new wxPanel(notebook);
    wxBoxSizer* fieldSizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticBoxSizer* fieldTypeBox = new wxStaticBoxSizer(wxVERTICAL, fieldPanel, wxT("Tipo de Campo"));
    
    wxString fieldTypes[] = {
        wxT("Ninguno"),
        wxT("Uniforme"),
        wxT("Lineal"),
        wxT("Radial"),
        wxT("Rotacional"),
        wxT("Personalizado")
    };
    fieldTypeChoice_ = new wxChoice(fieldPanel, ID_FIELD_TYPE, wxDefaultPosition, wxDefaultSize, 6, fieldTypes);
    fieldTypeChoice_->SetSelection(1); // Uniforme por defecto
    fieldTypeBox->Add(fieldTypeChoice_, 0, wxEXPAND | wxALL, 5);
    
    fieldSizer->Add(fieldTypeBox, 0, wxEXPAND | wxALL, 5);
    
    wxStaticBoxSizer* fieldParamsBox = new wxStaticBoxSizer(wxVERTICAL, fieldPanel, wxT("Parámetros"));
    
    wxBoxSizer* intensitySizer = new wxBoxSizer(wxHORIZONTAL);
    intensitySizer->Add(new wxStaticText(fieldPanel, wxID_ANY, wxT("Intensidad:")), 0, wxALIGN_CENTER_VERTICAL);
    fieldIntensitySlider_ = new wxSlider(fieldPanel, ID_FIELD_INTENSITY, 50, 0, 100,
                                         wxDefaultPosition, wxSize(150, -1), wxSL_HORIZONTAL | wxSL_LABELS);
    intensitySizer->Add(fieldIntensitySlider_, 1, wxEXPAND | wxLEFT, 5);
    fieldParamsBox->Add(intensitySizer, 0, wxEXPAND | wxALL, 5);
    
    wxFlexGridSizer* centerGrid = new wxFlexGridSizer(2, 5, 5);
    centerGrid->AddGrowableCol(1);
    centerGrid->Add(new wxStaticText(fieldPanel, wxID_ANY, wxT("Centro X:")));
    fieldCenterXCtrl_ = new wxSpinCtrl(fieldPanel, ID_FIELD_CENTER_X, wxT("25"));
    fieldCenterXCtrl_->SetRange(0, 200);
    centerGrid->Add(fieldCenterXCtrl_, 1, wxEXPAND);
    centerGrid->Add(new wxStaticText(fieldPanel, wxID_ANY, wxT("Centro Y:")));
    fieldCenterYCtrl_ = new wxSpinCtrl(fieldPanel, ID_FIELD_CENTER_Y, wxT("25"));
    fieldCenterYCtrl_->SetRange(0, 200);
    centerGrid->Add(fieldCenterYCtrl_, 1, wxEXPAND);
    fieldParamsBox->Add(centerGrid, 0, wxEXPAND | wxALL, 5);
    
    fieldSizer->Add(fieldParamsBox, 0, wxEXPAND | wxALL, 5);
    
    // Vista previa del campo
    wxStaticBoxSizer* previewBox = new wxStaticBoxSizer(wxVERTICAL, fieldPanel, wxT("Vista Previa"));
    previewBox->Add(new wxStaticText(fieldPanel, wxID_ANY, 
        wxT("La vista previa se muestra en el canvas principal")), 0, wxALL, 5);
    fieldSizer->Add(previewBox, 0, wxEXPAND | wxALL, 5);
    
    fieldPanel->SetSizer(fieldSizer);
    notebook->AddPage(fieldPanel, wxT("Campo"));
    
    mainSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);
    
    SetSizer(mainSizer);
}

void EditorPanel::setCurrentTool(EditorTool tool) {
    currentTool_ = tool;
    
    wxColour defaultBg(240, 240, 240);
    wxColour selectedBg(100, 149, 237);
    wxColour selectedFg(*wxWHITE);
    
    if (btnSelect_) btnSelect_->SetBackgroundColour(defaultBg);
    if (btnWater_) btnWater_->SetBackgroundColour(defaultBg);
    if (btnLand_) btnLand_->SetBackgroundColour(defaultBg);
    if (btnObstacle_) btnObstacle_->SetBackgroundColour(defaultBg);
    if (btnErase_) btnErase_->SetBackgroundColour(defaultBg);
    if (btnBoat_) btnBoat_->SetBackgroundColour(defaultBg);
    
    switch (tool) {
        case EditorTool::Select:
            if (btnSelect_) {
                btnSelect_->SetBackgroundColour(selectedBg);
                btnSelect_->SetForegroundColour(selectedFg);
            }
            break;
        case EditorTool::PaintWater:
            if (btnWater_) {
                btnWater_->SetBackgroundColour(selectedBg);
                btnWater_->SetForegroundColour(selectedFg);
            }
            break;
        case EditorTool::PaintLand:
            if (btnLand_) {
                btnLand_->SetBackgroundColour(selectedBg);
                btnLand_->SetForegroundColour(selectedFg);
            }
            break;
        case EditorTool::PlaceObstacle:
            if (btnObstacle_) {
                btnObstacle_->SetBackgroundColour(selectedBg);
                btnObstacle_->SetForegroundColour(selectedFg);
            }
            break;
        case EditorTool::Erase:
            if (btnErase_) {
                btnErase_->SetBackgroundColour(selectedBg);
                btnErase_->SetForegroundColour(selectedFg);
            }
            break;
        case EditorTool::PlaceBoat:
            if (btnBoat_) {
                btnBoat_->SetBackgroundColour(selectedBg);
                btnBoat_->SetForegroundColour(selectedFg);
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

void EditorPanel::onNewScenario(wxCommandEvent& event) {
    (void)event;
    if (mainWindow_) {
        int width = widthCtrl_ ? widthCtrl_->GetValue() : 50;
        int height = heightCtrl_ ? heightCtrl_->GetValue() : 50;
        wxString name = nameCtrl_ ? nameCtrl_->GetValue() : wxT("Nuevo Escenario");
        
        auto& config = mainWindow_->getExperimentService().getConfig();
        
        config.scenario = tp::domain::Scenario(width, height);
        
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                config.scenario.setCell(x, y, tp::shared::CellType::Water);
            }
        }
        
        config.boat.setPosition(tp::shared::Vec2d(width / 2.0, height / 2.0));
        config.boat.setOrientation(0.0);
        
        wxMessageBox(wxString::Format(wxT("Nuevo escenario '%s' creado (%dx%d)"), name, width, height),
                     wxT("Editor"), wxOK | wxICON_INFORMATION);
        
        mainWindow_->updateCanvas();
    }
}

void EditorPanel::onResizeScenario(wxCommandEvent& event) {
    (void)event;
    if (mainWindow_) {
        int newWidth = widthCtrl_ ? widthCtrl_->GetValue() : 50;
        int newHeight = heightCtrl_ ? heightCtrl_->GetValue() : 50;
        
        auto& config = mainWindow_->getExperimentService().getConfig();
        
        tp::domain::Scenario newScenario(newWidth, newHeight);
        
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
        
        wxMessageBox(wxString::Format(wxT("Escenario redimensionado a %dx%d"), newWidth, newHeight),
                     wxT("Editor"), wxOK | wxICON_INFORMATION);
        
        mainWindow_->updateCanvas();
    }
}

void EditorPanel::onClearScenario(wxCommandEvent& event) {
    (void)event;
    int result = wxMessageBox(wxT("¿Estás seguro de que quieres limpiar todo el escenario?"),
                              wxT("Confirmar"), wxYES_NO | wxICON_QUESTION);
    if (result == wxYES) {
        if (mainWindow_) {
            auto& config = mainWindow_->getExperimentService().getConfig();
            commandHistory_.executeCommand(
                std::make_unique<ClearScenarioCommand>(config.scenario));
            mainWindow_->updateCanvas();
        }
    }
}

void EditorPanel::onFillWater(wxCommandEvent& event) {
    (void)event;
    if (mainWindow_) {
        auto& config = mainWindow_->getExperimentService().getConfig();
        commandHistory_.executeCommand(
            std::make_unique<FillScenarioCommand>(config.scenario, tp::shared::CellType::Water));
        mainWindow_->updateCanvas();
    }
}

void EditorPanel::onFillLand(wxCommandEvent& event) {
    (void)event;
    if (mainWindow_) {
        auto& config = mainWindow_->getExperimentService().getConfig();
        commandHistory_.executeCommand(
            std::make_unique<FillScenarioCommand>(config.scenario, tp::shared::CellType::Land));
        mainWindow_->updateCanvas();
    }
}

void EditorPanel::onUndo(wxCommandEvent& event) {
    (void)event;
    if (commandHistory_.canUndo()) {
        commandHistory_.undo();
        // Refresh canvas
        if (mainWindow_) {
            mainWindow_->updateCanvas();
        }
    }
}

void EditorPanel::onRedo(wxCommandEvent& event) {
    (void)event;
    if (commandHistory_.canRedo()) {
        commandHistory_.redo();
        // Refresh canvas
        if (mainWindow_) {
            mainWindow_->updateCanvas();
        }
    }
}

void EditorPanel::onFieldTypeChange(wxCommandEvent& event) {
    (void)event;
    // Actualizar tipo de campo
    if (mainWindow_ && fieldTypeChoice_) {
        int selection = fieldTypeChoice_->GetSelection();
        
        auto& config = mainWindow_->getExperimentService().getConfig();
        
        // Aplicar el tipo de campo seleccionado
        switch (selection) {
            case 0: // Ninguno
                config.field = tp::domain::VectorField([](double, double) { 
                    return tp::shared::Vec2d(0, 0); 
                });
                break;
            case 1: // Uniforme
                config.field = tp::domain::VectorField::uniform(1.0, 0.0);
                break;
            case 2: // Lineal
                config.field = tp::domain::VectorField::linear(0.1, 0.0, 0.0, 0.1);
                break;
            case 3: // Radial
                if (fieldCenterXCtrl_ && fieldCenterYCtrl_) {
                    double cx = fieldCenterXCtrl_->GetValue();
                    double cy = fieldCenterYCtrl_->GetValue();
                    config.field = tp::domain::VectorField::radial(cx, cy, 1.0);
                }
                break;
            case 4: // Rotacional
                if (fieldCenterXCtrl_ && fieldCenterYCtrl_) {
                    double cx = fieldCenterXCtrl_->GetValue();
                    double cy = fieldCenterYCtrl_->GetValue();
                    config.field = tp::domain::VectorField::rotational(cx, cy, 1.0);
                }
                break;
            case 5: // Personalizado - mantener el actual
                break;
            default:
                break;
        }
        
        mainWindow_->updateCanvas();
    }
}

void EditorPanel::onFieldParamChange(wxCommandEvent& event) {
    (void)event;
    // Actualizar parámetros del campo (intensidad, centro)
    if (mainWindow_) {
        auto& config = mainWindow_->getExperimentService().getConfig();
        
        // Obtener valores actuales
        double intensity = fieldIntensitySlider_ ? fieldIntensitySlider_->GetValue() / 50.0 : 1.0;
        double centerX = fieldCenterXCtrl_ ? fieldCenterXCtrl_->GetValue() : 25.0;
        double centerY = fieldCenterYCtrl_ ? fieldCenterYCtrl_->GetValue() : 25.0;
        
        int fieldType = fieldTypeChoice_ ? fieldTypeChoice_->GetSelection() : 1;
        
        // Recrear el campo con los nuevos parámetros
        switch (fieldType) {
            case 0: // Ninguno
                config.field = tp::domain::VectorField([](double, double) { 
                    return tp::shared::Vec2d(0, 0); 
                });
                break;
            case 1: // Uniforme
                config.field = tp::domain::VectorField::uniform(intensity, 0.0);
                break;
            case 2: // Lineal
                config.field = tp::domain::VectorField::linear(
                    intensity * 0.1, 0.0, 0.0, intensity * 0.1);
                break;
            case 3: // Radial
                config.field = tp::domain::VectorField::radial(centerX, centerY, intensity);
                break;
            case 4: // Rotacional
                config.field = tp::domain::VectorField::rotational(centerX, centerY, intensity);
                break;
            default:
                break;
        }
        
        mainWindow_->updateCanvas();
    }
}

void EditorPanel::onSpinCtrlChange(wxSpinEvent& event) {
    (void)event;
    // Actualizar parámetros del campo desde SpinCtrl (centro X, centro Y)
    if (mainWindow_) {
        auto& config = mainWindow_->getExperimentService().getConfig();
        
        double centerX = fieldCenterXCtrl_ ? fieldCenterXCtrl_->GetValue() : 25.0;
        double centerY = fieldCenterYCtrl_ ? fieldCenterYCtrl_->GetValue() : 25.0;
        double intensity = fieldIntensitySlider_ ? fieldIntensitySlider_->GetValue() / 50.0 : 1.0;
        
        int fieldType = fieldTypeChoice_ ? fieldTypeChoice_->GetSelection() : 1;
        
        // Recrear el campo con los nuevos parámetros de centro
        switch (fieldType) {
            case 3: // Radial
                config.field = tp::domain::VectorField::radial(centerX, centerY, intensity);
                break;
            case 4: // Rotacional
                config.field = tp::domain::VectorField::rotational(centerX, centerY, intensity);
                break;
            default:
                break;
        }
        
        mainWindow_->updateCanvas();
    }
}

}
