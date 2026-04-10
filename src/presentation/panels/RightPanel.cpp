#include "panels/RightPanel.hpp"

#include "main_window/MainWindow.hpp"
#include "main_window/MainWindowIds.hpp"
#include "main_window/MainWindowStyles.hpp"
#include "widgets/SimulationCanvas.hpp"
#include "widgets/Water3DCanvas.hpp"
#include "widgets/SimpleGraph.hpp"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/slider.h>
#include <wx/choice.h>

#include <algorithm>

namespace tp::presentation {

wxBEGIN_EVENT_TABLE(RightPanel, wxPanel)
wxEND_EVENT_TABLE()

RightPanel::RightPanel(wxWindow* parent, MainWindow* mainWindow)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME)
    , mainWindow_(mainWindow)
    , notebook_(nullptr)
    , massCtrl_(nullptr)
    , dragCtrl_(nullptr)
    , sizeCtrl_(nullptr)
    , initialXCtrl_(nullptr)
    , initialYCtrl_(nullptr)
    , initialAngleCtrl_(nullptr)
    , thrustSlider_(nullptr)
    , directionSlider_(nullptr)
    , thrustValueText_(nullptr)
    , directionValueText_(nullptr)
    , autoThrustCheck_(nullptr)
    , fieldTypeChoice_(nullptr)
    , fieldIntensitySlider_(nullptr)
    , fieldIntensityValue_(nullptr)
    , fieldCenterXCtrl_(nullptr)
    , fieldCenterYCtrl_(nullptr)
    , customFieldEditBtn_(nullptr)
    , methodChoice_(nullptr)
    , dtSlider_(nullptr)
    , durationCtrl_(nullptr)
{
    SetBackgroundColour(ModernColors::PANEL_BG);
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Título
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("PROPIEDADES"));
    title->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(title, 0, wxALL, 10);
    
    // Notebook para organizar secciones
    notebook_ = new wxNotebook(this, wxID_ANY);
    
    // Pestaña: Motor
    wxPanel* motorPanel = new wxPanel(notebook_);
    motorPanel->SetBackgroundColour(ModernColors::PANEL_BG);
    wxBoxSizer* motorSizer = new wxBoxSizer(wxVERTICAL);
    
    // Empuje con valor en tiempo real
    wxBoxSizer* thrustSizer = new wxBoxSizer(wxHORIZONTAL);
    thrustSizer->Add(new wxStaticText(motorPanel, wxID_ANY, wxT("Empuje:")), 1, wxALIGN_CENTER_VERTICAL);
    thrustValueText_ = new wxStaticText(motorPanel, wxID_ANY, wxT("5.0 N"));
    thrustValueText_->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    thrustValueText_->SetForegroundColour(ModernColors::ACCENT_BLUE);
    thrustSizer->Add(thrustValueText_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    motorSizer->Add(thrustSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    
    thrustSlider_ = new wxSlider(motorPanel, ID_THRUST_SLIDER, 50, 0, 100);
    thrustSlider_->Bind(wxEVT_SLIDER, [this](wxCommandEvent& evt) {
        double val = evt.GetInt() / 10.0;
        if (thrustValueText_) {
            thrustValueText_->SetLabel(wxString::Format(wxT("%.1f N"), val));
        }
    });
    thrustSlider_->Bind(wxEVT_SLIDER, &RightPanel::onMotorParamChange, this);
    motorSizer->Add(thrustSlider_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    // Dirección con valor en tiempo real
    wxBoxSizer* dirSizer = new wxBoxSizer(wxHORIZONTAL);
    dirSizer->Add(new wxStaticText(motorPanel, wxID_ANY, wxT("Dirección:")), 1, wxALIGN_CENTER_VERTICAL);
    directionValueText_ = new wxStaticText(motorPanel, wxID_ANY, wxT("0°"));
    directionValueText_->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    directionValueText_->SetForegroundColour(ModernColors::ACCENT_BLUE);
    dirSizer->Add(directionValueText_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    motorSizer->Add(dirSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    
    directionSlider_ = new wxSlider(motorPanel, ID_DIRECTION_SLIDER, 0, 0, 360);
    directionSlider_->Bind(wxEVT_SLIDER, [this](wxCommandEvent& evt) {
        if (directionValueText_) {
            directionValueText_->SetLabel(wxString::Format(wxT("%d°"), evt.GetInt()));
        }
    });
    directionSlider_->Bind(wxEVT_SLIDER, &RightPanel::onMotorParamChange, this);
    motorSizer->Add(directionSlider_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    motorSizer->AddStretchSpacer();
    motorPanel->SetSizer(motorSizer);
    notebook_->AddPage(motorPanel, wxT("Motor"));
    
    // Pestaña: Campo
    wxPanel* fieldPanel = new wxPanel(notebook_);
    fieldPanel->SetBackgroundColour(ModernColors::PANEL_BG);
    wxBoxSizer* fieldSizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* fieldTypeLabel = new wxStaticText(fieldPanel, wxID_ANY, wxT("📊 Tipo de campo:"));
    fieldTypeLabel->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    fieldSizer->Add(fieldTypeLabel, 0, wxLEFT | wxRIGHT | wxTOP, 10);
    
    wxString fieldTypes[] = {wxT("➡️ Uniforme"), wxT("📈 Lineal"), wxT("🎯 Radial"), wxT("🌀 Rotacional"), wxT("⚙️ Personalizado")};
    fieldTypeChoice_ = new wxChoice(fieldPanel, ID_FIELD_TYPE, wxDefaultPosition, wxDefaultSize, 5, fieldTypes);
    fieldTypeChoice_->SetSelection(0);
    fieldTypeChoice_->SetBackgroundColour(ModernColors::PANEL_BG);
    fieldTypeChoice_->Bind(wxEVT_CHOICE, &RightPanel::onFieldTypeChange, this);
    fieldSizer->Add(fieldTypeChoice_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    // Intensidad con valor en tiempo real
    wxBoxSizer* intensitySizer = new wxBoxSizer(wxHORIZONTAL);
    intensitySizer->Add(new wxStaticText(fieldPanel, wxID_ANY, wxT("Intensidad:")), 1, wxALIGN_CENTER_VERTICAL);
    fieldIntensityValue_ = new wxStaticText(fieldPanel, wxID_ANY, wxT("5.0"));
    fieldIntensityValue_->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    fieldIntensityValue_->SetForegroundColour(ModernColors::ACCENT_ORANGE);
    intensitySizer->Add(fieldIntensityValue_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    fieldSizer->Add(intensitySizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    
    fieldIntensitySlider_ = new wxSlider(fieldPanel, ID_FIELD_INTENSITY, 50, 0, 100);
    fieldIntensitySlider_->Bind(wxEVT_SLIDER, [this](wxCommandEvent& evt) {
        double val = evt.GetInt() / 10.0;
        if (fieldIntensityValue_) {
            fieldIntensityValue_->SetLabel(wxString::Format(wxT("%.1f"), val));
        }
    });
    fieldIntensitySlider_->Bind(wxEVT_SLIDER, &RightPanel::onFieldParamChange, this);
    fieldSizer->Add(fieldIntensitySlider_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    wxFlexGridSizer* centerGrid = new wxFlexGridSizer(2, 5, 5);
    centerGrid->AddGrowableCol(1);
    centerGrid->Add(new wxStaticText(fieldPanel, wxID_ANY, wxT("Centro X:")));
    fieldCenterXCtrl_ = new wxSpinCtrl(fieldPanel, wxID_ANY, wxT("25"));
    fieldCenterXCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onFieldParamChange, this);
    centerGrid->Add(fieldCenterXCtrl_, 1, wxEXPAND);
    centerGrid->Add(new wxStaticText(fieldPanel, wxID_ANY, wxT("Centro Y:")));
    fieldCenterYCtrl_ = new wxSpinCtrl(fieldPanel, wxID_ANY, wxT("25"));
    fieldCenterYCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onFieldParamChange, this);
    centerGrid->Add(fieldCenterYCtrl_, 1, wxEXPAND);
    fieldSizer->Add(centerGrid, 0, wxEXPAND | wxALL, 5);

    customFieldEditBtn_ = new wxButton(fieldPanel, wxID_ANY, wxT("Editar campo personalizado..."));
    customFieldEditBtn_->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        if (mainWindow_) {
            mainWindow_->openCustomFieldDialog();
        }
    });
    fieldSizer->Add(customFieldEditBtn_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    fieldSizer->AddStretchSpacer();
    fieldPanel->SetSizer(fieldSizer);
    notebook_->AddPage(fieldPanel, wxT("Campo"));
    
    // Pestaña: Bote
    wxPanel* boatPanel = new wxPanel(notebook_);
    wxBoxSizer* boatSizer = new wxBoxSizer(wxVERTICAL);
    
    wxFlexGridSizer* boatGrid = new wxFlexGridSizer(2, 5, 5);
    boatGrid->AddGrowableCol(1);
    
    boatGrid->Add(new wxStaticText(boatPanel, wxID_ANY, wxT("Masa (kg):")));
    massCtrl_ = new wxSpinCtrl(boatPanel, wxID_ANY, wxT("10"));
    massCtrl_->SetRange(1, 1000);
    massCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onBoatParamChange, this);
    boatGrid->Add(massCtrl_, 1, wxEXPAND);
    
    boatGrid->Add(new wxStaticText(boatPanel, wxID_ANY, wxT("Arrastre:")));
    dragCtrl_ = new wxSpinCtrl(boatPanel, wxID_ANY, wxT("100"));
    dragCtrl_->SetRange(0, 1000);
    dragCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onBoatParamChange, this);
    boatGrid->Add(dragCtrl_, 1, wxEXPAND);
    
    boatGrid->Add(new wxStaticText(boatPanel, wxID_ANY, wxT("Tamaño:")));
    sizeCtrl_ = new wxSpinCtrl(boatPanel, wxID_ANY, wxT("5"));
    sizeCtrl_->SetRange(1, 20);
    sizeCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onBoatParamChange, this);
    boatGrid->Add(sizeCtrl_, 1, wxEXPAND);
    
    boatGrid->Add(new wxStaticText(boatPanel, wxID_ANY, wxT("Pos. Inicial X:")));
    initialXCtrl_ = new wxSpinCtrl(boatPanel, wxID_ANY, wxT("10"));
    initialXCtrl_->SetRange(0, 100);
    initialXCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onBoatParamChange, this);
    boatGrid->Add(initialXCtrl_, 1, wxEXPAND);
    
    boatGrid->Add(new wxStaticText(boatPanel, wxID_ANY, wxT("Pos. Inicial Y:")));
    initialYCtrl_ = new wxSpinCtrl(boatPanel, wxID_ANY, wxT("25"));
    initialYCtrl_->SetRange(0, 100);
    initialYCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onBoatParamChange, this);
    boatGrid->Add(initialYCtrl_, 1, wxEXPAND);
    
    boatGrid->Add(new wxStaticText(boatPanel, wxID_ANY, wxT("Ángulo inicial:")));
    initialAngleCtrl_ = new wxSpinCtrl(boatPanel, wxID_ANY, wxT("0"));
    initialAngleCtrl_->SetRange(0, 360);
    initialAngleCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onBoatParamChange, this);
    boatGrid->Add(initialAngleCtrl_, 1, wxEXPAND);
    
    boatSizer->Add(boatGrid, 0, wxEXPAND | wxALL, 10);
    boatSizer->AddStretchSpacer();
    boatPanel->SetSizer(boatSizer);
    notebook_->AddPage(boatPanel, wxT("Bote"));
    
    // Pestaña: Numérico
    wxPanel* numPanel = new wxPanel(notebook_);
    numPanel->SetBackgroundColour(ModernColors::PANEL_BG);
    wxBoxSizer* numSizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* methodLabel = new wxStaticText(numPanel, wxID_ANY, wxT("🔢 Método de integración:"));
    methodLabel->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    numSizer->Add(methodLabel, 0, wxLEFT | wxRIGHT | wxTOP, 10);
    
    wxString methods[] = {wxT("📐 Euler (rápido)"), wxT("📐 Euler Mejorado"), wxT("📐 RK4 (preciso)")};
    methodChoice_ = new wxChoice(numPanel, ID_METHOD_CHOICE, wxDefaultPosition, wxDefaultSize, 3, methods);
    methodChoice_->SetSelection(0);
    methodChoice_->SetBackgroundColour(ModernColors::PANEL_BG);
    methodChoice_->Bind(wxEVT_CHOICE, &RightPanel::onNumericalParamChange, this);
    numSizer->Add(methodChoice_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    // Paso temporal con valor en tiempo real
    wxBoxSizer* dtSizer = new wxBoxSizer(wxHORIZONTAL);
    dtSizer->Add(new wxStaticText(numPanel, wxID_ANY, wxT("⏱️ Paso temporal:")), 1, wxALIGN_CENTER_VERTICAL);
    wxStaticText* dtValue = new wxStaticText(numPanel, wxID_ANY, wxT("33 ms"));
    dtValue->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    dtValue->SetForegroundColour(ModernColors::ACCENT_GREEN);
    dtSizer->Add(dtValue, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    numSizer->Add(dtSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    
    dtSlider_ = new wxSlider(numPanel, ID_DT_SLIDER, 33, 10, 100);
    dtSlider_->Bind(wxEVT_SLIDER, [dtValue](wxCommandEvent& evt) {
        dtValue->SetLabel(wxString::Format(wxT("%d ms"), evt.GetInt()));
    });
    dtSlider_->Bind(wxEVT_SLIDER, &RightPanel::onNumericalParamChange, this);
    numSizer->Add(dtSlider_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    wxBoxSizer* durationSizer = new wxBoxSizer(wxHORIZONTAL);
    durationSizer->Add(new wxStaticText(numPanel, wxID_ANY, wxT("⏳ Duración máxima:")), 1, wxALIGN_CENTER_VERTICAL);
    numSizer->Add(durationSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    
    durationCtrl_ = new wxSpinCtrl(numPanel, wxID_ANY, wxT("60"));
    durationCtrl_->SetRange(1, 3600);
    durationCtrl_->SetBackgroundColour(ModernColors::PANEL_BG);
    durationCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onDurationChange, this);
    numSizer->Add(durationCtrl_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    // Info box
    wxStaticText* infoText = new wxStaticText(numPanel, wxID_ANY, 
        wxT("💡 Tip: RK4 es más preciso pero más lento.\n   Euler es rápido pero puede acumular error."));
    infoText->SetForegroundColour(ModernColors::TEXT_SECONDARY);
    infoText->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL));
    numSizer->Add(infoText, 0, wxALL, 10);
    
    numSizer->AddStretchSpacer();
    numPanel->SetSizer(numSizer);
    notebook_->AddPage(numPanel, wxT("Numérico"));
    
    // Pestaña: Escenario
    wxPanel* scenarioTabPanel = new wxPanel(notebook_);
    scenarioTabPanel->SetBackgroundColour(ModernColors::PANEL_BG);
    wxBoxSizer* scenarioTabSizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* scenarioTitle = new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("🗺️ Escenario Actual"));
    scenarioTitle->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    scenarioTabSizer->Add(scenarioTitle, 0, wxLEFT | wxRIGHT | wxTOP, 10);
    
    wxFlexGridSizer* scenarioGrid = new wxFlexGridSizer(2, 5, 5);
    scenarioGrid->AddGrowableCol(1);
    
    scenarioGrid->Add(new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("Ancho:")));
    wxStaticText* scenarioWidthText = new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("50"));
    scenarioWidthText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    scenarioWidthText->SetForegroundColour(ModernColors::ACCENT_BLUE);
    scenarioGrid->Add(scenarioWidthText, 1, wxEXPAND);
    
    scenarioGrid->Add(new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("Alto:")));
    wxStaticText* scenarioHeightText = new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("50"));
    scenarioHeightText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    scenarioHeightText->SetForegroundColour(ModernColors::ACCENT_BLUE);
    scenarioGrid->Add(scenarioHeightText, 1, wxEXPAND);
    
    scenarioGrid->Add(new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("Celdas agua:")));
    wxStaticText* waterCellsText = new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("2500"));
    waterCellsText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    waterCellsText->SetForegroundColour(ModernColors::ACCENT_GREEN);
    scenarioGrid->Add(waterCellsText, 1, wxEXPAND);
    
    scenarioGrid->Add(new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("Celdas tierra:")));
    wxStaticText* landCellsText = new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("0"));
    landCellsText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    landCellsText->SetForegroundColour(ModernColors::ACCENT_ORANGE);
    scenarioGrid->Add(landCellsText, 1, wxEXPAND);
    
    scenarioGrid->Add(new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("Obstáculos:")));
    wxStaticText* obstaclesText = new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("0"));
    obstaclesText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    obstaclesText->SetForegroundColour(ModernColors::ACCENT_RED);
    scenarioGrid->Add(obstaclesText, 1, wxEXPAND);
    
    scenarioTabSizer->Add(scenarioGrid, 0, wxEXPAND | wxALL, 10);
    
    wxStaticText* scenarioInfo = new wxStaticText(scenarioTabPanel, wxID_ANY, 
        wxT("💡 Usa el Editor de escenario (F3) para modificar el mapa\ngráficamente."));
    scenarioInfo->SetForegroundColour(ModernColors::TEXT_SECONDARY);
    scenarioInfo->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL));
    scenarioTabSizer->Add(scenarioInfo, 0, wxALL, 10);
    
    scenarioTabSizer->AddStretchSpacer();
    scenarioTabPanel->SetSizer(scenarioTabSizer);
    notebook_->AddPage(scenarioTabPanel, wxT("Escenario"));
    
    mainSizer->Add(notebook_, 1, wxEXPAND | wxALL, 5);
    
    SetSizer(mainSizer);
}

void RightPanel::updateProperties()
{
    if (!mainWindow_) return;
    
    auto& config = mainWindow_->getExperimentService().getConfig();
    
    // Sincronizar controles del bote
    if (massCtrl_) massCtrl_->SetValue(static_cast<int>(config.boat.getMass()));
    if (dragCtrl_) dragCtrl_->SetValue(static_cast<int>(config.boat.getDragCoefficient() * 100));
    if (sizeCtrl_) sizeCtrl_->SetValue(static_cast<int>(config.boat.getSize()));
    if (initialXCtrl_) initialXCtrl_->SetValue(static_cast<int>(config.boat.getPosition().x));
    if (initialYCtrl_) initialYCtrl_->SetValue(static_cast<int>(config.boat.getPosition().y));
    if (initialAngleCtrl_) initialAngleCtrl_->SetValue(static_cast<int>(config.boat.getOrientation() * 180 / 3.14159));
    
    // Sincronizar controles del motor
    if (thrustSlider_) {
        double thrust = config.motor.getThrustMagnitude(0.0);
        thrustSlider_->SetValue(std::clamp(static_cast<int>(thrust * 10), 0, 100));
        if (thrustValueText_) {
            thrustValueText_->SetLabel(wxString::Format(wxT("%.1f N"), thrust));
        }
    }
    if (directionSlider_) {
        double dir = config.motor.getDirection();
        directionSlider_->SetValue(static_cast<int>(dir * 180 / 3.14159));
        if (directionValueText_) {
            directionValueText_->SetLabel(wxString::Format(wxT("%d°"), static_cast<int>(dir * 180 / 3.14159)));
        }
    }
    
    syncFieldControlsFromConfig();
    
    // Sincronizar controles numéricos
    if (dtSlider_) {
        dtSlider_->SetValue(static_cast<int>(config.timeStep * 1000));
    }
    if (durationCtrl_) {
        durationCtrl_->SetValue(static_cast<int>(config.maxTime));
    }
}

void RightPanel::syncFieldControlsFromConfig()
{
    if (!fieldTypeChoice_ || !fieldIntensitySlider_ || !mainWindow_) {
        return;
    }

    auto& config = mainWindow_->getExperimentService().getConfig();
    fieldTypeChoice_->SetSelection(static_cast<int>(config.fieldView.type));
    fieldIntensitySlider_->SetValue(std::clamp(static_cast<int>(config.fieldView.intensity * 10.0), 0, 100));
    if (fieldIntensityValue_) {
        fieldIntensityValue_->SetLabel(wxString::Format(wxT("%.1f"), config.fieldView.intensity));
    }
    if (fieldCenterXCtrl_) {
        fieldCenterXCtrl_->SetValue(static_cast<int>(config.fieldView.centerX));
    }
    if (fieldCenterYCtrl_) {
        fieldCenterYCtrl_->SetValue(static_cast<int>(config.fieldView.centerY));
    }
    updateFieldControlAvailability();
}

void RightPanel::updateFieldControlAvailability()
{
    if (!mainWindow_) {
        return;
    }

    const bool isCustom = mainWindow_->getExperimentService().getConfig().fieldView.type == tp::application::FieldPresetType::Custom;
    if (fieldIntensitySlider_) fieldIntensitySlider_->Enable(!isCustom);
    if (fieldCenterXCtrl_) fieldCenterXCtrl_->Enable(!isCustom);
    if (fieldCenterYCtrl_) fieldCenterYCtrl_->Enable(!isCustom);
    if (fieldIntensityValue_) {
        fieldIntensityValue_->SetForegroundColour(isCustom ? wxColour(120, 120, 120) : ModernColors::ACCENT_ORANGE);
        if (isCustom) {
            fieldIntensityValue_->SetLabel(wxT("n/a"));
        }
    }
    if (customFieldEditBtn_) {
        customFieldEditBtn_->Show(isCustom);
    }
    Layout();
}

void RightPanel::showBoatProperties()
{
    if (notebook_) {
        notebook_->SetSelection(2); // Pestaña Bote
    }
}

void RightPanel::showMotorProperties()
{
    if (notebook_) {
        notebook_->SetSelection(0); // Pestaña Motor
    }
}

void RightPanel::showFieldProperties()
{
    if (notebook_) {
        notebook_->SetSelection(1); // Pestaña Campo
    }
}

void RightPanel::showScenarioProperties()
{
    if (notebook_) {
        notebook_->SetSelection(4); // Pestaña Escenario
    }
}

double RightPanel::getThrustValue() const
{
    if (thrustSlider_) {
        return thrustSlider_->GetValue() / 10.0;
    }
    return 5.0;
}

double RightPanel::getDirectionValue() const
{
    if (directionSlider_) {
        return directionSlider_->GetValue() * 3.14159 / 180.0;
    }
    return 0.0;
}

int RightPanel::getFieldType() const
{
    if (fieldTypeChoice_) {
        return fieldTypeChoice_->GetSelection();
    }
    return 0;
}

double RightPanel::getFieldIntensity() const
{
    if (fieldIntensitySlider_) {
        return fieldIntensitySlider_->GetValue() / 10.0;
    }
    return 5.0;
}

int RightPanel::getMethod() const
{
    if (methodChoice_) {
        return methodChoice_->GetSelection();
    }
    return 0;
}

double RightPanel::getTimeStep() const
{
    if (dtSlider_) {
        return dtSlider_->GetValue() / 1000.0;
    }
    return 0.033;
}

void RightPanel::onBoatParamChange(wxSpinEvent& event)
{
    (void)event;
    updateBoatFromUI();
}

void RightPanel::onMotorParamChange(wxCommandEvent& event)
{
    (void)event;
    updateMotorFromUI();
}

void RightPanel::onFieldTypeChange(wxCommandEvent& event)
{
    (void)event;
    updateFieldFromUI();
}

void RightPanel::onFieldParamChange(wxCommandEvent& event)
{
    (void)event;
    updateFieldFromUI();
}

void RightPanel::onNumericalParamChange(wxCommandEvent& event)
{
    (void)event;
    updateNumericalFromUI();
}

void RightPanel::onDurationChange(wxSpinEvent& event)
{
    (void)event;
    updateNumericalFromUI();
}

void RightPanel::updateBoatFromUI()
{
    if (!mainWindow_) return;
    
    auto& config = mainWindow_->getExperimentService().getConfig();
    
    // Actualizar masa
    if (massCtrl_) {
        config.boat.setMass(massCtrl_->GetValue());
    }
    
    // Actualizar arrastre
    if (dragCtrl_) {
        config.boat.setDragCoefficient(dragCtrl_->GetValue() / 100.0);
    }
    
    // Actualizar tamaño
    if (sizeCtrl_) {
        config.boat.setSize(sizeCtrl_->GetValue());
    }
    
    // Actualizar posición inicial
    double x = initialXCtrl_ ? initialXCtrl_->GetValue() : 10.0;
    double y = initialYCtrl_ ? initialYCtrl_->GetValue() : 25.0;
    config.boat.setPosition(tp::shared::Vec2d(x, y));
    
    // Actualizar ángulo inicial
    if (initialAngleCtrl_) {
        double angle = initialAngleCtrl_->GetValue() * 3.14159 / 180.0;
        config.boat.setOrientation(angle);
    }
    
    refreshScenePreview(true);
}

void RightPanel::updateMotorFromUI()
{
    if (!mainWindow_) return;
    
    auto& config = mainWindow_->getExperimentService().getConfig();
    
    double thrust = thrustSlider_ ? thrustSlider_->GetValue() / 10.0 : 5.0;
    double direction = directionSlider_ ? directionSlider_->GetValue() * 3.14159 / 180.0 : 0.0;

    if (thrustValueText_) {
        thrustValueText_->SetLabel(wxString::Format(wxT("%.1f N"), thrust));
    }
    if (directionValueText_) {
        directionValueText_->SetLabel(wxString::Format(wxT("%d°"), static_cast<int>(direction * 180.0 / 3.14159)));
    }
    
    config.motor = tp::domain::Motor::constant(thrust, direction);
    config.boat.setOrientation(direction);

    refreshScenePreview(true);
}

void RightPanel::updateFieldFromUI()
{
    if (!mainWindow_) return;
    
    auto& service = mainWindow_->getExperimentService();
    auto& config = service.getConfig();
    
    int fieldType = fieldTypeChoice_ ? fieldTypeChoice_->GetSelection() : 0;
    double intensity = fieldIntensitySlider_ ? fieldIntensitySlider_->GetValue() / 10.0 : 5.0;
    double centerX = fieldCenterXCtrl_ ? fieldCenterXCtrl_->GetValue() : 25.0;
    double centerY = fieldCenterYCtrl_ ? fieldCenterYCtrl_->GetValue() : 25.0;

    if (fieldType == static_cast<int>(tp::application::FieldPresetType::Custom)) {
        updateFieldControlAvailability();
        return;
    }

    config.fieldView.type = static_cast<tp::application::FieldPresetType>(fieldType);
    config.fieldView.intensity = intensity;
    config.fieldView.centerX = centerX;
    config.fieldView.centerY = centerY;

    service.rebuildFieldFromView();
    updateFieldControlAvailability();
    refreshScenePreview(true);
}

void RightPanel::updateNumericalFromUI()
{
    if (!mainWindow_) return;
    
    auto& config = mainWindow_->getExperimentService().getConfig();
    
    // Actualizar método
    if (methodChoice_) {
        int method = methodChoice_->GetSelection();
        switch (method) {
            case 0: config.method = tp::shared::IntegrationMethod::Euler; break;
            case 1: config.method = tp::shared::IntegrationMethod::ImprovedEuler; break;
            case 2: config.method = tp::shared::IntegrationMethod::RK4; break;
        }
    }
    
    // Actualizar paso temporal (convertir ms a segundos)
    if (dtSlider_) {
        config.timeStep = dtSlider_->GetValue() / 1000.0;
    }
    
    // Actualizar duración máxima
    if (durationCtrl_) {
        config.maxTime = durationCtrl_->GetValue();
    }
    
    refreshScenePreview(false);
}

void RightPanel::refreshScenePreview(bool syncProperties)
{
    if (!mainWindow_) return;

    mainWindow_->getExperimentService().notifyConfigChange();
    mainWindow_->updateCanvas();

    if (syncProperties) {
        updateProperties();
    }
}


} // namespace tp::presentation
