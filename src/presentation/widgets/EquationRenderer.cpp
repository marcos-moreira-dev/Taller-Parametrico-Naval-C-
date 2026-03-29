// EquationRenderer.cpp - Implementacion de renderizado de ecuaciones

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include "EquationRenderer.hpp"
#include <cmath>
#include <sstream>
#include <map>
#include <wx/dcbuffer.h>

namespace tp::presentation {

wxBEGIN_EVENT_TABLE(EquationWidget, wxPanel)
    EVT_PAINT(EquationWidget::OnPaint)
    EVT_SIZE(EquationWidget::OnSize)
wxEND_EVENT_TABLE()

EquationRenderer::EquationRenderer()
    : baseFont_(wxFontInfo(14).Family(wxFONTFAMILY_MODERN))
    , subscriptFont_(wxFontInfo(10).Family(wxFONTFAMILY_MODERN))
    , superscriptFont_(wxFontInfo(10).Family(wxFONTFAMILY_MODERN))
    , symbolFont_(wxFontInfo(14).Family(wxFONTFAMILY_MODERN)) {
}

std::vector<EquationElement> EquationRenderer::parseExpression(const wxString& expr) {
    std::vector<EquationElement> elements;
    size_t i = 0;
    
    while (i < expr.length()) {
        wxChar c = expr[i];
        
        // Detectar subíndices: _{...} o _x
        if (c == wxT('_') && i + 1 < expr.length()) {
            i++;
            if (i < expr.length() && expr[i] == wxT('{')) {
                // Forma _{abc}
                size_t end = expr.find(wxT('}'), i);
                if (end != wxString::npos) {
                    wxString sub = expr.substr(i + 1, end - i - 1);
                    elements.push_back(EquationElement(EquationElement::SUBSCRIPT, sub));
                    i = end + 1;
                    continue;
                }
            } else {
                // Forma _x (un solo carácter)
                wxString sub = expr.substr(i, 1);
                elements.push_back(EquationElement(EquationElement::SUBSCRIPT, sub));
                i++;
                continue;
            }
        }
        
        // Detectar superíndices: ^{...} o ^2
        if (c == wxT('^') && i + 1 < expr.length()) {
            i++;
            if (i < expr.length() && expr[i] == wxT('{')) {
                size_t end = expr.find(wxT('}'), i);
                if (end != wxString::npos) {
                    wxString sup = expr.substr(i + 1, end - i - 1);
                    elements.push_back(EquationElement(EquationElement::SUPERSCRIPT, sup));
                    i = end + 1;
                    continue;
                }
            } else {
                wxString sup = expr.substr(i, 1);
                elements.push_back(EquationElement(EquationElement::SUPERSCRIPT, sup));
                i++;
                continue;
            }
        }
        
        // Detectar fracciones: \frac{a}{b}
        if (expr.substr(i, 6) == wxT("\\frac{")) {
            i += 6;
            size_t numEnd = expr.find(wxT("}{"), i);
            size_t denEnd = expr.find(wxT("}"), numEnd + 2);
            if (numEnd != wxString::npos && denEnd != wxString::npos) {
                wxString num = expr.substr(i, numEnd - i);
                wxString den = expr.substr(numEnd + 2, denEnd - (numEnd + 2));
                elements.push_back(EquationElement(EquationElement::FRACTION, num + wxT("|") + den));
                i = denEnd + 1;
                continue;
            }
        }
        
        // Detectar raíces: \sqrt{x}
        if (expr.substr(i, 6) == wxT("\\sqrt{")) {
            i += 6;
            size_t end = expr.find(wxT("}"), i);
            if (end != wxString::npos) {
                wxString content = expr.substr(i, end - i);
                elements.push_back(EquationElement(EquationElement::ROOT, content));
                i = end + 1;
                continue;
            }
        }
        
        // Detectar símbolos griegos
        if (c == wxT('\\') && i + 1 < expr.length()) {
            size_t end = expr.find_first_of(wxT(" {}^_+-*/=<>"), i + 1);
            if (end == wxString::npos) end = expr.length();
            wxString cmd = expr.substr(i + 1, end - (i + 1));
            
            // Mapear comandos LaTeX a Unicode
            wxString greek = latexToUnicode(cmd);
            if (!greek.IsEmpty()) {
                elements.push_back(EquationElement(EquationElement::GREEK, greek));
                i = end;
                continue;
            }
        }
        
        // Detectar operadores
        if (wxString(wxT("+-*/=<>")).Find(c) != wxNOT_FOUND) {
            elements.push_back(EquationElement(EquationElement::OPERATOR, wxString(c)));
            i++;
            continue;
        }
        
        // Detectar paréntesis
        if (c == wxT('(') || c == wxT('[') || c == wxT('{')) {
            elements.push_back(EquationElement(EquationElement::PAREN_LEFT, wxString(c)));
            i++;
            continue;
        }
        if (c == wxT(')') || c == wxT(']') || c == wxT('}')) {
            elements.push_back(EquationElement(EquationElement::PAREN_RIGHT, wxString(c)));
            i++;
            continue;
        }
        
        // Texto normal (acumular)
        wxString text;
        while (i < expr.length() && 
               expr[i] != wxT('_') && 
               expr[i] != wxT('^') && 
               expr[i] != wxT('\\') &&
               wxString(wxT("+-*/=<>()[]{}")).Find(expr[i]) == wxNOT_FOUND) {
            text += expr[i];
            i++;
        }
        
        if (!text.IsEmpty()) {
            elements.push_back(EquationElement(EquationElement::TEXT, text));
        }
    }
    
    return elements;
}

void EquationRenderer::render(wxDC& dc, const std::vector<EquationElement>& elements, int x, int y) {
    int currentX = x;
    int baseline = y + 20; // Línea base
    
    for (const auto& elem : elements) {
        drawElement(dc, elem, currentX, y, baseline);
    }
}

wxSize EquationRenderer::getSize(wxDC& dc, const std::vector<EquationElement>& elements) {
    int width = 0;
    int height = 30; // Altura base
    
    for (const auto& elem : elements) {
        switch (elem.type) {
            case EquationElement::FRACTION:
                height = std::max(height, 50);
                width += 10;
                break;
            case EquationElement::ROOT:
                height = std::max(height, 35);
                width += 10;
                break;
            default:
                dc.SetFont(baseFont_);
                wxSize size = dc.GetTextExtent(elem.text);
                width += size.GetWidth() + 2;
                height = std::max(height, size.GetHeight());
                break;
        }
    }
    
    return wxSize(width + 10, height + 10);
}

void EquationRenderer::renderString(wxDC& dc, const wxString& expr, int x, int y, 
                                    const wxColour& color, int fontSize) {
    wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_MODERN));
    dc.SetFont(font);
    dc.SetTextForeground(color);
    
    // Procesamiento básico de subíndices y superíndices
    int currentX = x;
    int currentY = y;
    size_t i = 0;
    
    while (i < expr.length()) {
        wxChar c = expr[i];
        
        // Subíndice
        if (c == wxT('_') && i + 1 < expr.length()) {
            i++;
            wxString sub;
            if (i < expr.length() && expr[i] == wxT('{')) {
                size_t end = expr.find(wxT('}'), i);
                if (end != wxString::npos) {
                    sub = expr.substr(i + 1, end - i - 1);
                    i = end + 1;
                }
            } else {
                sub = expr.substr(i, 1);
                i++;
            }
            
            dc.SetFont(wxFontInfo(fontSize - 4).Family(wxFONTFAMILY_MODERN));
            dc.DrawText(sub, currentX, currentY + 8);
            currentX += dc.GetTextExtent(sub).GetWidth();
            dc.SetFont(font);
            continue;
        }
        
        // Superíndice
        if (c == wxT('^') && i + 1 < expr.length()) {
            i++;
            wxString sup;
            if (i < expr.length() && expr[i] == wxT('{')) {
                size_t end = expr.find(wxT('}'), i);
                if (end != wxString::npos) {
                    sup = expr.substr(i + 1, end - i - 1);
                    i = end + 1;
                }
            } else {
                sup = expr.substr(i, 1);
                i++;
            }
            
            dc.SetFont(wxFontInfo(fontSize - 4).Family(wxFONTFAMILY_MODERN));
            dc.DrawText(sup, currentX, currentY - 8);
            currentX += dc.GetTextExtent(sup).GetWidth();
            dc.SetFont(font);
            continue;
        }
        
        // Texto normal
        wxString text;
        while (i < expr.length() && expr[i] != wxT('_') && expr[i] != wxT('^')) {
            text += expr[i];
            i++;
        }
        
        if (!text.IsEmpty()) {
            dc.DrawText(text, currentX, currentY);
            currentX += dc.GetTextExtent(text).GetWidth();
        }
    }
}

wxString EquationRenderer::latexToPlain(const wxString& latex) {
    // Simplificar LaTeX a texto legible
    wxString result = latex;
    
    // Reemplazar comandos comunes
    result.Replace(wxT("\\frac{"), wxT("("));
    result.Replace(wxT("}{"), wxT(")/("));
    result.Replace(wxT("}"), wxT(")"));
    result.Replace(wxT("\\sqrt{"), wxT("sqrt("));
    result.Replace(wxT("\\cdot"), wxT("·"));
    result.Replace(wxT("\\hat{i}"), wxT("î"));
    result.Replace(wxT("\\hat{j}"), wxT("ĵ"));
    result.Replace(wxT("\\Gamma"), wxT("Γ"));
    result.Replace(wxT("\\times"), wxT("×"));
    result.Replace(wxT("\\div"), wxT("÷"));
    result.Replace(wxT("\\alpha"), wxT("α"));
    result.Replace(wxT("\\beta"), wxT("β"));
    result.Replace(wxT("\\gamma"), wxT("γ"));
    result.Replace(wxT("\\delta"), wxT("δ"));
    result.Replace(wxT("\\Delta"), wxT("Δ"));
    result.Replace(wxT("\\pi"), wxT("π"));
    result.Replace(wxT("\\theta"), wxT("θ"));
    result.Replace(wxT("\\omega"), wxT("ω"));
    result.Replace(wxT("\\nabla"), wxT("∇"));
    result.Replace(wxT("\\int"), wxT("∫"));
    result.Replace(wxT("\\oint"), wxT("∮"));
    result.Replace(wxT("\\sum"), wxT("∑"));
    result.Replace(wxT("\\infty"), wxT("∞"));
    result.Replace(wxT("\\partial"), wxT("∂"));
    
    return result;
}

void EquationRenderer::drawElement(wxDC& dc, const EquationElement& elem, int& x, int y, int baseline) {
    switch (elem.type) {
        case EquationElement::SUBSCRIPT:
            dc.SetFont(subscriptFont_);
            dc.SetTextForeground(elem.color);
            dc.DrawText(elem.text, x, baseline + 5);
            x += dc.GetTextExtent(elem.text).GetWidth() + 1;
            dc.SetFont(baseFont_);
            break;
            
        case EquationElement::SUPERSCRIPT:
            dc.SetFont(superscriptFont_);
            dc.SetTextForeground(elem.color);
            dc.DrawText(elem.text, x, baseline - 12);
            x += dc.GetTextExtent(elem.text).GetWidth() + 1;
            dc.SetFont(baseFont_);
            break;
            
        case EquationElement::FRACTION: {
            // Dividir numerador y denominador
            size_t sep = elem.text.find(wxT('|'));
            if (sep != wxString::npos) {
                wxString num = elem.text.substr(0, sep);
                wxString den = elem.text.substr(sep + 1);
                
                dc.SetFont(baseFont_);
                dc.SetTextForeground(elem.color);
                
                wxSize numSize = dc.GetTextExtent(num);
                wxSize denSize = dc.GetTextExtent(den);
                int width = std::max(numSize.GetWidth(), denSize.GetWidth()) + 10;
                
                // Numerador
                dc.DrawText(num, x + (width - numSize.GetWidth()) / 2, baseline - 22);
                // Barra
                dc.SetPen(wxPen(elem.color, 2));
                dc.DrawLine(x, baseline - 5, x + width, baseline - 5);
                // Denominador
                dc.DrawText(den, x + (width - denSize.GetWidth()) / 2, baseline + 2);
                
                x += width + 5;
            }
            break;
        }
            
        case EquationElement::ROOT: {
            dc.SetFont(baseFont_);
            dc.SetTextForeground(elem.color);
            
            wxSize size = dc.GetTextExtent(elem.text);
            
            // Símbolo de raíz
            dc.SetPen(wxPen(elem.color, 2));
            int x1 = x + 5;
            int y1 = baseline - size.GetHeight() + 5;
            int x2 = x + 12;
            int y2 = baseline;
            int x3 = x + 15;
            int y3 = baseline - size.GetHeight() + 5;
            
            dc.DrawLine(x1, y1, x2, y2);
            dc.DrawLine(x2, y2, x3, y3);
            dc.DrawLine(x3, y3, x + 18 + size.GetWidth(), y3);
            dc.DrawLine(x + 18, y3, x + 20 + size.GetWidth(), y3);
            
            // Contenido
            dc.DrawText(elem.text, x + 18, baseline - size.GetHeight() + 5);
            
            x += 25 + size.GetWidth();
            break;
        }
            
        case EquationElement::GREEK:
        case EquationElement::SYMBOL:
            dc.SetFont(symbolFont_);
            dc.SetTextForeground(elem.color);
            dc.DrawText(elem.text, x, baseline - 12);
            x += dc.GetTextExtent(elem.text).GetWidth() + 2;
            dc.SetFont(baseFont_);
            break;
            
        default:
            dc.SetFont(baseFont_);
            dc.SetTextForeground(elem.color);
            dc.DrawText(elem.text, x, baseline - 12);
            x += dc.GetTextExtent(elem.text).GetWidth() + 3;
            break;
    }
}

wxString EquationRenderer::latexToUnicode(const wxString& latex) {
    static std::map<wxString, wxString> mapping = {
        {wxT("alpha"), wxT("α")},
        {wxT("beta"), wxT("β")},
        {wxT("gamma"), wxT("γ")},
        {wxT("delta"), wxT("δ")},
        {wxT("epsilon"), wxT("ε")},
        {wxT("theta"), wxT("θ")},
        {wxT("lambda"), wxT("λ")},
        {wxT("mu"), wxT("μ")},
        {wxT("pi"), wxT("π")},
        {wxT("rho"), wxT("ρ")},
        {wxT("sigma"), wxT("σ")},
        {wxT("tau"), wxT("τ")},
        {wxT("phi"), wxT("φ")},
        {wxT("omega"), wxT("ω")},
        {wxT("Delta"), wxT("Δ")},
        {wxT("Sigma"), wxT("Σ")},
        {wxT("Omega"), wxT("Ω")},
        {wxT("Gamma"), wxT("Γ")},
        {wxT("infty"), wxT("∞")},
        {wxT("nabla"), wxT("∇")},
        {wxT("partial"), wxT("∂")},
        {wxT("cdot"), wxT("·")},
        {wxT("times"), wxT("×")},
        {wxT("div"), wxT("÷")},
        {wxT("pm"), wxT("±")},
        {wxT("leq"), wxT("≤")},
        {wxT("geq"), wxT("≥")},
        {wxT("neq"), wxT("≠")},
        {wxT("approx"), wxT("≈")},
        {wxT("int"), wxT("∫")},
        {wxT("oint"), wxT("∮")},
        {wxT("sum"), wxT("∑")},
        {wxT("prod"), wxT("∏")},
        {wxT("sqrt"), wxT("√")},
        {wxT("to"), wxT("→")},
        {wxT("rightarrow"), wxT("→")},
        {wxT("leftarrow"), wxT("←")},
        {wxT("Rightarrow"), wxT("⇒")},
        {wxT("Leftrightarrow"), wxT("⇔")}
    };
    
    auto it = mapping.find(latex);
    if (it != mapping.end()) {
        return it->second;
    }
    return wxT("");
}

// Implementación de EquationWidget
EquationWidget::EquationWidget(wxWindow* parent, const wxString& equation)
    : wxPanel(parent, wxID_ANY)
    , equation_(equation)
    , color_(wxColour(0, 0, 128))
    , fontSize_(14)
    , needsRedraw_(true) {
    
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetBackgroundColour(*wxWHITE);
    SetMinSize(wxSize(200, 72));
}

void EquationWidget::setEquation(const wxString& equation) {
    equation_ = equation;
    int lineCount = 1;
    for (wxUniChar ch : equation_) {
        if (ch == '\n') {
            lineCount++;
        }
    }
    SetMinSize(wxSize(200, std::max(72, 26 * lineCount + 18)));
    needsRedraw_ = true;
    Refresh();
}

void EquationWidget::setColor(const wxColour& color) {
    color_ = color;
    needsRedraw_ = true;
    Refresh();
}

void EquationWidget::setFontSize(int size) {
    fontSize_ = size;
    needsRedraw_ = true;
    Refresh();
}

void EquationWidget::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    
    // Fondo
    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.Clear();
    
    wxArrayString lines = wxSplit(equation_, '\n');
    if (lines.empty()) {
        lines.Add(equation_);
    }

    int top = 10;
    for (size_t i = 0; i < lines.size(); ++i) {
        renderer_.renderString(dc, renderer_.latexToPlain(lines[i]), 10, top + static_cast<int>(i) * (fontSize_ + 8),
                               color_, fontSize_);
    }
}

void EquationWidget::OnSize(wxSizeEvent& event) {
    event.Skip();
    Refresh();
}

} // namespace tp::presentation
