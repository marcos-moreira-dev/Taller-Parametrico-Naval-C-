#pragma once

/**
 * @file MathRenderer.hpp
 * @brief Renderizador de expresiones matemáticas
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Tokeniza y renderiza expresiones matemáticas:
 * - Números y variables
 * - Operadores (+, -, *, /, ^)
 * - Funciones (sin, cos, tan, etc.)
 * - Paréntesis
 * 
 * @see EquationRenderer para renderizado de ecuaciones completas
 */

#include <wx/wx.h>
#include <wx/dcclient.h>
#include <string>
#include <vector>
#include <cmath>

namespace tp::presentation {

// Representa un token matemático (número, variable, operador, etc.)
struct MathToken {
    enum Type {
        NUMBER,
        VARIABLE,
        OPERATOR,
        FUNCTION,
        SUBSCRIPT,
        SUPERSCRIPT,
        FRACTION_BAR,
        PAREN_OPEN,
        PAREN_CLOSE,
        SPECIAL
    };
    
    Type type;
    std::string text;
    double scale;  // Para subíndices y superíndices
    
    MathToken(Type t, const std::string& txt, double s = 1.0) 
        : type(t), text(txt), scale(s) {}
};

// Renderizador simple de ecuaciones matemáticas
class MathRenderer {
public:
    MathRenderer();
    
    // Parsear expresión LaTeX simple
    std::vector<MathToken> parseLatex(const std::string& latex);
    
    // Renderizar ecuación en un DC
    void render(wxDC& dc, const std::string& latex, int x, int y, 
                const wxColour& color = wxColour(0, 0, 128));
    
    // Calcular tamaño necesario
    wxSize getSize(wxDC& dc, const std::string& latex);
    
private:
    int baseFontSize_;
    wxFont baseFont_;
    wxFont subscriptFont_;
    wxFont superscriptFont_;
    wxFont operatorFont_;
    wxFont functionFont_;
    
    void drawToken(wxDC& dc, const MathToken& token, int& x, int y, int baseline);
    wxSize getTokenSize(wxDC& dc, const MathToken& token);
};

// Widget que muestra una ecuación como imagen
class MathEquationPanel : public wxPanel {
public:
    MathEquationPanel(wxWindow* parent, const wxString& equation);
    
    void setEquation(const wxString& equation);
    void setTextColor(const wxColour& color);
    
private:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    
    wxString equation_;
    wxColour textColor_;
    wxBitmap buffer_;
    bool needsRedraw_;
    
    void redrawBuffer();
    
    wxDECLARE_EVENT_TABLE();
};

} // namespace tp::presentation
