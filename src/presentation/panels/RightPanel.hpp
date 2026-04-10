#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include <wx/slider.h>
#include <wx/checkbox.h>

namespace tp::presentation {

class MainWindow;

class RightPanel : public wxPanel {
public:
    RightPanel(wxWindow* parent, MainWindow* mainWindow);

    void updateProperties();
    void showBoatProperties();
    void showMotorProperties();
    void showFieldProperties();
    void showScenarioProperties();

    double getThrustValue() const;
    double getDirectionValue() const;
    int getFieldType() const;
    double getFieldIntensity() const;
    int getMethod() const;
    double getTimeStep() const;
    bool isAutoThrustEnabled() const { return autoThrustCheck_ ? autoThrustCheck_->GetValue() : false; }

private:
    void onBoatParamChange(wxSpinEvent& event);
    void onMotorParamChange(wxCommandEvent& event);
    void onFieldTypeChange(wxCommandEvent& event);
    void onFieldParamChange(wxCommandEvent& event);
    void onNumericalParamChange(wxCommandEvent& event);
    void onDurationChange(wxSpinEvent& event);
    void updateBoatFromUI();
    void updateMotorFromUI();
    void updateFieldFromUI();
    void updateNumericalFromUI();
    void syncFieldControlsFromConfig();
    void updateFieldControlAvailability();
    void refreshScenePreview(bool syncProperties = false);

    MainWindow* mainWindow_;
    wxNotebook* notebook_;
    wxSpinCtrl* massCtrl_;
    wxSpinCtrl* dragCtrl_;
    wxSpinCtrl* sizeCtrl_;
    wxSpinCtrl* initialXCtrl_;
    wxSpinCtrl* initialYCtrl_;
    wxSpinCtrl* initialAngleCtrl_;
    wxSlider* thrustSlider_;
    wxSlider* directionSlider_;
    wxStaticText* thrustValueText_;
    wxStaticText* directionValueText_;
    wxCheckBox* autoThrustCheck_;
    wxChoice* fieldTypeChoice_;
    wxSlider* fieldIntensitySlider_;
    wxStaticText* fieldIntensityValue_;
    wxSpinCtrl* fieldCenterXCtrl_;
    wxSpinCtrl* fieldCenterYCtrl_;
    wxButton* customFieldEditBtn_;
    wxChoice* methodChoice_;
    wxSlider* dtSlider_;
    wxSpinCtrl* durationCtrl_;

    wxDECLARE_EVENT_TABLE();
};

} // namespace tp::presentation
