#include "widgets/MathHtmlPanel.hpp"

#include <wx/filesys.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>

namespace tp::presentation {

namespace {

wxString LocalTheoryBaseUrl() {
#ifdef TP_SOURCE_DIR
    wxFileName path(wxString::FromUTF8(TP_SOURCE_DIR));
    path.AppendDir(wxT("assets"));
    path.AppendDir(wxT("web"));
    path.SetFullName(wxT("theory_base.html"));
    return wxFileSystem::FileNameToURL(path);
#else
    return wxString();
#endif
}

}

MathHtmlPanel::MathHtmlPanel(wxWindow* parent, const wxString& htmlBody)
    : wxPanel(parent, wxID_ANY)
    , htmlBody_(htmlBody)
    , baseUrl_(wxString())
    , htmlIsDocument_(false)
    , webView_(nullptr)
    , fallbackCtrl_(nullptr) {
    SetBackgroundColour(*wxWHITE);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    webView_ = wxWebView::New(this, wxID_ANY, wxString(), wxDefaultPosition, wxDefaultSize, wxWebViewBackendEdge);

    if (webView_) {
        sizer->Add(webView_, 1, wxEXPAND);
        refreshPage();
    } else {
        fallbackCtrl_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                       wxDefaultPosition, wxDefaultSize,
                                       wxTE_MULTILINE | wxTE_READONLY | wxBORDER_NONE);
        fallbackCtrl_->SetBackgroundColour(*wxWHITE);
        sizer->Add(fallbackCtrl_, 1, wxEXPAND | wxALL, 6);
        fallbackCtrl_->SetValue(htmlBody_);
    }

    SetSizer(sizer);
}

void MathHtmlPanel::setHtmlBody(const wxString& htmlBody) {
    htmlBody_ = htmlBody;
    baseUrl_.clear();
    htmlIsDocument_ = false;
    refreshPage();
}

void MathHtmlPanel::setHtmlDocument(const wxString& htmlDocument, const wxString& baseUrl) {
    htmlBody_ = htmlDocument;
    baseUrl_ = baseUrl;
    htmlIsDocument_ = true;
    refreshPage();
}

wxString MathHtmlPanel::buildHtmlDocument() const {
    return wxString::Format(
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<meta charset='utf-8'>"
        "<meta http-equiv='X-UA-Compatible' content='IE=edge'>"
        "<link rel='stylesheet' href='katex/katex.min.css'>"
        "<script defer src='katex/katex.min.js'></script>"
        "<script defer src='katex/auto-render.min.js'></script>"
        "<style>"
        "html,body{margin:0;padding:0;background:#ffffff;color:#1f2a37;font-family:'Segoe UI',Arial,sans-serif;}"
        "body{padding:10px 14px 18px 14px;line-height:1.55;font-size:15px;}"
        "p{margin:0 0 12px 0;}"
        "ul,ol{margin:0 0 14px 18px;padding-left:18px;}"
        "li{margin:0 0 8px 0;}"
        "code{font-family:'Cascadia Mono','Consolas',monospace;background:#f3f7fb;padding:2px 5px;border-radius:4px;}"
        "pre{font-family:'Cascadia Mono','Consolas',monospace;background:#f7fbff;padding:10px;border-radius:8px;overflow:auto;}"
        "h4{margin:0 0 10px 0;font-size:17px;color:#16324f;}"
        ".theory-box{background:#ffffff;border:1px solid #d6dde8;border-radius:10px;padding:12px 14px;margin:10px 0;}"
        ".example-box{background:#eefaf0;border:1px solid #cae7d0;border-radius:10px;padding:12px 14px;margin:12px 0;}"
        ".note-box{background:#fff7ea;border:1px solid #f0d7a6;border-radius:10px;padding:12px 14px;margin:12px 0;}"
        ".math-inline{font-size:1.02em;}"
        ".katex-display{margin:0.65em 0;overflow-x:auto;overflow-y:hidden;padding-bottom:2px;text-align:left !important;}"
        ".katex-display > .katex{text-align:left !important;display:inline-block;}"
        "</style>"
        "</head>"
        "<body>%s"
        "<script>"
        "window.addEventListener('load', function(){"
        "  if(window.renderMathInElement){"
        "    renderMathInElement(document.body,{"
        "      delimiters:["
        "        {left:'$$', right:'$$', display:true},"
        "        {left:'\\\\[', right:'\\\\]', display:true},"
        "        {left:'\\\\(', right:'\\\\)', display:false},"
        "        {left:'$', right:'$', display:false}"
        "      ],"
        "      throwOnError:false"
        "    });"
        "  }"
        "});"
        "</script>"
        "</body>"
        "</html>",
        htmlBody_);
}

void MathHtmlPanel::refreshPage() {
    if (webView_) {
        if (htmlIsDocument_) {
            webView_->SetPage(htmlBody_, baseUrl_);
        } else {
            webView_->SetPage(buildHtmlDocument(), LocalTheoryBaseUrl());
        }
    } else if (fallbackCtrl_) {
        fallbackCtrl_->SetValue(htmlBody_);
    }
}

} // namespace tp::presentation
