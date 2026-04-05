#pragma once

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include <wx/wx.h>
#include <wx/spinctrl.h>

#include "main_window/MainWindow.hpp"

namespace tp::presentation {

class FieldEditorPanel : public wxPanel {
public:
    FieldEditorPanel(wxWindow* parent, MainWindow* mainWindow);

    void syncFromDocument();

private:
    void setupUI();
    void applyControlsToDocument();
    void onFieldTypeChange(wxCommandEvent& event);
    void onFieldParamChange(wxCommandEvent& event);
    void onSpinCtrlChange(wxSpinEvent& event);
    void onOpenCustomField(wxCommandEvent& event);

    MainWindow* mainWindow_;
    wxChoice* fieldTypeChoice_;
    wxSlider* fieldIntensitySlider_;
    wxSpinCtrl* fieldCenterXCtrl_;
    wxSpinCtrl* fieldCenterYCtrl_;
    wxStaticText* previewText_;
    wxButton* customFieldBtn_;

    wxDECLARE_EVENT_TABLE();
};

} // namespace tp::presentation
