// MathGraph.cpp - Implementación de gráficas matemáticas estilo matplotlib

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include "MathGraph.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <wx/dcbuffer.h>

namespace tp::presentation {

namespace {

double niceStep(double range, int targetTicks) {
    if (range <= 0.0) {
        return 1.0;
    }
    const double rough = range / std::max(1, targetTicks);
    const double exponent = std::floor(std::log10(rough));
    const double base = std::pow(10.0, exponent);
    const double fraction = rough / base;
    if (fraction <= 1.0) return 1.0 * base;
    if (fraction <= 2.0) return 2.0 * base;
    if (fraction <= 5.0) return 5.0 * base;
    return 10.0 * base;
}

wxString formatTick(double value, double step) {
    int decimals = 0;
    if (step < 1.0) {
        decimals = static_cast<int>(std::ceil(-std::log10(step)));
        decimals = std::clamp(decimals, 0, 4);
    }
    return wxString::Format(wxString::Format(wxT("%%.%df"), decimals), value);
}

} // namespace

wxBEGIN_EVENT_TABLE(MathGraph, wxPanel)
    EVT_PAINT(MathGraph::OnPaint)
    EVT_SIZE(MathGraph::OnSize)
wxEND_EVENT_TABLE()

MathGraph::MathGraph(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
    , xMin_(-10)
    , xMax_(10)
    , yMin_(-10)
    , yMax_(10)
    , showGrid_(true)
    , showAxes_(true)
    , title_(wxT(""))
    , xLabel_(wxT("x"))
    , yLabel_(wxT("y"))
    , hasVectorField_(false) {
    
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(*wxWHITE);
    SetMinSize(wxSize(520, 360));
}

void MathGraph::setTitle(const wxString& title) {
    title_ = title;
    Refresh();
}

void MathGraph::setLabels(const wxString& xLabel, const wxString& yLabel) {
    xLabel_ = xLabel;
    yLabel_ = yLabel;
    Refresh();
}

void MathGraph::setRange(double xMin, double xMax, double yMin, double yMax) {
    xMin_ = xMin;
    xMax_ = xMax;
    yMin_ = yMin;
    yMax_ = yMax;
    Refresh();
}

void MathGraph::clearFunctions() {
    functions_.clear();
    Refresh();
}

void MathGraph::addFunction(const wxString& name, 
                            std::function<double(double)> func,
                            const wxColour& color) {
    functions_.push_back({name, func, color});
    Refresh();
}

void MathGraph::addVectorField(std::function<std::pair<double, double>(double, double)> field,
                               double step) {
    vectorField_ = std::move(field);
    hasVectorField_ = true;
    vectorStep_ = step;
    Refresh();
}

void MathGraph::addTrajectory(const std::vector<std::pair<double, double>>& points,
                              const wxColour& color) {
    trajectories_.push_back({points, color});
    Refresh();
}

void MathGraph::clearTrajectories() {
    trajectories_.clear();
    Refresh();
}

wxPoint MathGraph::worldToScreen(double x, double y) {
    int width = GetClientSize().GetWidth();
    int height = GetClientSize().GetHeight();
    
    int marginLeft = 58;
    int marginRight = 28;
    int marginTop = 48;
    int marginBottom = 46;
    int plotWidth = std::max(1, width - marginLeft - marginRight);
    int plotHeight = std::max(1, height - marginTop - marginBottom);
    
    int screenX = marginLeft + static_cast<int>((x - xMin_) / (xMax_ - xMin_) * plotWidth);
    int screenY = height - marginBottom - static_cast<int>((y - yMin_) / (yMax_ - yMin_) * plotHeight);
    
    return wxPoint(screenX, screenY);
}

void MathGraph::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    
    // Fondo blanco
    dc.SetBackground(wxBrush(*wxWHITE));
    dc.Clear();
    
    if (showGrid_) {
        drawGrid(dc);
    }
    
    if (showAxes_) {
        drawAxes(dc);
    }
    
    drawFunctions(dc);
    
    if (hasVectorField_) {
        drawVectorField(dc);
    }
    
    drawTrajectories(dc);
    
    drawTitle(dc);
    
    drawLegend(dc);
}

void MathGraph::drawGrid(wxDC& dc) {
    dc.SetPen(wxPen(wxColour(230, 230, 230), 1));
    const double xStep = niceStep(xMax_ - xMin_, 10);
    const double yStep = niceStep(yMax_ - yMin_, 10);
    
    // Líneas verticales
    for (double x = std::ceil(xMin_ / xStep) * xStep; x <= xMax_; x += xStep) {
        wxPoint p1 = worldToScreen(x, yMin_);
        wxPoint p2 = worldToScreen(x, yMax_);
        dc.DrawLine(p1.x, p1.y, p2.x, p2.y);
    }
    
    // Líneas horizontales
    for (double y = std::ceil(yMin_ / yStep) * yStep; y <= yMax_; y += yStep) {
        wxPoint p1 = worldToScreen(xMin_, y);
        wxPoint p2 = worldToScreen(xMax_, y);
        dc.DrawLine(p1.x, p1.y, p2.x, p2.y);
    }
}

void MathGraph::drawAxes(wxDC& dc) {
    dc.SetPen(wxPen(wxColour(100, 100, 100), 2));
    const wxSize size = GetClientSize();
    const int marginLeft = 58;
    const int marginRight = 28;
    const int marginTop = 48;
    const int marginBottom = 46;
    
    // Eje X
    wxPoint xStart = worldToScreen(xMin_, 0);
    wxPoint xEnd = worldToScreen(xMax_, 0);
    if (0 >= yMin_ && 0 <= yMax_) {
        dc.DrawLine(xStart.x, xStart.y, xEnd.x, xEnd.y);
        
        // Flecha
        dc.DrawLine(xEnd.x, xEnd.y, xEnd.x - 10, xEnd.y - 5);
        dc.DrawLine(xEnd.x, xEnd.y, xEnd.x - 10, xEnd.y + 5);
    }
    
    // Eje Y
    wxPoint yStart = worldToScreen(0, yMin_);
    wxPoint yEnd = worldToScreen(0, yMax_);
    if (0 >= xMin_ && 0 <= xMax_) {
        dc.DrawLine(yStart.x, yStart.y, yEnd.x, yEnd.y);
        
        // Flecha
        dc.DrawLine(yEnd.x, yEnd.y, yEnd.x - 5, yEnd.y + 10);
        dc.DrawLine(yEnd.x, yEnd.y, yEnd.x + 5, yEnd.y + 10);
    }
    
    // Etiquetas de ejes
    dc.SetFont(wxFont(wxFontInfo(10).Family(wxFONTFAMILY_MODERN)));
    dc.SetTextForeground(wxColour(80, 80, 80));
    
    // Etiquetas X
    double xStep = niceStep(xMax_ - xMin_, 5);
    for (double x = std::ceil(xMin_ / xStep) * xStep; x <= xMax_; x += xStep) {
        if (std::abs(x) > xStep / 10) {
            wxPoint pos = worldToScreen(x, 0);
            wxString label = formatTick(x, xStep);
            dc.DrawText(label, pos.x - 10, std::min(pos.y + 5, size.GetHeight() - marginBottom + 4));
        }
    }
    
    // Etiquetas Y
    double yStep = niceStep(yMax_ - yMin_, 5);
    for (double y = std::ceil(yMin_ / yStep) * yStep; y <= yMax_; y += yStep) {
        if (std::abs(y) > yStep / 10) {
            wxPoint pos = worldToScreen(0, y);
            wxString label = formatTick(y, yStep);
            const int textX = std::max(6, std::min(pos.x + 5, marginLeft - 28));
            dc.DrawText(label, textX, pos.y - 8);
        }
    }
    
    // Etiquetas de ejes
    wxSize xLabelSize = dc.GetTextExtent(xLabel_);
    dc.DrawText(xLabel_, size.GetWidth() - marginRight - xLabelSize.GetWidth(), size.GetHeight() - 24);
    dc.DrawRotatedText(yLabel_, 14, marginTop + 20, 90);
}

void MathGraph::drawFunctions(wxDC& dc) {
    wxSize size = GetClientSize();
    dc.SetClippingRegion(58, 48, std::max(1, size.GetWidth() - 86), std::max(1, size.GetHeight() - 94));
    for (const auto& func : functions_) {
        dc.SetPen(wxPen(func.color, 2));
        
        double step = (xMax_ - xMin_) / 500; // 500 puntos
        bool first = true;
        int prevX = 0, prevY = 0;
        
        for (double x = xMin_; x <= xMax_; x += step) {
            double y = func.func(x);
            
            if (y >= yMin_ && y <= yMax_) {
                wxPoint pos = worldToScreen(x, y);
                
                if (!first) {
                    dc.DrawLine(prevX, prevY, pos.x, pos.y);
                }
                
                prevX = pos.x;
                prevY = pos.y;
                first = false;
            } else {
                first = true;
            }
        }
    }
    dc.DestroyClippingRegion();
}

void MathGraph::drawVectorField(wxDC& dc) {
    if (!hasVectorField_) return;

    wxSize size = GetClientSize();
    dc.SetClippingRegion(58, 48, std::max(1, size.GetWidth() - 86), std::max(1, size.GetHeight() - 94));

    std::vector<std::tuple<double, double, double, double, double>> samples;
    double maxMagnitude = 0.0;
    for (double x = xMin_; x <= xMax_; x += vectorStep_) {
        for (double y = yMin_; y <= yMax_; y += vectorStep_) {
            auto vec = vectorField_(x, y);
            const double mag = std::sqrt(vec.first * vec.first + vec.second * vec.second);
            samples.emplace_back(x, y, vec.first, vec.second, mag);
            maxMagnitude = std::max(maxMagnitude, mag);
        }
    }
    if (maxMagnitude < 1e-9) {
        dc.DestroyClippingRegion();
        return;
    }
    
    for (const auto& sample : samples) {
            double x = std::get<0>(sample);
            double y = std::get<1>(sample);
            double vx = std::get<2>(sample);
            double vy = std::get<3>(sample);
            double mag = std::get<4>(sample);
            const double referenceMagnitude = 10.0;
            const double normalized = std::clamp(mag / referenceMagnitude, 0.0, 1.0);
            const wxColour color(
                static_cast<unsigned char>(140 + normalized * 115.0),
                static_cast<unsigned char>(200 + normalized * 55.0),
                255,
                255);
            dc.SetPen(wxPen(color, normalized < 0.4 ? 1 : 2));

            wxPoint start = worldToScreen(x, y);
            wxPoint end = worldToScreen(x + vx * 0.55, y + vy * 0.55);
            
            dc.DrawLine(start.x, start.y, end.x, end.y);
            
            // Flecha
            double angle = std::atan2(end.y - start.y, end.x - start.x);
            int arrowX = end.x - 9 * std::cos(angle - 0.5);
            int arrowY = end.y - 9 * std::sin(angle - 0.5);
            dc.DrawLine(end.x, end.y, arrowX, arrowY);
            
            arrowX = end.x - 9 * std::cos(angle + 0.5);
            arrowY = end.y - 9 * std::sin(angle + 0.5);
            dc.DrawLine(end.x, end.y, arrowX, arrowY);
    }
    dc.DestroyClippingRegion();
}

void MathGraph::drawTrajectories(wxDC& dc) {
    wxSize size = GetClientSize();
    dc.SetClippingRegion(58, 48, std::max(1, size.GetWidth() - 86), std::max(1, size.GetHeight() - 94));
    for (const auto& traj : trajectories_) {
        if (traj.points.size() < 2) continue;
        
        dc.SetPen(wxPen(traj.color, 3, wxPENSTYLE_SOLID));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        
        for (size_t i = 1; i < traj.points.size(); ++i) {
            wxPoint p1 = worldToScreen(traj.points[i-1].first, traj.points[i-1].second);
            wxPoint p2 = worldToScreen(traj.points[i].first, traj.points[i].second);
            dc.DrawLine(p1.x, p1.y, p2.x, p2.y);
        }
        
        // Puntos
        dc.SetBrush(wxBrush(traj.color));
        for (const auto& point : traj.points) {
            wxPoint pos = worldToScreen(point.first, point.second);
            dc.DrawCircle(pos.x, pos.y, 3);
        }
    }
    dc.DestroyClippingRegion();
}

void MathGraph::drawTitle(wxDC& dc) {
    if (title_.IsEmpty()) return;
    
    dc.SetFont(wxFont(wxFontInfo(13).Family(wxFONTFAMILY_MODERN).Bold()));
    dc.SetTextForeground(wxColour(50, 50, 50));
    
    wxSize size = GetClientSize();
    wxSize textSize = dc.GetTextExtent(title_);
    
    dc.DrawText(title_, (size.GetWidth() - textSize.GetWidth()) / 2, 12);
}

void MathGraph::drawLegend(wxDC& dc) {
    if (functions_.empty()) return;
    
    dc.SetFont(wxFont(wxFontInfo(10).Family(wxFONTFAMILY_MODERN)));
    
    int x = GetClientSize().GetWidth() - 170;
    int y = 52;
    int lineHeight = 20;
    int legendWidth = 155;
    
    // Fondo
    dc.SetPen(wxPen(wxColour(200, 200, 200)));
    dc.SetBrush(wxBrush(wxColour(255, 255, 255, 230)));
    dc.DrawRoundedRectangle(x - 10, y - 5, legendWidth, functions_.size() * lineHeight + 10, 6);
    
    // Leyendas
    for (const auto& func : functions_) {
        dc.SetPen(wxPen(func.color, 3));
        dc.DrawLine(x, y + 8, x + 20, y + 8);
        
        dc.SetTextForeground(wxColour(50, 50, 50));
        dc.DrawText(func.name, x + 25, y);
        
        y += lineHeight;
    }
}

void MathGraph::OnSize(wxSizeEvent& event) {
    event.Skip();
    Refresh();
}

} // namespace tp::presentation
