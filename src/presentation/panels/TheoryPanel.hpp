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
 * @see LatexBitmapPanel para fórmulas renderizadas con KaTeX
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
    
    /**
     * @brief Establece el callback para solicitudes de carga de escenarios
     * @param callback Función a invocar cuando el usuario solicita cargar un escenario
     *                 desde el panel de teoría. Recibe el nombre del escenario.
     * 
     * @details Este método implementa el patrón Observer (también conocido como
     *          patrón Callback o Publish-Subscribe). En lugar de que TheoryPanel
     *          dependa directamente de MainWindow (acoplamiento fuerte), usamos
     *          este callback para mantener un acoplamiento débil.
     * 
     *          Patrón utilizado: Inversión de Dependencia (Dependency Inversion)
     *          - TheoryPanel no sabe quién implementa el callback
     *          - Solo sabe "cuando ocurra X, llamo a esta función"
     *          - MainWindow (o cualquier otro) proporciona la implementación
     * 
     *          Ventajas de este diseño:
     *          1. TheoryPanel es reutilizable en otros contextos
     *          2. Facilita testing unitario (podemos pasar un mock)
     *          3. Evita dependencias circulares de includes
     *          4. Cumple con el Principio de Responsabilidad Única (SRP)
     * 
     * @note El callback se almacena como std::function, lo que permite:
     *       - Lambdas (como se usa en MainWindow)
     *       - Functores
     *       - std::bind
     *       - Funciones libres
     * 
     * @example
     *   // En MainWindow:
     *   theoryPanel->setScenarioCallback([this](const wxString& name) {
     *       this->loadScenarioByName(name);
     *   });
     * 
     * @see https://en.cppreference.com/w/cpp/utility/functional/function
     */
    void setScenarioCallback(std::function<void(const wxString&)> callback);
    
    /**
     * @brief Solicita la carga de un escenario mediante el callback registrado
     * @param scenarioName Nombre del escenario a cargar
     * @return true si el callback fue invocado, false si no hay callback registrado
     * 
     * @details Este método demuestra el uso de null-checks defensivos.
     *          Siempre verificamos si el callback existe antes de llamarlo.
     */
    bool requestScenarioLoad(const wxString& scenarioName) const;
     
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
    
    /**
     * @brief Callback para solicitudes de carga de escenarios
     * @details Almacena la función proporcionada por setScenarioCallback().
     *          Usamos std::function en lugar de un puntero a función crudo porque:
     *          1. Puede capturar contexto (lambdas con [this])
     *          2. Es type-safe en tiempo de compilación
     *          3. Permite std::bind para partial application
     *          4. Se puede verificar si está vacío (nullptr check implícito)
     * 
     * @note Inicializado a nullptr implícitamente (std::function vacío)
     * @note No es necesario mutex porque wxWidgets es single-threaded para UI
     */
    std::function<void(const wxString&)> scenarioCallback_;
     
    wxDECLARE_EVENT_TABLE();
};

}
