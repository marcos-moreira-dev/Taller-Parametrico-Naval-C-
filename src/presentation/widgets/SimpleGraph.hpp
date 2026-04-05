#pragma once

/**
 * @file SimpleGraph.hpp
 * @brief Widget de gráficas simple para datos de simulación
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Muestra gráficos de línea en tiempo real para:
 * - Velocidad vs tiempo
 * - Energía vs tiempo
 * - Posición vs tiempo
 * 
 * @see BottomPanel para integración en UI
 */

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include <wx/wx.h>
#include <vector>

namespace tp::presentation {

// Estructura para punto de datos
struct DataPoint {
    double time;
    double value;
};

// Widget simple de gráficas
class SimpleGraph : public wxPanel {
public:
    SimpleGraph(wxWindow* parent, const wxString& title);
    
    void addDataPoint(double time, double value);
    void clearData();
    void setYRange(double min, double max);
    void setColor(const wxColour& color);
    
private:
    static constexpr size_t MAX_POINTS = 600;
    void onPaint(wxPaintEvent& event);
    void drawGraph(wxDC& dc);
    
    wxString title_;
    std::vector<DataPoint> dataPoints_;
    double yMin_;
    double yMax_;
    wxColour graphColor_;
    
    wxDECLARE_EVENT_TABLE();
};

}
