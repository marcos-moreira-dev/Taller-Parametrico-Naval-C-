#include "widgets/SimpleGraph.hpp"
#include <wx/dcbuffer.h>

namespace tp::presentation {

wxBEGIN_EVENT_TABLE(SimpleGraph, wxPanel)
    EVT_PAINT(SimpleGraph::onPaint)
wxEND_EVENT_TABLE()

SimpleGraph::SimpleGraph(wxWindow* parent, const wxString& title)
    : wxPanel(parent, wxID_ANY)
    , title_(title)
    , yMin_(0.0)
    , yMax_(10.0)
    , graphColor_(wxColour(0, 100, 200))
{
    SetBackgroundColour(wxColour(255, 255, 255));
    SetMinSize(wxSize(200, 120));
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void SimpleGraph::addDataPoint(double time, double value)
{
    DataPoint point;
    point.time = time;
    point.value = value;
    dataPoints_.push_back(point);

    if (dataPoints_.size() > SimpleGraph::MAX_POINTS) {
        dataPoints_.erase(dataPoints_.begin(), dataPoints_.begin() + (dataPoints_.size() - SimpleGraph::MAX_POINTS));
    }
    
    // Auto-escalar eje Y si es necesario
    if (value < yMin_) yMin_ = value * 1.1;
    if (value > yMax_) yMax_ = value * 1.1;
    
    Refresh();
}

void SimpleGraph::clearData()
{
    dataPoints_.clear();
    yMin_ = 0.0;
    yMax_ = 10.0;
    Refresh();
}

void SimpleGraph::setYRange(double min, double max)
{
    yMin_ = min;
    yMax_ = max;
    Refresh();
}

void SimpleGraph::setColor(const wxColour& color)
{
    graphColor_ = color;
    Refresh();
}

void SimpleGraph::onPaint(wxPaintEvent& event)
{
    (void)event;
    wxAutoBufferedPaintDC dc(this);
    drawGraph(dc);
}

void SimpleGraph::drawGraph(wxDC& dc)
{
    wxSize size = GetClientSize();
    int width = size.GetWidth();
    int height = size.GetHeight();
    
    // Margen
    int marginLeft = 50;
    int marginRight = 20;
    int marginTop = 30;
    int marginBottom = 30;
    
    int graphWidth = width - marginLeft - marginRight;
    int graphHeight = height - marginTop - marginBottom;
    if (graphWidth <= 0 || graphHeight <= 0) {
        return;
    }
    
    // Fondo blanco
    dc.SetBrush(wxBrush(wxColour(255, 255, 255)));
    dc.SetPen(wxPen(wxColour(255, 255, 255)));
    dc.DrawRectangle(0, 0, width, height);
    
    // Título
    dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    dc.SetTextForeground(wxColour(0, 0, 0));
    dc.DrawText(title_, 10, 5);
    
    if (dataPoints_.empty()) {
        dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        dc.SetTextForeground(wxColour(150, 150, 150));
        dc.DrawText(wxT("Sin datos"), marginLeft + 10, marginTop + graphHeight/2);
        return;
    }
    
    // Encontrar rangos
    double tMin = dataPoints_.front().time;
    double tMax = dataPoints_.back().time;
    if (tMin == tMax) tMax = tMin + 1.0;
    
    // Dibujar ejes
    dc.SetPen(wxPen(wxColour(200, 200, 200)));
    dc.DrawLine(marginLeft, marginTop, marginLeft, height - marginBottom);
    dc.DrawLine(marginLeft, height - marginBottom, width - marginRight, height - marginBottom);
    
    // Dibujar grid
    dc.SetPen(wxPen(wxColour(240, 240, 240)));
    for (int i = 1; i < 5; ++i) {
        int y = marginTop + (graphHeight * i) / 5;
        dc.DrawLine(marginLeft, y, width - marginRight, y);
    }
    
    // Dibujar línea de datos
    dc.SetPen(wxPen(graphColor_, 2));
    dc.SetClippingRegion(marginLeft, marginTop, graphWidth + 1, graphHeight + 1);

    const double yRange = (std::abs(yMax_ - yMin_) < 1e-9) ? 1.0 : (yMax_ - yMin_);
    
    for (size_t i = 1; i < dataPoints_.size(); ++i) {
        double t1 = dataPoints_[i-1].time;
        double v1 = dataPoints_[i-1].value;
        double t2 = dataPoints_[i].time;
        double v2 = dataPoints_[i].value;
        
        int x1 = marginLeft + (int)((t1 - tMin) / (tMax - tMin) * graphWidth);
        int y1 = height - marginBottom - (int)((v1 - yMin_) / yRange * graphHeight);
        int x2 = marginLeft + (int)((t2 - tMin) / (tMax - tMin) * graphWidth);
        int y2 = height - marginBottom - (int)((v2 - yMin_) / yRange * graphHeight);

        x1 = std::clamp(x1, marginLeft, width - marginRight);
        x2 = std::clamp(x2, marginLeft, width - marginRight);
        y1 = std::clamp(y1, marginTop, height - marginBottom);
        y2 = std::clamp(y2, marginTop, height - marginBottom);
        
        dc.DrawLine(x1, y1, x2, y2);
    }
    dc.DestroyClippingRegion();
    
    // Dibujar etiquetas de ejes
    dc.SetFont(wxFont(7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    dc.SetTextForeground(wxColour(100, 100, 100));
    
    // Eje Y
    wxString yLabel;
    yLabel.Printf(wxT("%.1f"), yMax_);
    dc.DrawText(yLabel, 5, marginTop);
    yLabel.Printf(wxT("%.1f"), (yMax_ + yMin_) / 2);
    dc.DrawText(yLabel, 5, marginTop + graphHeight/2);
    yLabel.Printf(wxT("%.1f"), yMin_);
    dc.DrawText(yLabel, 5, height - marginBottom - 10);
    
    // Eje X (tiempo)
    wxString xLabel;
    xLabel.Printf(wxT("t=%.1f"), tMax);
    dc.DrawText(xLabel, width - marginRight - 30, height - marginBottom + 5);
}

}
