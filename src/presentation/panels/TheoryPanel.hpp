#pragma once

/**
 * @file TheoryPanel.hpp
 * @brief Panel de teoría educativa
 * @author Proyecto Taller Paramétrico
 * @version 1.1.0
 */

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include <wx/wx.h>
#include <wx/statbmp.h>
#include <wx/scrolwin.h>
#include <vector>
#include <functional>
#include "../../education/TheoryRepository.hpp"
#include "../../education/TheoryMarkdownRenderer.hpp"
#include "../widgets/MathHtmlPanel.hpp"
#include "../widgets/LatexBitmapPanel.hpp"

namespace tp::presentation {

class TheoryPanel : public wxPanel {
public:
    TheoryPanel(wxWindow* parent);

    void loadConcept(int index);
    void nextConcept();
    void previousConcept();
    void jumpToConcept(const wxString& conceptId);
    void setScenarioCallback(std::function<void(const wxString&)> callback);
    bool requestScenarioLoad(const wxString& scenarioName) const;

private:
    void setupUI();
    void loadTopics();
    void updateDisplay();
    void rebuildConceptList();
    void updateFigureForTopic(const tp::education::TheoryTopic& topic);
    void onConceptSelect(wxCommandEvent& event);
    void onCategorySelect(wxCommandEvent& event);
    void onNext(wxCommandEvent& event);
    void onPrevious(wxCommandEvent& event);
    void onSearch(wxCommandEvent& event);
    wxString toWx(const std::string& utf8) const;

    wxListBox* conceptList_;
    wxChoice* categoryChoice_;
    wxTextCtrl* searchCtrl_;
    wxStaticText* titleText_;
    wxStaticText* categoryText_;
    wxStaticText* difficultyText_;
    MathHtmlPanel* descriptionHtml_;
    wxPanel* formulaPanel_;
    wxScrolledWindow* formulaScroll_;
    LatexBitmapPanel* formulaWidget_;
    wxPanel* figurePanel_;
    wxStaticBitmap* figureBitmap_;
    MathHtmlPanel* figureCaptionHtml_;
    MathHtmlPanel* explanationHtml_;
    MathHtmlPanel* exerciseHtml_;
    wxButton* prevBtn_;
    wxButton* nextBtn_;

    std::vector<tp::education::TheoryTopic> topics_;
    std::vector<int> filteredConceptIndices_;
    int currentConceptIndex_;

    tp::education::TheoryRepository repository_;
    tp::education::TheoryMarkdownRenderer markdownRenderer_;
    std::function<void(const wxString&)> scenarioCallback_;

    wxDECLARE_EVENT_TABLE();
};

}
