#pragma once

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include <wx/wx.h>
#include <wx/webview.h>

namespace tp::presentation {

class MathHtmlPanel : public wxPanel {
public:
    MathHtmlPanel(wxWindow* parent, const wxString& htmlBody = wxEmptyString);

    void setHtmlBody(const wxString& htmlBody);
    void setHtmlDocument(const wxString& htmlDocument, const wxString& baseUrl = wxString());

private:
    wxString buildHtmlDocument() const;
    void refreshPage();

    wxString htmlBody_;
    wxString baseUrl_;
    bool htmlIsDocument_;
    wxWebView* webView_;
    wxTextCtrl* fallbackCtrl_;
};

} // namespace tp::presentation
