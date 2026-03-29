#pragma once

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

/**
 * @file MathGraph.hpp
 * @brief Widget para graficar funciones matemáticas
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Muestra gráficas de funciones matemáticas:
 * - Funciones custom (std::function)
 * - Múltiples funciones superpuestas
 * - Zoom y pan
 * - Ejes y grid
 * 
 * @see SimpleGraph para gráficas de datos
 */

#include <wx/wx.h>
#include <vector>
#include <functional>

namespace tp::presentation {

// Estructura para funciones a graficar
struct GraphFunction {
    wxString name;
    std::function<double(double)> func;
    wxColour color;
    
    GraphFunction(const wxString& n, std::function<double(double)> f, const wxColour& c)
        : name(n), func(f), color(c) {}
};

// Estructura para trayectorias
struct Trajectory {
    std::vector<std::pair<double, double>> points;
    wxColour color;
    
    Trajectory(const std::vector<std::pair<double, double>>& p, const wxColour& c)
        : points(p), color(c) {}
};

// Widget de gráficas matemáticas estilo matplotlib
class MathGraph : public wxPanel {
public:
    MathGraph(wxWindow* parent);
    
    // Configuración
    void setTitle(const wxString& title);
    void setLabels(const wxString& xLabel, const wxString& yLabel);
    void setRange(double xMin, double xMax, double yMin, double yMax);
    void showGrid(bool show) { showGrid_ = show; Refresh(); }
    void showAxes(bool show) { showAxes_ = show; Refresh(); }
    
    // Funciones
    void clearFunctions();
    void addFunction(const wxString& name, std::function<double(double)> func, 
                     const wxColour& color = wxColour(255, 0, 0));
    
    // Campos vectoriales
    void addVectorField(std::function<std::pair<double, double>(double, double)> field,
                        double step = 1.0);
    void clearVectorField() { hasVectorField_ = false; Refresh(); }
    
    // Trayectorias
    void addTrajectory(const std::vector<std::pair<double, double>>& points,
                       const wxColour& color = wxColour(0, 0, 255));
    void clearTrajectories();
    
    // Utilidades
    wxPoint worldToScreen(double x, double y);
    
private:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    
    void drawGrid(wxDC& dc);
    void drawAxes(wxDC& dc);
    void drawFunctions(wxDC& dc);
    void drawVectorField(wxDC& dc);
    void drawTrajectories(wxDC& dc);
    void drawTitle(wxDC& dc);
    void drawLegend(wxDC& dc);
    
    // Rango
    double xMin_, xMax_, yMin_, yMax_;
    
    // Opciones de visualización
    bool showGrid_;
    bool showAxes_;
    
    // Títulos
    wxString title_;
    wxString xLabel_;
    wxString yLabel_;
    
    // Datos
    std::vector<GraphFunction> functions_;
    std::function<std::pair<double, double>(double, double)> vectorField_;
    bool hasVectorField_;
    double vectorStep_;
    std::vector<Trajectory> trajectories_;
    
    wxDECLARE_EVENT_TABLE();
};

} // namespace tp::presentation
