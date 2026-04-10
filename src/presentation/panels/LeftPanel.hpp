#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/listbox.h>
#include <wx/checklst.h>

namespace tp::presentation {

class MainWindow;

class LeftPanel : public wxPanel {
public:
    LeftPanel(wxWindow* parent, MainWindow* mainWindow);

    void updateScenarioList();
    int getSelectedScenarioIndex() const;
    void setSelectedScenarioIndex(int index);
    void clearSelectedScenario();
    void setCurrentScenarioLabel(const wxString& label);
    void setLoadedFileScenarioName(const wxString& name);
    void applyLayerVisibility();
    void setFieldLayerVisible(bool visible);

private:
    void onModeSelect(wxCommandEvent& event);
    void onScenarioSelect(wxCommandEvent& event);
    void onLoadScenario(wxCommandEvent& event);
    void onLayerToggle(wxCommandEvent& event);

    MainWindow* mainWindow_;
    wxNotebook* notebook_;
    wxListBox* scenarioList_;
    wxStaticText* currentScenarioLabel_;
    wxCheckListBox* layerList_;
    wxRadioBox* modeSelector_;
    wxString loadedFileScenarioName_;

    wxDECLARE_EVENT_TABLE();
};

} // namespace tp::presentation
