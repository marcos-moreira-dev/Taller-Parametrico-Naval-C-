#include "panels/TheoryPanel.hpp"

#include <wx/button.h>
#include <wx/choice.h>
#include <wx/filesys.h>
#include <wx/filename.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include <algorithm>
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

wxString PathToUrl(const std::filesystem::path& path) {
    wxFileName fn(wxString::FromUTF8(path.string().c_str()));
    return wxFileSystem::FileNameToURL(fn);
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
    , formulaWidget_(nullptr)
    , graphWidget_(nullptr)
    , explanationHtml_(nullptr)
    , prevBtn_(nullptr)
    , nextBtn_(nullptr)
    , currentConceptIndex_(-1)
    , repository_()
    , markdownRenderer_()
    , htmlTemplate_(repository_.webRoot() / "theory_base.html") {
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
        descriptionHtml_->SetPage(U(u8"<html><body style='font-family:Segoe UI, Arial, sans-serif;'>No se encontraron conceptos con ese filtro.</body></html>"));
        formulaWidget_->setLatex(wxEmptyString);
        explanationHtml_->setHtmlBody(U(u8"<p>Prueba otra categoría o una búsqueda más corta.</p>"));
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

    descriptionHtml_ = new wxHtmlWindow(contentPanel, wxID_ANY, wxDefaultPosition, wxSize(-1, 140));
    contentSizer->Add(descriptionHtml_, 0, wxEXPAND | wxALL, 10);

    wxStaticText* formulaLabel = new wxStaticText(contentPanel, wxID_ANY, U(u8"FÓRMULA"));
    formulaLabel->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    contentSizer->Add(formulaLabel, 0, wxLEFT | wxRIGHT | wxTOP, 15);

    formulaPanel_ = new wxPanel(contentPanel);
    formulaPanel_->SetBackgroundColour(wxColour(250, 250, 250));
    formulaPanel_->SetMinSize(wxSize(-1, 250));
    wxBoxSizer* formulaSizer = new wxBoxSizer(wxVERTICAL);
    formulaWidget_ = new LatexBitmapPanel(formulaPanel_, wxEmptyString);
    formulaWidget_->setForegroundColor(wxColour(0, 0, 128));
    formulaSizer->Add(formulaWidget_, 1, wxALL | wxEXPAND, 10);
    formulaPanel_->SetSizer(formulaSizer);
    contentSizer->Add(formulaPanel_, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    wxStaticText* graphLabel = new wxStaticText(contentPanel, wxID_ANY, U(u8"GRÁFICA DEL PROBLEMA"));
    graphLabel->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    contentSizer->Add(graphLabel, 0, wxLEFT | wxRIGHT | wxTOP, 15);

    graphWidget_ = new MathGraph(contentPanel);
    graphWidget_->SetMinSize(wxSize(-1, 500));
    graphWidget_->setLabels(wxT("x"), wxT("y"));
    contentSizer->Add(graphWidget_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxStaticText* explLabel = new wxStaticText(contentPanel, wxID_ANY, U(u8"DESARROLLO Y EJEMPLO"));
    explLabel->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    contentSizer->Add(explLabel, 0, wxLEFT | wxRIGHT | wxTOP, 15);

    explanationHtml_ = new MathHtmlPanel(contentPanel);
    explanationHtml_->SetMinSize(wxSize(-1, 980));
    contentSizer->Add(explanationHtml_, 1, wxEXPAND | wxALL, 10);

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

void TheoryPanel::updateGraphForTopic(const tp::education::TheoryTopic& topic) {
    if (graphWidget_) {
        TheoryGraphFactory::configure(*graphWidget_, topic.graphId);
    }
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

    descriptionHtml_->SetPage(
        wxT("<html><body style='font-family:Segoe UI, Arial, sans-serif; font-size:11pt; margin:8px; line-height:135%;'>") +
        toWx(topic.description) +
        wxT("</body></html>"));

    formulaWidget_->setLatex(toWx(topic.formulaLatex));

    const std::string bodyHtml = markdownRenderer_.toHtml(topic.bodyMarkdown);
    const std::string htmlDocument = htmlTemplate_.render(topic, bodyHtml);
    const wxString baseUrl = PathToUrl(repository_.theoryTemplatePath());
    explanationHtml_->setHtmlDocument(toWx(htmlDocument), baseUrl);

    updateGraphForTopic(topic);

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

/* ============================================================================
 * SECCIÓN: Implementación del Patrón Observer/Callback
 * ============================================================================
 * 
 * Esta sección demuestra un patrón de diseño fundamental en desarrollo de
 * software: el patrón Observer (también llamado Callback o Listener).
 * 
 * PROBLEMA:
 * TheoryPanel necesita notificar a alguien (MainWindow) cuando el usuario
 * quiere cargar un escenario desde la vista de teoría. Pero TheoryPanel
 * no debería depender directamente de MainWindow (acoplamiento fuerte).
 * 
 * SOLUCIÓN:
 * Usamos std::function como un contrato: "Yo te aviso cuando pase X, tú
 * decides qué hacer". MainWindow registra una función que se llamará.
 * 
 * BENEFICIOS:
 * 1. Desacoplamiento: TheoryPanel no sabe de MainWindow
 * 2. Testabilidad: Podemos probar TheoryPanel con mocks
 * 3. Reusabilidad: TheoryPanel funciona en cualquier contexto
 * 4. Cumple SOLID:
 *    - SRP: TheoryPanel solo maneja UI de teoría
 *    - DIP: Depende de abstracción (std::function), no de concreción
 * 
 * ============================================================================ */

void TheoryPanel::setScenarioCallback(std::function<void(const wxString&)> callback) {
    /**
     * @internal Usamos std::move para transferir ownership del callable.
     * Esto es una optimización: evita copiar la lambda si contiene capturas
     * grandes (ej: [this] con objetos pesados).
     * 
     * Si callback es nullptr o vacío, std::function::operator= lo maneja
     * correctamente: scenarioCallback_ queda en estado "vacío" (equivalente
     * a nullptr para punteros a función tradicionales).
     */
    scenarioCallback_ = std::move(callback);
}

bool TheoryPanel::requestScenarioLoad(const wxString& scenarioName) const {
    /**
     * @internal Verificación defensiva: std::function tiene operator bool
     * que retorna false si no tiene un callable asignado. Esto es equivalente
     * a verificar ptr != nullptr para punteros a función.
     * 
     * Este patrón de "verificar antes de llamar" es fundamental para evitar
     * undefined behavior. En C++20 también podríamos usar std::observer_ptr
     * para punteros observadores, pero para callbacks std::function es ideal.
     */
    if (!scenarioCallback_) {
        // No hay callback registrado - esto es válido, no es un error
        // El panel puede funcionar independientemente de si alguien
        // escucha las solicitudes de escenario o no
        return false;
    }
    
    // Invocamos el callback con el nombre del escenario
    // La sintaxis es igual que llamar una función normal - esto es la
    // magna de std::function: unifica la interfaz de cualquier callable
    scenarioCallback_(scenarioName);
    return true;
}

} // namespace tp::presentation
