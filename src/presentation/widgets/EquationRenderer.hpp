#pragma once

/**
 * @file EquationRenderer.hpp
 * @brief Renderizador de ecuaciones matemáticas
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Renderiza ecuaciones matemáticas en el panel de teoría:
 * - Texto normal
 * - Subíndices y superíndices
 * - Fracciones
 * - Raíces
 * - Integrales (básico)
 * 
 * @see MathRenderer para parsing de expresiones
 */

#include <wx/wx.h>
#include <vector>
#include <string>
#include <memory>

namespace tp::presentation {

// Representa un elemento de una ecuación
struct EquationElement {
    enum Type {
        TEXT,           // Texto normal
        SUBSCRIPT,      // Subíndice
        SUPERSCRIPT,    // Superíndice
        FRACTION,       // Fracción
        SYMBOL,         // Símbolo matemático (∫, ∑, etc.)
        GREEK,          // Letra griega
        OPERATOR,       // Operador (+, -, ×, ÷)
        PAREN_LEFT,     // Paréntesis izquierdo
        PAREN_RIGHT,    // Paréntesis derecho
        ROOT,           // Raíz cuadrada
        BAR             // Barra superior
    };
    
    Type type;
    wxString text;
    wxColour color;
    int fontSize;
    
    EquationElement(Type t, const wxString& txt, const wxColour& c = wxColour(0, 0, 128), int size = 14)
        : type(t), text(txt), color(c), fontSize(size) {}
};

// Renderizador de ecuaciones matemáticas
class EquationRenderer {
public:
    EquationRenderer();
    
    // Parsear expresión matemática simple a elementos
    std::vector<EquationElement> parseExpression(const wxString& expr);
    
    // Renderizar ecuación
    void render(wxDC& dc, const std::vector<EquationElement>& elements, int x, int y);
    
    // Calcular tamaño necesario
    wxSize getSize(wxDC& dc, const std::vector<EquationElement>& elements);
    
    // Versión simplificada: renderizar string directamente
    void renderString(wxDC& dc, const wxString& expr, int x, int y, 
                      const wxColour& color = wxColour(0, 0, 128), int fontSize = 14);
    
    // Convertir LaTeX simple a ecuación formateada
    wxString latexToPlain(const wxString& latex);
    
private:
    wxFont baseFont_;
    wxFont subscriptFont_;
    wxFont superscriptFont_;
    wxFont symbolFont_;
    
    void drawElement(wxDC& dc, const EquationElement& elem, int& x, int y, int baseline);
    int getElementHeight(const EquationElement& elem);
    int getElementWidth(wxDC& dc, const EquationElement& elem);
    
    // Helpers para parsing
    wxString parseSubscript(const wxString& expr, size_t& pos);
    wxString parseSuperscript(const wxString& expr, size_t& pos);
    wxString parseFraction(const wxString& expr, size_t& pos);
    
    // Mapeo de símbolos LaTeX
    wxString latexToUnicode(const wxString& latex);
};

// Widget que muestra una ecuación renderizada
class EquationWidget : public wxPanel {
public:
    EquationWidget(wxWindow* parent, const wxString& equation = wxT(""));
    
    void setEquation(const wxString& equation);
    void setColor(const wxColour& color);
    void setFontSize(int size);
    
private:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    
    EquationRenderer renderer_;
    wxString equation_;
    wxColour color_;
    int fontSize_;
    wxBitmap buffer_;
    bool needsRedraw_;
    
    void renderToBuffer();
    
    wxDECLARE_EVENT_TABLE();
};

// Panel de ejercicios con ecuaciones y gráficas
class ExercisePanel : public wxPanel {
public:
    ExercisePanel(wxWindow* parent);
    
    // Configurar ejercicio
    void setTitle(const wxString& title);
    void setProblem(const wxString& problem);
    void setEquation(const wxString& equation);
    void setGraph(std::function<double(double)> func, double xMin, double xMax);
    void setSolution(const wxString& solution);
    
    void showHint();
    void showSolution();
    void reset();
    
private:
    void setupUI();
    void OnCheckAnswer(wxCommandEvent& event);
    void OnShowHint(wxCommandEvent& event);
    void OnShowSolution(wxCommandEvent& event);
    void OnNextExercise(wxCommandEvent& event);
    
    wxString title_;
    wxString problem_;
    wxString equation_;
    wxString solution_;
    wxString hint_;
    
    wxStaticText* titleLabel_;
    wxStaticText* problemLabel_;
    EquationWidget* equationWidget_;
    wxTextCtrl* answerInput_;
    wxButton* checkBtn_;
    wxButton* hintBtn_;
    wxButton* solutionBtn_;
    wxButton* nextBtn_;
    wxStaticText* feedbackLabel_;
    
    bool hintShown_;
    bool solutionShown_;
    
    wxDECLARE_EVENT_TABLE();
};

} // namespace tp::presentation
