#include "panels/TheoryPanel.hpp"

#include <wx/button.h>
#include <wx/choice.h>
#include <wx/filename.h>
#include <wx/filesys.h>
#include <wx/image.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/log.h>
#include <wx/textctrl.h>

#include <algorithm>
#include <sstream>
#include <set>

namespace tp::presentation {

namespace {

wxString U(const char8_t* text) {
    return wxString::FromUTF8(reinterpret_cast<const char*>(text));
}

wxString NormalizeForSearch(wxString text) {
    text.MakeLower();
    text.Replace("á", "a");
    text.Replace("é", "e");
    text.Replace("í", "i");
    text.Replace("ó", "o");
    text.Replace("ú", "u");
    text.Replace("ñ", "n");
    return text;
}

int CountMeaningfulLines(const std::string& text) {
    int count = 0;
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.find_first_not_of(" \\t\\r\\n") != std::string::npos) {
            ++count;
        }
    }
    return count;
}

int EstimateDescriptionHeight(const std::string& description) {
    const int approxLines = std::max(2, static_cast<int>(description.size() / 90) + 1);
    return std::max(140, 56 + approxLines * 34);
}

int EstimateBodyHeight(const std::string& bodyMarkdown, int minHeight = 440, int maxHeight = 1500) {
    const int lines = CountMeaningfulLines(bodyMarkdown);
    return std::clamp(220 + lines * 28, minHeight, maxHeight);
}


wxString TopicBaseUrl(const std::string& basePath) {
    if (basePath.empty()) {
        return wxEmptyString;
    }
    wxFileName fileName = wxFileName::DirName(wxString::FromUTF8(basePath.c_str()));
    return wxFileSystem::FileNameToURL(fileName);
}

wxBitmap LoadScaledBitmap(const wxString& path, int maxWidth) {
    if (path.IsEmpty() || !wxFileExists(path)) {
        return wxBitmap();
    }

    wxLogNull suppressImageErrors;
    wxImage image(path, wxBITMAP_TYPE_ANY);
    if (!image.IsOk()) {
        return wxBitmap();
    }

    if (image.GetWidth() > maxWidth && maxWidth > 0) {
        const double scale = static_cast<double>(maxWidth) / static_cast<double>(image.GetWidth());
        const int scaledHeight = std::max(1, static_cast<int>(image.GetHeight() * scale));
        image = image.Scale(maxWidth, scaledHeight, wxIMAGE_QUALITY_HIGH);
    }

    return wxBitmap(image);
}

} // namespace

enum {
    ID_CONCEPT_LIST = 100,
    ID_CATEGORY_CHOICE,
    ID_SEARCH_CTRL,
    ID_NEXT_BTN,
    ID_PREV_BTN
};

wxBEGIN_EVENT_TABLE(TheoryPanel, wxPanel)
    EVT_LISTBOX(ID_CONCEPT_LIST, TheoryPanel::onConceptSelect)
    EVT_CHOICE(ID_CATEGORY_CHOICE, TheoryPanel::onCategorySelect)
    EVT_TEXT_ENTER(ID_SEARCH_CTRL, TheoryPanel::onSearch)
    EVT_BUTTON(ID_NEXT_BTN, TheoryPanel::onNext)
    EVT_BUTTON(ID_PREV_BTN, TheoryPanel::onPrevious)
wxEND_EVENT_TABLE()

TheoryPanel::TheoryPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
    , conceptList_(nullptr)
    , categoryChoice_(nullptr)
    , searchCtrl_(nullptr)
    , titleText_(nullptr)
    , categoryText_(nullptr)
    , difficultyText_(nullptr)
    , descriptionHtml_(nullptr)
    , formulaPanel_(nullptr)
    , formulaScroll_(nullptr)
    , formulaWidget_(nullptr)
    , figurePanel_(nullptr)
    , figureBitmap_(nullptr)
    , figureCaptionHtml_(nullptr)
    , explanationHtml_(nullptr)
    , exerciseHtml_(nullptr)
    , prevBtn_(nullptr)
    , nextBtn_(nullptr)
    , currentConceptIndex_(-1)
    , repository_()
    , markdownRenderer_() {
    SetBackgroundColour(wxColour(250, 250, 250));
    loadTopics();
    setupUI();
    if (!topics_.empty()) {
        loadConcept(0);
    }
}

wxString TheoryPanel::toWx(const std::string& utf8) const {
    return wxString::FromUTF8(utf8.c_str());
}

void TheoryPanel::loadTopics() {
    topics_ = repository_.loadAllTopics();
}

void TheoryPanel::rebuildConceptList() {
    if (!conceptList_) {
        return;
    }

    const wxString selectedCategory = categoryChoice_ ? categoryChoice_->GetStringSelection() : wxT("Todas");
    const wxString searchTerm = searchCtrl_ ? NormalizeForSearch(searchCtrl_->GetValue()) : wxT("");

    filteredConceptIndices_.clear();
    conceptList_->Clear();

    for (size_t i = 0; i < topics_.size(); ++i) {
        const auto& topic = topics_[i];
        const wxString title = toWx(topic.title);
        const wxString category = toWx(topic.category);
        const wxString description = toWx(topic.description);

        const bool categoryMatch = (selectedCategory == wxT("Todas") || category == selectedCategory);
        const bool textMatch = searchTerm.IsEmpty() ||
            NormalizeForSearch(title).Contains(searchTerm) ||
            NormalizeForSearch(description).Contains(searchTerm);

        if (categoryMatch && textMatch) {
            filteredConceptIndices_.push_back(static_cast<int>(i));
            conceptList_->Append(title);
        }
    }

    if (filteredConceptIndices_.empty()) {
        currentConceptIndex_ = -1;
        titleText_->SetLabel(U(u8"Sin resultados"));
        categoryText_->SetLabel(wxEmptyString);
        difficultyText_->SetLabel(wxEmptyString);
        descriptionHtml_->setHtmlBody(U(u8"<p class='description-lead'>No se encontraron conceptos con ese filtro.</p>"));
        formulaWidget_->setLatex(wxEmptyString);
        explanationHtml_->setHtmlBody(U(u8"<p>Prueba otra categoría o una búsqueda más corta.</p>"));
        exerciseHtml_->setHtmlBody(U(u8"<p>Cuando exista un ejercicio guiado para este tema aparecerá aquí.</p>"));
        if (figurePanel_) {
            figurePanel_->Hide();
        }
        prevBtn_->Enable(false);
        nextBtn_->Enable(false);
        return;
    }

    auto currentIt = std::find(filteredConceptIndices_.begin(), filteredConceptIndices_.end(), currentConceptIndex_);
    if (currentIt == filteredConceptIndices_.end()) {
        currentConceptIndex_ = filteredConceptIndices_.front();
        conceptList_->SetSelection(0);
    } else {
        conceptList_->SetSelection(static_cast<int>(std::distance(filteredConceptIndices_.begin(), currentIt)));
    }
}

void TheoryPanel::setupUI() {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    wxPanel* leftPanel = new wxPanel(this);
    leftPanel->SetBackgroundColour(wxColour(240, 240, 240));
    leftPanel->SetMinSize(wxSize(300, -1));
    wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* navTitle = new wxStaticText(leftPanel, wxID_ANY, wxT("CONCEPTOS"));
    navTitle->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    leftSizer->Add(navTitle, 0, wxALL, 10);

    searchCtrl_ = new wxTextCtrl(leftPanel, ID_SEARCH_CTRL, wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    searchCtrl_->SetHint(U(u8"Buscar concepto..."));
    leftSizer->Add(searchCtrl_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxStaticText* catLabel = new wxStaticText(leftPanel, wxID_ANY, U(u8"Categoría:"));
    leftSizer->Add(catLabel, 0, wxLEFT | wxRIGHT, 10);

    std::set<wxString> categories;
    for (const auto& topic : topics_) {
        categories.insert(toWx(topic.category));
    }

    wxArrayString categoryItems;
    categoryItems.Add(wxT("Todas"));
    for (const auto& category : categories) {
        categoryItems.Add(category);
    }

    categoryChoice_ = new wxChoice(leftPanel, ID_CATEGORY_CHOICE, wxDefaultPosition, wxDefaultSize, categoryItems);
    categoryChoice_->SetSelection(0);
    leftSizer->Add(categoryChoice_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    conceptList_ = new wxListBox(leftPanel, ID_CONCEPT_LIST);
    rebuildConceptList();
    leftSizer->Add(conceptList_, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    leftPanel->SetSizer(leftSizer);
    mainSizer->Add(leftPanel, 0, wxEXPAND | wxALL, 5);

    wxScrolledWindow* contentPanel = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                          wxVSCROLL | wxBORDER_NONE);
    contentPanel->SetScrollRate(0, 14);
    contentPanel->SetBackgroundColour(*wxWHITE);
    wxBoxSizer* contentSizer = new wxBoxSizer(wxVERTICAL);

    wxPanel* headerPanel = new wxPanel(contentPanel);
    headerPanel->SetBackgroundColour(wxColour(230, 240, 255));
    wxBoxSizer* headerSizer = new wxBoxSizer(wxHORIZONTAL);

    titleText_ = new wxStaticText(headerPanel, wxID_ANY, U(u8"Título"));
    titleText_->SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    headerSizer->Add(titleText_, 1, wxALL, 15);

    categoryText_ = new wxStaticText(headerPanel, wxID_ANY, U(u8"Categoría"));
    categoryText_->SetForegroundColour(wxColour(100, 100, 100));
    headerSizer->Add(categoryText_, 0, wxALL | wxALIGN_CENTER_VERTICAL, 15);

    difficultyText_ = new wxStaticText(headerPanel, wxID_ANY, wxT("★★★☆☆"));
    difficultyText_->SetForegroundColour(wxColour(200, 150, 0));
    headerSizer->Add(difficultyText_, 0, wxALL | wxALIGN_CENTER_VERTICAL, 15);

    headerPanel->SetSizer(headerSizer);
    contentSizer->Add(headerPanel, 0, wxEXPAND);

    wxStaticText* descLabel = new wxStaticText(contentPanel, wxID_ANY, U(u8"DESCRIPCIÓN"));
    descLabel->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    contentSizer->Add(descLabel, 0, wxLEFT | wxRIGHT | wxTOP, 15);

    descriptionHtml_ = new MathHtmlPanel(contentPanel);
    descriptionHtml_->SetMinSize(wxSize(-1, 150));
    contentSizer->Add(descriptionHtml_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxStaticText* formulaLabel = new wxStaticText(contentPanel, wxID_ANY, U(u8"FÓRMULA PRINCIPAL"));
    formulaLabel->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    contentSizer->Add(formulaLabel, 0, wxLEFT | wxRIGHT | wxTOP, 15);

    formulaPanel_ = new wxPanel(contentPanel);
    formulaPanel_->SetBackgroundColour(wxColour(248, 250, 253));
    formulaPanel_->SetMinSize(wxSize(-1, 220));
    wxBoxSizer* formulaSizer = new wxBoxSizer(wxVERTICAL);
    formulaWidget_ = new LatexBitmapPanel(formulaPanel_, wxEmptyString);
    formulaWidget_->setForegroundColor(wxColour(0, 0, 128));
    formulaSizer->Add(formulaWidget_, 1, wxEXPAND | wxALL, 0);
    formulaPanel_->SetSizer(formulaSizer);
    contentSizer->Add(formulaPanel_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxStaticText* figureLabel = new wxStaticText(contentPanel, wxID_ANY, U(u8"FIGURA DE ESTUDIO"));
    figureLabel->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    contentSizer->Add(figureLabel, 0, wxLEFT | wxRIGHT | wxTOP, 15);

    figurePanel_ = new wxPanel(contentPanel);
    figurePanel_->SetBackgroundColour(wxColour(251, 253, 255));
    wxBoxSizer* figureSizer = new wxBoxSizer(wxVERTICAL);
    figureBitmap_ = new wxStaticBitmap(figurePanel_, wxID_ANY, wxBitmap());
    figureCaptionHtml_ = new MathHtmlPanel(figurePanel_);
    figureCaptionHtml_->SetMinSize(wxSize(-1, 120));
    figureSizer->Add(figureBitmap_, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 12);
    figureSizer->Add(figureCaptionHtml_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
    figurePanel_->SetSizer(figureSizer);
    contentSizer->Add(figurePanel_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxStaticText* explLabel = new wxStaticText(contentPanel, wxID_ANY, U(u8"DESARROLLO Y CONTEXTO"));
    explLabel->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    contentSizer->Add(explLabel, 0, wxLEFT | wxRIGHT | wxTOP, 15);

    explanationHtml_ = new MathHtmlPanel(contentPanel);
    explanationHtml_->SetMinSize(wxSize(-1, 760));
    contentSizer->Add(explanationHtml_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxStaticText* exerciseLabel = new wxStaticText(contentPanel, wxID_ANY, U(u8"EJERCICIO GUIADO"));
    exerciseLabel->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    contentSizer->Add(exerciseLabel, 0, wxLEFT | wxRIGHT | wxTOP, 15);

    exerciseHtml_ = new MathHtmlPanel(contentPanel);
    exerciseHtml_->SetMinSize(wxSize(-1, 520));
    contentSizer->Add(exerciseHtml_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxBoxSizer* actionSizer = new wxBoxSizer(wxHORIZONTAL);
    prevBtn_ = new wxButton(contentPanel, ID_PREV_BTN, U(u8"◀ Anterior"));
    nextBtn_ = new wxButton(contentPanel, ID_NEXT_BTN, U(u8"Siguiente ▶"));
    actionSizer->Add(prevBtn_, 0, wxALL, 10);
    actionSizer->AddStretchSpacer();
    actionSizer->Add(nextBtn_, 0, wxALL, 10);
    contentSizer->Add(actionSizer, 0, wxEXPAND);

    contentPanel->SetSizer(contentSizer);
    contentPanel->FitInside();
    mainSizer->Add(contentPanel, 1, wxEXPAND | wxALL, 5);

    SetSizer(mainSizer);
}

void TheoryPanel::loadConcept(int index) {
    if (index < 0 || index >= static_cast<int>(topics_.size())) {
        return;
    }
    currentConceptIndex_ = index;
    auto it = std::find(filteredConceptIndices_.begin(), filteredConceptIndices_.end(), index);
    if (it != filteredConceptIndices_.end()) {
        conceptList_->SetSelection(static_cast<int>(std::distance(filteredConceptIndices_.begin(), it)));
    }
    updateDisplay();
}


void TheoryPanel::updateFigureForTopic(const tp::education::TheoryTopic& topic) {
    if (!figurePanel_ || !figureBitmap_ || !figureCaptionHtml_) {
        return;
    }

    if (topic.figureMainPath.empty()) {
        figurePanel_->Hide();
        return;
    }

    const wxBitmap bitmap = LoadScaledBitmap(toWx(topic.figureMainPath), 900);
    if (!bitmap.IsOk()) {
        figureBitmap_->SetBitmap(wxBitmap());
        figureCaptionHtml_->setHtmlBody(U(u8"<div class='note-box'>No se pudo cargar la figura estática de este tema.</div>"));
        figurePanel_->Show();
        figurePanel_->Layout();
        return;
    }

    figureBitmap_->SetBitmap(bitmap);
    wxString captionHtml;
    if (!topic.figureNoteMarkdown.empty()) {
        captionHtml = toWx(markdownRenderer_.toHtml(topic.figureNoteMarkdown));
    } else {
        captionHtml = U(u8"<p>Figura estática generada como apoyo visual del tema.</p>");
    }
    figureCaptionHtml_->setHtmlBody(captionHtml, TopicBaseUrl(topic.contentBasePath));
    figurePanel_->Show();
    figurePanel_->Layout();
}

void TheoryPanel::updateDisplay() {
    if (currentConceptIndex_ < 0 || currentConceptIndex_ >= static_cast<int>(topics_.size())) {
        return;
    }

    const auto& topic = topics_[currentConceptIndex_];
    titleText_->SetLabel(toWx(topic.title));
    categoryText_->SetLabel(toWx(topic.category));
    difficultyText_->SetLabel(wxString::Format(wxT("%s%s%s%s%s"),
        topic.difficulty >= 1 ? wxT("★") : wxT("☆"),
        topic.difficulty >= 2 ? wxT("★") : wxT("☆"),
        topic.difficulty >= 3 ? wxT("★") : wxT("☆"),
        topic.difficulty >= 4 ? wxT("★") : wxT("☆"),
        topic.difficulty >= 5 ? wxT("★") : wxT("☆")));

    const wxString topicBaseUrl = TopicBaseUrl(topic.contentBasePath);

    descriptionHtml_->setHtmlBody(
        wxT("<p class='description-lead'>") +
        toWx(topic.description) +
        wxT("</p>"), topicBaseUrl);
    descriptionHtml_->SetMinSize(wxSize(-1, EstimateDescriptionHeight(topic.description)));

    formulaWidget_->setImagePath(toWx(topic.formulaImagePath));
    formulaWidget_->setLatex(toWx(topic.formulaLatex));

    const std::string bodyHtml = markdownRenderer_.toHtml(topic.bodyMarkdown);
    explanationHtml_->setHtmlBody(toWx(bodyHtml), topicBaseUrl);
    explanationHtml_->SetMinSize(wxSize(-1, EstimateBodyHeight(topic.bodyMarkdown, 520, 1500)));

    const std::string exerciseSource = topic.exerciseMarkdown.empty()
        ? std::string("<div class='note-box'><p>Este tema todavía no tiene un ejercicio guiado separado. Por ahora se apoya en la explicación principal.</p></div>")
        : markdownRenderer_.toHtml(topic.exerciseMarkdown);
    exerciseHtml_->setHtmlBody(toWx(exerciseSource), topicBaseUrl);
    exerciseHtml_->SetMinSize(wxSize(-1, EstimateBodyHeight(topic.exerciseMarkdown.empty() ? std::string("pendiente") : topic.exerciseMarkdown, 320, 900)));

    updateFigureForTopic(topic);

    if (auto* scrolled = wxDynamicCast(descriptionHtml_->GetParent(), wxScrolledWindow)) {
        scrolled->Layout();
        scrolled->FitInside();
    }

    auto currentIt = std::find(filteredConceptIndices_.begin(), filteredConceptIndices_.end(), currentConceptIndex_);
    const bool hasCurrent = currentIt != filteredConceptIndices_.end();
    prevBtn_->Enable(hasCurrent && currentIt != filteredConceptIndices_.begin());
    nextBtn_->Enable(hasCurrent && (currentIt + 1) != filteredConceptIndices_.end());
}

void TheoryPanel::jumpToConcept(const wxString& conceptId) {
    const std::string id = std::string(conceptId.ToUTF8());
    for (size_t i = 0; i < topics_.size(); ++i) {
        if (topics_[i].id == id) {
            loadConcept(static_cast<int>(i));
            return;
        }
    }
}

void TheoryPanel::nextConcept() {
    auto it = std::find(filteredConceptIndices_.begin(), filteredConceptIndices_.end(), currentConceptIndex_);
    if (it != filteredConceptIndices_.end() && (it + 1) != filteredConceptIndices_.end()) {
        loadConcept(*(it + 1));
    }
}

void TheoryPanel::previousConcept() {
    auto it = std::find(filteredConceptIndices_.begin(), filteredConceptIndices_.end(), currentConceptIndex_);
    if (it != filteredConceptIndices_.end() && it != filteredConceptIndices_.begin()) {
        loadConcept(*(it - 1));
    }
}

void TheoryPanel::onConceptSelect(wxCommandEvent& event) {
    const int selected = event.GetSelection();
    if (selected >= 0 && selected < static_cast<int>(filteredConceptIndices_.size())) {
        loadConcept(filteredConceptIndices_[selected]);
    }
}

void TheoryPanel::onCategorySelect(wxCommandEvent& event) {
    (void)event;
    rebuildConceptList();
    if (!filteredConceptIndices_.empty()) {
        loadConcept(filteredConceptIndices_.front());
    }
}

void TheoryPanel::onSearch(wxCommandEvent& event) {
    (void)event;
    rebuildConceptList();
    if (!filteredConceptIndices_.empty()) {
        loadConcept(filteredConceptIndices_.front());
    }
}

void TheoryPanel::onNext(wxCommandEvent& event) {
    (void)event;
    nextConcept();
}

void TheoryPanel::onPrevious(wxCommandEvent& event) {
    (void)event;
    previousConcept();
}

void TheoryPanel::setScenarioCallback(std::function<void(const wxString&)> callback) {
    scenarioCallback_ = std::move(callback);
}

bool TheoryPanel::requestScenarioLoad(const wxString& scenarioName) const {
    if (!scenarioCallback_) {
        return false;
    }

    scenarioCallback_(scenarioName);
    return true;
}

} // namespace tp::presentation
