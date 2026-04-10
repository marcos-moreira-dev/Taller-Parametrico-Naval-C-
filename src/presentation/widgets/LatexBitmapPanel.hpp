#pragma once

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include <wx/wx.h>
#include <wx/webview.h>
#include <wx/scrolwin.h>

namespace tp::presentation {

class LatexBitmapPanel : public wxPanel {
public:
    LatexBitmapPanel(wxWindow* parent, const wxString& latex = wxT(""));

    void setLatex(const wxString& latex);
    void setImagePath(const wxString& path);
    void setForegroundColor(const wxColour& color);

private:
    void refreshHtml();
    wxString buildHtml() const;
    wxString normalizedLatex() const;
    int estimatedHeight() const;
    static wxString escapeHtml(const wxString& text);
    static wxString escapeForJsString(const wxString& text);
    static wxString cssColor(const wxColour& color);

    wxString latex_;
    wxString imagePath_;
    wxColour color_;
    wxScrolledWindow* imageScroll_;
    wxWebView* webView_;
    wxStaticBitmap* imageCtrl_;
    wxTextCtrl* fallbackCtrl_;
};

} // namespace tp::presentation
