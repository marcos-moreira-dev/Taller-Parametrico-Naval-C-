#include "panels/BottomPanel.hpp"

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

wxBEGIN_EVENT_TABLE(BottomPanel, wxPanel)
    // Eventos
wxEND_EVENT_TABLE()

BottomPanel::BottomPanel(wxWindow* parent, MainWindow* mainWindow)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME)
    , mainWindow_(mainWindow)
    , notebook_(nullptr)
    , timeText_(nullptr)
    , speedText_(nullptr)
    , energyText_(nullptr)
    , distanceText_(nullptr)
    , collisionsText_(nullptr)
    , displacementText_(nullptr)
    , efficiencyText_(nullptr)
    , eventsList_(nullptr)
    , theoryTitle_(nullptr)
    , theoryContent_(nullptr)
    , theoryFormula_(nullptr)
    , comparisonText_(nullptr)
{
    SetBackgroundColour(ModernColors::PANEL_BG);
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Notebook para pestañas
    notebook_ = new wxNotebook(this, wxID_ANY);
    
    // Pestaña: Resultados
    wxPanel* resultsPanel = new wxPanel(notebook_);
    wxBoxSizer* resultsSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* resultsTitle = new wxStaticText(resultsPanel, wxID_ANY, wxT("Resumen de la simulación"));
    resultsTitle->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    resultsSizer->Add(resultsTitle, 0, wxLEFT | wxRIGHT | wxTOP, 12);

    wxStaticText* resultsIntro = new wxStaticText(
        resultsPanel,
        wxID_ANY,
        wxT("Estos indicadores se actualizan durante la simulación y resumen el desempeño del bote respecto al escenario y al método numérico seleccionado."));
    resultsIntro->Wrap(900);
    resultsSizer->Add(resultsIntro, 0, wxLEFT | wxRIGHT | wxBOTTOM, 12);

    wxFlexGridSizer* statsGrid = new wxFlexGridSizer(0, 3, 12, 12);
    statsGrid->AddGrowableCol(0, 1);
    statsGrid->AddGrowableCol(1, 1);
    statsGrid->AddGrowableCol(2, 1);

    auto createStatCard = [&](const wxString& title, const wxString& initialValue, wxColour accent, wxStaticText** valueOut) {
        wxPanel* card = new wxPanel(resultsPanel);
        card->SetBackgroundColour(wxColour(248, 250, 253));
        card->SetMinSize(wxSize(210, 92));

        wxBoxSizer* cardSizer = new wxBoxSizer(wxVERTICAL);

        wxPanel* accentBar = new wxPanel(card);
        accentBar->SetBackgroundColour(accent);
        accentBar->SetMinSize(wxSize(-1, 4));
        cardSizer->Add(accentBar, 0, wxEXPAND);

        wxStaticText* label = new wxStaticText(card, wxID_ANY, title);
        label->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        label->SetForegroundColour(wxColour(90, 100, 115));
        cardSizer->Add(label, 0, wxLEFT | wxRIGHT | wxTOP, 10);

        wxStaticText* value = new wxStaticText(card, wxID_ANY, initialValue);
        value->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        value->SetForegroundColour(wxColour(25, 35, 50));
        cardSizer->Add(value, 1, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 10);

        card->SetSizer(cardSizer);
        *valueOut = value;
        return card;
    };

    statsGrid->Add(createStatCard(wxT("Tiempo total"), wxT("0.00 s"), wxColour(66, 133, 244), &timeText_), 1, wxEXPAND);
    statsGrid->Add(createStatCard(wxT("Velocidad actual"), wxT("0.00 m/s"), wxColour(15, 157, 88), &speedText_), 1, wxEXPAND);
    statsGrid->Add(createStatCard(wxT("Energía cinética"), wxT("0.00 J"), wxColour(244, 160, 0), &energyText_), 1, wxEXPAND);
    statsGrid->Add(createStatCard(wxT("Distancia recorrida"), wxT("0.00 m"), wxColour(171, 71, 188), &distanceText_), 1, wxEXPAND);
    statsGrid->Add(createStatCard(wxT("Desplazamiento neto"), wxT("0.00 m"), wxColour(0, 172, 193), &displacementText_), 1, wxEXPAND);
    statsGrid->Add(createStatCard(wxT("Eficiencia de ruta"), wxT("100.0 %"), wxColour(239, 83, 80), &efficiencyText_), 1, wxEXPAND);

    wxPanel* extraRow = new wxPanel(resultsPanel);
    extraRow->SetBackgroundColour(wxColour(248, 250, 253));
    wxBoxSizer* extraSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* collisionLabel = new wxStaticText(extraRow, wxID_ANY, wxT("Colisiones detectadas"));
    collisionLabel->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    collisionLabel->SetForegroundColour(wxColour(90, 100, 115));
    collisionsText_ = new wxStaticText(extraRow, wxID_ANY, wxT("0"));
    collisionsText_->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    collisionsText_->SetForegroundColour(wxColour(25, 35, 50));
    extraSizer->Add(collisionLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 10);
    extraSizer->AddStretchSpacer();
    extraSizer->Add(collisionsText_, 0, wxALL | wxALIGN_CENTER_VERTICAL, 10);
    extraRow->SetSizer(extraSizer);

    resultsSizer->Add(statsGrid, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);
    resultsSizer->Add(extraRow, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);
    resultsSizer->AddStretchSpacer();
    resultsPanel->SetSizer(resultsSizer);
    notebook_->AddPage(resultsPanel, wxT("Resultados"));
    
    // Pestaña: Gráficas
    wxPanel* graphsPanel = new wxPanel(notebook_);
    wxBoxSizer* graphsSizer = new wxBoxSizer(wxHORIZONTAL);
    
    speedGraph_ = new SimpleGraph(graphsPanel, wxT("Velocidad vs Tiempo"));
    speedGraph_->setColor(wxColour(0, 100, 200));
    graphsSizer->Add(speedGraph_, 1, wxEXPAND | wxALL, 5);
    
    energyGraph_ = new SimpleGraph(graphsPanel, wxT("Energía vs Tiempo"));
    energyGraph_->setColor(wxColour(200, 100, 0));
    graphsSizer->Add(energyGraph_, 1, wxEXPAND | wxALL, 5);
    
    positionGraph_ = new SimpleGraph(graphsPanel, wxT("Posición X vs Tiempo"));
    positionGraph_->setColor(wxColour(0, 150, 0));
    graphsSizer->Add(positionGraph_, 1, wxEXPAND | wxALL, 5);
    
    graphsPanel->SetSizer(graphsSizer);
    notebook_->AddPage(graphsPanel, wxT("Gráficas"));
    
    // Pestaña: Eventos
    wxPanel* eventsPanel = new wxPanel(notebook_);
    wxBoxSizer* eventsSizer = new wxBoxSizer(wxVERTICAL);
    eventsList_ = new wxListBox(eventsPanel, wxID_ANY);
    eventsSizer->Add(eventsList_, 1, wxEXPAND | wxALL, 5);
    wxButton* clearEventsBtn = new wxButton(eventsPanel, wxID_ANY, wxT("Limpiar"));
    eventsSizer->Add(clearEventsBtn, 0, wxALIGN_RIGHT | wxALL, 5);
    eventsPanel->SetSizer(eventsSizer);
    notebook_->AddPage(eventsPanel, wxT("Eventos"));
    
    mainSizer->Add(notebook_, 1, wxEXPAND | wxALL, 5);
    SetSizer(mainSizer);
}

void BottomPanel::updateResults()
{
    if (!mainWindow_) return;
    
    // Obtener datos reales de mainWindow
    double time = mainWindow_->getCurrentTime();
    double speed = mainWindow_->getCurrentSpeed();
    double energy = mainWindow_->getCurrentEnergy();
    double distance = mainWindow_->getTotalDistance();
    double displacement = mainWindow_->getNetDisplacement();
    double efficiency = mainWindow_->getRouteEfficiency();
    
    // Actualizar texto de resultados
    if (timeText_) {
        wxString txt;
        txt.Printf(wxT("%.2f s"), time);
        timeText_->SetLabel(txt);
    }
    if (speedText_) {
        wxString txt;
        txt.Printf(wxT("%.2f m/s"), speed);
        speedText_->SetLabel(txt);
    }
    if (energyText_) {
        wxString txt;
        txt.Printf(wxT("%.2f J"), energy);
        energyText_->SetLabel(txt);
    }
    if (distanceText_) {
        wxString txt;
        txt.Printf(wxT("%.2f m"), distance);
        distanceText_->SetLabel(txt);
    }
    if (collisionsText_) {
        collisionsText_->SetLabel(wxString::Format(wxT("%d"), 0));
    }
    if (displacementText_) {
        wxString txt;
        txt.Printf(wxT("%.2f m"), displacement);
        displacementText_->SetLabel(txt);
    }
    if (efficiencyText_) {
        wxString txt;
        txt.Printf(wxT("%.1f %%"), efficiency * 100);
        efficiencyText_->SetLabel(txt);
    }
    
    // Actualizar gráficas si hay datos
    const auto& trajectory = mainWindow_->getTrajectoryPoints();
    if (!trajectory.empty()) {
        const auto& lastPoint = trajectory.back();
        
        if (speedGraph_) {
            speedGraph_->addDataPoint(time, speed);
        }
        if (energyGraph_) {
            energyGraph_->addDataPoint(time, energy);
        }
        if (positionGraph_) {
            positionGraph_->addDataPoint(time, lastPoint.x);
        }
    }
}

void BottomPanel::addEvent(const wxString& message)
{
    if (eventsList_) {
        wxDateTime now = wxDateTime::Now();
        wxString timestamp = now.Format(wxT("%H:%M:%S"));
        eventsList_->Append(timestamp + wxT(" - ") + message);
        // Auto-scroll al último evento
        eventsList_->SetSelection(eventsList_->GetCount() - 1);
    }
}

void BottomPanel::clearGraphs()
{
    if (speedGraph_) {
        speedGraph_->clearData();
    }
    if (energyGraph_) {
        energyGraph_->clearData();
    }
    if (positionGraph_) {
        positionGraph_->clearData();
    }
}

void BottomPanel::clearEvents()
{
    if (eventsList_) {
        eventsList_->Clear();
    }
}

void BottomPanel::setActiveTab(int tabIndex)
{
    if (notebook_ && tabIndex >= 0 && tabIndex < (int)notebook_->GetPageCount()) {
        notebook_->SetSelection(tabIndex);
    }
}


} // namespace tp::presentation
