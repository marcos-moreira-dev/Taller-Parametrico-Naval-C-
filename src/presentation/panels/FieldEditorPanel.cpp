#include "panels/FieldEditorPanel.hpp"

#include <algorithm>
#include <cmath>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>

namespace tp::presentation {

namespace {

void updateCustomFieldAvailability(wxChoice* fieldTypeChoice,
                                   wxSlider* intensitySlider,
                                   wxSpinCtrl* centerXCtrl,
                                   wxSpinCtrl* centerYCtrl,
                                   wxStaticText* previewText,
                                   wxButton* customFieldBtn,
                                   const tp::application::FieldConfigView& fieldView) {
    const bool isCustom = fieldView.type == tp::application::FieldPresetType::Custom;
    if (intensitySlider) intensitySlider->Enable(!isCustom);
    if (centerXCtrl) centerXCtrl->Enable(!isCustom);
    if (centerYCtrl) centerYCtrl->Enable(!isCustom);
    if (customFieldBtn) customFieldBtn->Enable(true);
    if (previewText && isCustom && fieldTypeChoice) {
        previewText->SetLabel(previewText->GetLabel() + wxT("\nNota: en modo personalizado la ecuación define completamente la magnitud y dirección del campo."));
    }
}

} // namespace

enum {
    ID_FIELD_EDITOR_TYPE = 4000,
    ID_FIELD_EDITOR_INTENSITY,
    ID_FIELD_EDITOR_CENTER_X,
    ID_FIELD_EDITOR_CENTER_Y,
    ID_FIELD_EDITOR_CUSTOM
};

wxBEGIN_EVENT_TABLE(FieldEditorPanel, wxPanel)
    EVT_CHOICE(ID_FIELD_EDITOR_TYPE, FieldEditorPanel::onFieldTypeChange)
    EVT_SLIDER(ID_FIELD_EDITOR_INTENSITY, FieldEditorPanel::onFieldParamChange)
    EVT_SPINCTRL(ID_FIELD_EDITOR_CENTER_X, FieldEditorPanel::onSpinCtrlChange)
    EVT_SPINCTRL(ID_FIELD_EDITOR_CENTER_Y, FieldEditorPanel::onSpinCtrlChange)
    EVT_BUTTON(ID_FIELD_EDITOR_CUSTOM, FieldEditorPanel::onOpenCustomField)
wxEND_EVENT_TABLE()

FieldEditorPanel::FieldEditorPanel(wxWindow* parent, MainWindow* mainWindow)
    : wxPanel(parent, wxID_ANY)
    , mainWindow_(mainWindow)
    , fieldTypeChoice_(nullptr)
    , fieldIntensitySlider_(nullptr)
    , fieldCenterXCtrl_(nullptr)
    , fieldCenterYCtrl_(nullptr)
    , previewText_(nullptr)
    , customFieldBtn_(nullptr) {
    SetBackgroundColour(wxColour(245, 245, 245));
    setupUI();
    syncFromDocument();
}

void FieldEditorPanel::setupUI() {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("EDITOR DE CAMPOS"));
    title->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(title, 0, wxALL, 10);

    wxStaticText* subtitle = new wxStaticText(this, wxID_ANY,
        wxT("Define el campo vectorial independientemente del mapa."));
    subtitle->Wrap(320);
    mainSizer->Add(subtitle, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxStaticText* postponed = new wxStaticText(this, wxID_ANY,
        wxT("(pospuesto para próxima versión)"));
    postponed->SetForegroundColour(wxColour(180, 35, 35));
    postponed->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD));
    mainSizer->Add(postponed, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxStaticBoxSizer* fieldTypeBox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Tipo de campo"));
    wxString fieldTypes[] = {
        wxT("Uniforme"),
        wxT("Lineal"),
        wxT("Radial"),
        wxT("Rotacional"),
        wxT("Personalizado")
    };
    fieldTypeChoice_ = new wxChoice(this, ID_FIELD_EDITOR_TYPE, wxDefaultPosition, wxDefaultSize, 5, fieldTypes);
    fieldTypeBox->Add(fieldTypeChoice_, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(fieldTypeBox, 0, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* paramsBox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Parámetros"));
    wxBoxSizer* intensitySizer = new wxBoxSizer(wxHORIZONTAL);
    intensitySizer->Add(new wxStaticText(this, wxID_ANY, wxT("Intensidad:")), 0, wxALIGN_CENTER_VERTICAL);
    fieldIntensitySlider_ = new wxSlider(this, ID_FIELD_EDITOR_INTENSITY, 50, 0, 100,
                                         wxDefaultPosition, wxSize(160, -1), wxSL_HORIZONTAL | wxSL_LABELS);
    intensitySizer->Add(fieldIntensitySlider_, 1, wxEXPAND | wxLEFT, 5);
    paramsBox->Add(intensitySizer, 0, wxEXPAND | wxALL, 5);

    wxFlexGridSizer* centerGrid = new wxFlexGridSizer(2, 5, 5);
    centerGrid->AddGrowableCol(1);
    centerGrid->Add(new wxStaticText(this, wxID_ANY, wxT("Centro X:")));
    fieldCenterXCtrl_ = new wxSpinCtrl(this, ID_FIELD_EDITOR_CENTER_X, wxT("25"));
    fieldCenterXCtrl_->SetRange(0, 200);
    centerGrid->Add(fieldCenterXCtrl_, 1, wxEXPAND);
    centerGrid->Add(new wxStaticText(this, wxID_ANY, wxT("Centro Y:")));
    fieldCenterYCtrl_ = new wxSpinCtrl(this, ID_FIELD_EDITOR_CENTER_Y, wxT("25"));
    fieldCenterYCtrl_->SetRange(0, 200);
    centerGrid->Add(fieldCenterYCtrl_, 1, wxEXPAND);
    paramsBox->Add(centerGrid, 0, wxEXPAND | wxALL, 5);

    customFieldBtn_ = new wxButton(this, ID_FIELD_EDITOR_CUSTOM, wxT("Campo personalizado..."));
    paramsBox->Add(customFieldBtn_, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(paramsBox, 0, wxEXPAND | wxALL, 5);

    wxStaticBoxSizer* previewBox = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Resumen"));
    previewText_ = new wxStaticText(this, wxID_ANY, wxEmptyString);
    previewText_->Wrap(320);
    previewBox->Add(previewText_, 0, wxALL, 5);
    mainSizer->Add(previewBox, 0, wxEXPAND | wxALL, 5);

    mainSizer->AddStretchSpacer();
    SetSizer(mainSizer);
}

void FieldEditorPanel::syncFromDocument() {
    if (!mainWindow_) {
        return;
    }

    auto& fieldView = mainWindow_->getScenarioDocument().fieldView();
    if (fieldTypeChoice_ && !fieldTypeChoice_->HasFocus()) {
        fieldTypeChoice_->SetSelection(std::clamp(static_cast<int>(fieldView.type), 0, 4));
    }
    if (fieldIntensitySlider_ && !fieldIntensitySlider_->HasFocus()) {
        fieldIntensitySlider_->SetValue(std::clamp(static_cast<int>(fieldView.intensity * 50.0), 0, 100));
    }
    if (fieldCenterXCtrl_ && !fieldCenterXCtrl_->HasFocus()) {
        fieldCenterXCtrl_->SetValue(static_cast<int>(fieldView.centerX));
    }
    if (fieldCenterYCtrl_ && !fieldCenterYCtrl_->HasFocus()) {
        fieldCenterYCtrl_->SetValue(static_cast<int>(fieldView.centerY));
    }

    if (previewText_) {
        const auto& config = mainWindow_->getExperimentService().getConfig();
        const auto sampleCenter = config.field.getValue(fieldView.centerX, fieldView.centerY);
        const auto sampleBoat = config.field.getValue(config.boat.getPosition().x, config.boat.getPosition().y);

        double sampledMaxMagnitude = 0.0;
        for (int y = 0; y < config.scenario.getHeight(); y += 5) {
            for (int x = 0; x < config.scenario.getWidth(); x += 5) {
                const auto v = config.field.getValue(static_cast<double>(x), static_cast<double>(y));
                sampledMaxMagnitude = std::max(sampledMaxMagnitude, std::sqrt(v.x * v.x + v.y * v.y));
            }
        }

        wxString summary = wxString::Format(
            wxT("Tipo actual: %s\nIntensidad: %.2f\nCentro: (%.1f, %.1f)\nMuestra en centro: (%.2f, %.2f)\nMuestra en bote: (%.2f, %.2f)\nMagnitud máxima muestreada: %.3f\nCanvas: el campo se previsualiza sobre el escenario."),
            fieldTypeChoice_ ? fieldTypeChoice_->GetStringSelection() : wxT("-"),
            fieldView.intensity,
            fieldView.centerX,
            fieldView.centerY,
            sampleCenter.x,
            sampleCenter.y,
            sampleBoat.x,
            sampleBoat.y,
            sampledMaxMagnitude);

        if (fieldView.type == tp::application::FieldPresetType::Custom) {
            summary += wxString::Format(
                wxT("\nFx = %s\nFy = %s"),
                wxString::FromUTF8(fieldView.customFx.c_str()),
                wxString::FromUTF8(fieldView.customFy.c_str()));
            if (fieldView.customFx == "0" && fieldView.customFy == "0") {
                summary += wxT("\nCampo personalizado no configurado todavía.");
            }
        }

        previewText_->SetLabel(summary);
    }

    updateCustomFieldAvailability(fieldTypeChoice_, fieldIntensitySlider_, fieldCenterXCtrl_, fieldCenterYCtrl_, previewText_, customFieldBtn_, fieldView);
}

void FieldEditorPanel::applyControlsToDocument() {
    if (!mainWindow_) {
        return;
    }

    auto& fieldView = mainWindow_->getScenarioDocument().fieldView();
    fieldView.type = static_cast<tp::application::FieldPresetType>(std::clamp(fieldTypeChoice_->GetSelection(), 0, 4));
    fieldView.intensity = fieldIntensitySlider_ ? fieldIntensitySlider_->GetValue() / 50.0 : fieldView.intensity;
    fieldView.centerX = fieldCenterXCtrl_ ? fieldCenterXCtrl_->GetValue() : fieldView.centerX;
    fieldView.centerY = fieldCenterYCtrl_ ? fieldCenterYCtrl_->GetValue() : fieldView.centerY;

    std::string errorMessage;
    if (!mainWindow_->getScenarioDocument().rebuildField(&errorMessage) && !errorMessage.empty()) {
        wxMessageBox(wxString::FromUTF8(errorMessage.c_str()), wxT("Campo vectorial"), wxOK | wxICON_WARNING);
        return;
    }

    mainWindow_->setModified(true);
    mainWindow_->requestSimulationReset();
    mainWindow_->updateCanvas();
    syncFromDocument();
}

void FieldEditorPanel::onFieldTypeChange(wxCommandEvent& event) {
    if (fieldTypeChoice_ && fieldTypeChoice_->GetSelection() == 4) {
        onOpenCustomField(event);
        syncFromDocument();
        return;
    }
    applyControlsToDocument();
}

void FieldEditorPanel::onFieldParamChange(wxCommandEvent& event) {
    (void)event;
    applyControlsToDocument();
}

void FieldEditorPanel::onSpinCtrlChange(wxSpinEvent& event) {
    (void)event;
    applyControlsToDocument();
}

void FieldEditorPanel::onOpenCustomField(wxCommandEvent& event) {
    (void)event;
    if (!mainWindow_) {
        return;
    }
    mainWindow_->openCustomFieldDialog();
}

} // namespace tp::presentation
