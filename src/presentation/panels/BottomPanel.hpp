#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>

namespace tp::presentation {

class MainWindow;
class SimpleGraph;

class BottomPanel : public wxPanel {
public:
    BottomPanel(wxWindow* parent, MainWindow* mainWindow);

    void updateResults();
    void addEvent(const wxString& message);
    void clearEvents();
    void clearGraphs();
    void setActiveTab(int tabIndex);

private:
    MainWindow* mainWindow_;
    wxNotebook* notebook_;
    wxStaticText* timeText_;
    wxStaticText* speedText_;
    wxStaticText* energyText_;
    wxStaticText* distanceText_;
    wxStaticText* collisionsText_;
    wxStaticText* displacementText_;
    wxStaticText* efficiencyText_;
    SimpleGraph* speedGraph_;
    SimpleGraph* energyGraph_;
    SimpleGraph* positionGraph_;
    wxListBox* eventsList_;
    wxStaticText* theoryTitle_;
    wxStaticText* theoryContent_;
    wxStaticText* theoryFormula_;
    wxStaticText* comparisonText_;

    wxDECLARE_EVENT_TABLE();
};

} // namespace tp::presentation
