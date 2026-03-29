#pragma once

/**
 * @file EquationPanel.hpp
 * @brief Panel para mostrar ecuaciones LaTeX
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Panel especializado en mostrar ecuaciones:
 * - Código LaTeX
 * - Versión legible alternativa
 * - Personalización de fuente y color
 * 
 * @see EquationRenderer para renderizado
 */

#include <wx/wx.h>
#include <vector>
#include <string>

namespace tp::presentation {

// Estructura para representar una ecuación LaTeX renderizable
struct LatexEquation {
    std::string latex;      // Código LaTeX
    std::string plain;      // Versión legible
    int fontSize;
    wxColour color;
    
    LatexEquation(const std::string& l, const std::string& p, int size = 14, 
                  const wxColour& c = wxColour(0, 0, 128))
        : latex(l), plain(p), fontSize(size), color(c) {}
};

// Panel que renderiza ecuaciones matemáticas
class EquationPanel : public wxPanel {
public:
    EquationPanel(wxWindow* parent, const LatexEquation& equation);
    
    void setEquation(const LatexEquation& equation);
    
private:
    void OnPaint(wxPaintEvent& event);
    void renderEquation(wxDC& dc);
    
    LatexEquation equation_;
    wxBitmap buffer_;
    bool needsRedraw_;
    
    wxDECLARE_EVENT_TABLE();
};

// Panel que dibuja gráficas de funciones
class GraphPanel : public wxPanel {
public:
    GraphPanel(wxWindow* parent, const std::string& function);
    
    void setFunction(const std::string& func);
    void setRange(double xMin, double xMax, double yMin, double yMax);
    void addVectorField(bool show) { showVectors_ = show; Refresh(); }
    void addTrajectory(const std::vector<std::pair<double, double>>& points);
    
private:
    void OnPaint(wxPaintEvent& event);
    void drawAxes(wxDC& dc);
    void drawFunction(wxDC& dc);
    void drawVectorField(wxDC& dc);
    void drawTrajectory(wxDC& dc);
    
    std::string function_;
    double xMin_, xMax_, yMin_, yMax_;
    bool showVectors_;
    std::vector<std::pair<double, double>> trajectory_;
    
    double scaleX_, scaleY_;
    int offsetX_, offsetY_;
    
    wxPoint worldToScreen(double x, double y);
    
    wxDECLARE_EVENT_TABLE();
};

} // namespace tp::presentation
