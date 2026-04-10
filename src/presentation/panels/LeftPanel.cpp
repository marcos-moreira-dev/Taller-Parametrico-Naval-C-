#include "panels/LeftPanel.hpp"

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

wxBEGIN_EVENT_TABLE(LeftPanel, wxPanel)
    EVT_LISTBOX(ID_SCENARIO_LIST, LeftPanel::onScenarioSelect)
    EVT_CHECKLISTBOX(wxID_ANY, LeftPanel::onLayerToggle)
wxEND_EVENT_TABLE()

LeftPanel::LeftPanel(wxWindow* parent, MainWindow* mainWindow)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME)
    , mainWindow_(mainWindow)
    , notebook_(nullptr)
    , scenarioList_(nullptr)
    , currentScenarioLabel_(nullptr)
    , layerList_(nullptr)
    , modeSelector_(nullptr)
    , loadedFileScenarioName_(wxEmptyString)
{
    SetBackgroundColour(ModernColors::PANEL_BG);
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Título
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("NAVEGACIÓN"));
    title->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(title, 0, wxALL, 10);
    SetMinSize(wxSize(285, -1));
    
    // Notebook para organizar secciones
    notebook_ = new wxNotebook(this, wxID_ANY);
     
    // Pestaña: Escenarios
    wxPanel* scenarioPanel = new wxPanel(notebook_);
    wxBoxSizer* scenarioSizer = new wxBoxSizer(wxVERTICAL);
    
    scenarioList_ = new wxListBox(scenarioPanel, ID_SCENARIO_LIST);
    
    // Cargar escenarios desde ExperimentService
    if (mainWindow_) {
        auto& expSvc = mainWindow_->getExperimentService();
        for (size_t i = 0; i < expSvc.getScenarioPresetCount(); ++i) {
            scenarioList_->Append(wxString::FromUTF8(expSvc.getScenarioPresetName(i).c_str()));
        }
    }
    scenarioList_->SetSelection(0);
    scenarioSizer->Add(scenarioList_, 1, wxEXPAND | wxALL, 5);

    currentScenarioLabel_ = new wxStaticText(scenarioPanel, wxID_ANY, wxT("Archivo actual: presets internos"));
    currentScenarioLabel_->Wrap(240);
    currentScenarioLabel_->SetForegroundColour(wxColour(70, 80, 95));
    scenarioSizer->Add(currentScenarioLabel_, 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
    
    wxBoxSizer* loadButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* loadBtn = new wxButton(scenarioPanel, wxID_ANY, wxT("Cargar preset"));
    loadBtn->Bind(wxEVT_BUTTON, &LeftPanel::onLoadScenario, this);
    wxButton* openFileBtn = new wxButton(scenarioPanel, wxID_ANY, wxT("Abrir archivo"));
    openFileBtn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        if (mainWindow_) {
            mainWindow_->requestLoadScenario();
        }
    });
    loadButtonsSizer->Add(loadBtn, 1, wxRIGHT, 5);
    loadButtonsSizer->Add(openFileBtn, 1);
    scenarioSizer->Add(loadButtonsSizer, 0, wxEXPAND | wxALL, 5);
    
    scenarioPanel->SetSizer(scenarioSizer);
    notebook_->AddPage(scenarioPanel, wxT("Escenarios"));
    
    // Pestaña: Capas
    wxPanel* layersPanel = new wxPanel(notebook_);
    wxBoxSizer* layersSizer = new wxBoxSizer(wxVERTICAL);
    
    layerList_ = new wxCheckListBox(layersPanel, wxID_ANY);
    layerList_->Append(wxT("Grilla"));
    layerList_->Append(wxT("Campo vectorial"));
    layerList_->Append(wxT("Trayectoria"));
    layerList_->Append(wxT("Bote"));
    layerList_->Append(wxT("Obstáculos"));
    layerList_->Check(0, true);
    layerList_->Check(1, true);
    layerList_->Check(2, true);
    layerList_->Check(3, true);
    layerList_->Check(4, true);
    layerList_->Bind(wxEVT_CHECKLISTBOX, &LeftPanel::onLayerToggle, this);
    
    layersSizer->Add(layerList_, 1, wxEXPAND | wxALL, 5);
    layersPanel->SetSizer(layersSizer);
    notebook_->AddPage(layersPanel, wxT("Capas"));
    
    mainSizer->Add(notebook_, 1, wxEXPAND | wxALL, 5);
    
    SetSizer(mainSizer);
}

void LeftPanel::updateScenarioList()
{
    if (!scenarioList_ || !mainWindow_) return;
    
    scenarioList_->Clear();

    if (!loadedFileScenarioName_.empty()) {
        scenarioList_->Append(wxT("[Archivo] ") + loadedFileScenarioName_);
    }
    
    auto& expSvc = mainWindow_->getExperimentService();
    for (size_t i = 0; i < expSvc.getScenarioPresetCount(); ++i) {
        scenarioList_->Append(wxString::FromUTF8(expSvc.getScenarioPresetName(i).c_str()));
    }
    
    if (!loadedFileScenarioName_.empty()) {
        scenarioList_->SetSelection(0);
    } else if (scenarioList_->GetCount() > 0) {
        scenarioList_->SetSelection(0);
    }
}

int LeftPanel::getSelectedScenarioIndex() const
{
    if (scenarioList_) {
        const int selection = scenarioList_->GetSelection();
        if (selection == wxNOT_FOUND) {
            return -1;
        }
        if (!loadedFileScenarioName_.empty()) {
            return selection - 1;
        }
        return selection;
    }
    return -1;
}

void LeftPanel::setSelectedScenarioIndex(int index)
{
    if (!scenarioList_) {
        return;
    }
    int actualIndex = index;
    if (!loadedFileScenarioName_.empty()) {
        actualIndex += 1;
    }
    if (actualIndex >= 0 && actualIndex < static_cast<int>(scenarioList_->GetCount())) {
        scenarioList_->SetSelection(actualIndex);
    }
}

void LeftPanel::clearSelectedScenario()
{
    if (scenarioList_) {
        scenarioList_->SetSelection(wxNOT_FOUND);
    }
}

void LeftPanel::setLoadedFileScenarioName(const wxString& name)
{
    loadedFileScenarioName_ = name;
    updateScenarioList();
}

void LeftPanel::setCurrentScenarioLabel(const wxString& label)
{
    if (!currentScenarioLabel_) {
        return;
    }
    currentScenarioLabel_->SetLabel(label);
    currentScenarioLabel_->Wrap(240);
    Layout();
}

void LeftPanel::applyLayerVisibility()
{
    wxCommandEvent dummy;
    onLayerToggle(dummy);
}

void LeftPanel::setFieldLayerVisible(bool visible)
{
    if (!layerList_ || layerList_->GetCount() < 2) {
        return;
    }
    layerList_->Check(1, visible);
    applyLayerVisibility();
}

void LeftPanel::onScenarioSelect(wxCommandEvent& event)
{
    (void)event;
}

void LeftPanel::onLoadScenario(wxCommandEvent& event)
{
    (void)event;
    if (!mainWindow_) return;
    const int rawSelection = scenarioList_ ? scenarioList_->GetSelection() : wxNOT_FOUND;
    if (!loadedFileScenarioName_.empty() && rawSelection == 0) {
        if (!mainWindow_->requestReloadCurrentScenarioFile()) {
            wxMessageBox(wxT("No se pudo recargar el archivo actual."),
                         wxT("Abrir archivo"),
                         wxOK | wxICON_ERROR,
                         this);
        }
        return;
    }

    int sel = getSelectedScenarioIndex();
    if (sel < 0) {
        wxMessageBox(wxT("Selecciona un preset interno para cargarlo.\n\nUsa 'Abrir archivo' para escenarios guardados."),
                     wxT("Cargar preset"),
                     wxOK | wxICON_INFORMATION,
                     this);
        return;
    }
    mainWindow_->requestLoadScenarioPreset(static_cast<size_t>(sel));
}

void LeftPanel::onModeSelect(wxCommandEvent& event)
{
    (void)event;
    if (!mainWindow_) return;
    
    int sel = modeSelector_->GetSelection();
    switch (sel) {
        case 0: mainWindow_->setMode(AppMode::Theory); break;
        case 1: mainWindow_->setMode(AppMode::Simulation); break;
        case 2: mainWindow_->setMode(AppMode::Editor); break;
    }
}

void LeftPanel::onLayerToggle(wxCommandEvent& event)
{
    (void)event;
    if (!mainWindow_ || !layerList_) return;

    const bool showGrid = layerList_->IsChecked(0);
    const bool showField = layerList_->IsChecked(1);
    const bool showTrajectory = layerList_->IsChecked(2);
    const bool showBoat = layerList_->IsChecked(3);
    const bool showObstacles = layerList_->IsChecked(4);

    if (mainWindow_->getCanvas()) {
        mainWindow_->getCanvas()->setShowGrid(showGrid);
        mainWindow_->getCanvas()->setShowField(showField);
        mainWindow_->getCanvas()->setShowTrajectory(showTrajectory);
        mainWindow_->getCanvas()->setShowBoat(showBoat);
        mainWindow_->getCanvas()->setShowObstacles(showObstacles);
    }

    if (mainWindow_->getCanvas3D()) {
        mainWindow_->getCanvas3D()->setShowGrid(showGrid);
        mainWindow_->getCanvas3D()->setShowField(showField);
        mainWindow_->getCanvas3D()->setShowTrajectory(showTrajectory);
        mainWindow_->getCanvas3D()->setShowBoat(showBoat);
        mainWindow_->getCanvas3D()->setShowObstacles(showObstacles);
    }
}


} // namespace tp::presentation
