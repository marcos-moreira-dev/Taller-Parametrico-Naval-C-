#pragma once

/**
 * @file TheoryPanel.hpp
 * @brief Panel de teoría educativa
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Panel que muestra contenido educativo sobre física de fluidos:
 * - Catálogo de conceptos teóricos
 * - Renderizado de fórmulas matemáticas
 * - Navegación entre conceptos
 * - Integración con escenarios de simulación
 * 
 * @see TheoryCatalog para el contenido
 * @see EquationRenderer para fórmulas
 */

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include <wx/wx.h>
#include <wx/html/htmlwin.h>
#include <vector>
#include <functional>
#include "../../education/TheoryRepository.hpp"
#include "../../education/TheoryMarkdownRenderer.hpp"
#include "../../education/TheoryHtmlTemplate.hpp"
#include "../widgets/MathGraph.hpp"
#include "../widgets/MathHtmlPanel.hpp"
#include "../widgets/LatexBitmapPanel.hpp"
#include "../widgets/TheoryGraphFactory.hpp"

namespace tp::presentation {

class TheoryPanel : public wxPanel {
public:
    TheoryPanel(wxWindow* parent);
    
    void loadConcept(int index);
    void nextConcept();
    void previousConcept();
    void jumpToConcept(const wxString& conceptId);
    void setScenarioCallback(std::function<void(const wxString&)> callback) { (void)callback; }
     
private:
    void setupUI();
    void loadTopics();
    void updateDisplay();
    void rebuildConceptList();
    void updateGraphForTopic(const tp::education::TheoryTopic& topic);
    void onConceptSelect(wxCommandEvent& event);
    void onCategorySelect(wxCommandEvent& event);
    void onNext(wxCommandEvent& event);
    void onPrevious(wxCommandEvent& event);
    void onSearch(wxCommandEvent& event);
    wxString toWx(const std::string& utf8) const;
    
    // Controles
    wxListBox* conceptList_;
    wxChoice* categoryChoice_;
    wxTextCtrl* searchCtrl_;
    wxStaticText* titleText_;
    wxStaticText* categoryText_;
    wxStaticText* difficultyText_;
    wxHtmlWindow* descriptionHtml_;
    wxPanel* formulaPanel_;
    LatexBitmapPanel* formulaWidget_;
    MathGraph* graphWidget_;
    MathHtmlPanel* explanationHtml_;
    wxButton* prevBtn_;
    wxButton* nextBtn_;
     
    // Datos
    std::vector<tp::education::TheoryTopic> topics_;
    std::vector<int> filteredConceptIndices_;
    int currentConceptIndex_;

    tp::education::TheoryRepository repository_;
    tp::education::TheoryMarkdownRenderer markdownRenderer_;
    tp::education::TheoryHtmlTemplate htmlTemplate_;
     
    wxDECLARE_EVENT_TABLE();
};

}
