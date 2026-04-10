#include "widgets/MathHtmlPanel.hpp"

#include <wx/filesys.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/textfile.h>

namespace tp::presentation {

namespace {
wxString defaultWebBaseUrl() {
#ifdef TP_SOURCE_DIR
    wxFileName fileName(wxString::FromUTF8(TP_SOURCE_DIR));
    fileName.AppendDir("assets");
    fileName.AppendDir("web");
    return wxFileSystem::FileNameToURL(fileName);
#else
    wxFileName fileName(wxFileName::DirName(wxGetCwd()));
    return wxFileSystem::FileNameToURL(fileName);
#endif
}

wxString assetsWebDir() {
#ifdef TP_SOURCE_DIR
    wxFileName fileName(wxString::FromUTF8(TP_SOURCE_DIR));
    fileName.AppendDir("assets");
    fileName.AppendDir("web");
    return fileName.GetFullPath();
#else
    wxFileName fileName(wxGetCwd(), wxEmptyString);
    return fileName.GetFullPath();
#endif
}

wxString loadTextFile(const wxString& fullPath) {
    wxTextFile file;
    if (!file.Open(fullPath)) {
        return wxEmptyString;
    }
    wxString content;
    for (size_t i = 0; i < file.GetLineCount(); ++i) {
        content += file.GetLine(i);
        if (i + 1 < file.GetLineCount()) {
            content += "\n";
        }
    }
    return content;
}

wxString inlineScriptSafe(const wxString& text) {
    wxString out = text;
    out.Replace("</script", "</scr" "ipt", true);
    return out;
}
}

MathHtmlPanel::MathHtmlPanel(wxWindow* parent, const wxString& htmlBody)
    : wxPanel(parent, wxID_ANY)
    , htmlBody_(htmlBody)
    , baseUrl_(defaultWebBaseUrl())
    , htmlIsDocument_(false)
    , htmlWindow_(nullptr)
    , webView_(nullptr) {
    SetBackgroundColour(*wxWHITE);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    webView_ = wxWebView::New(this, wxID_ANY);
    if (webView_) {
        sizer->Add(webView_, 1, wxEXPAND);
    }

    htmlWindow_ = new wxHtmlWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                   wxHW_SCROLLBAR_AUTO | wxBORDER_NONE);
    htmlWindow_->SetBorders(0);
    htmlWindow_->SetBackgroundColour(*wxWHITE);
    if (webView_) {
        htmlWindow_->Hide();
    }
    sizer->Add(htmlWindow_, 1, wxEXPAND);

    SetSizer(sizer);
    refreshPage();
}

void MathHtmlPanel::setHtmlBody(const wxString& htmlBody) {
    htmlBody_ = htmlBody;
    htmlIsDocument_ = false;
    if (baseUrl_.IsEmpty()) {
        baseUrl_ = defaultWebBaseUrl();
    }
    refreshPage();
}

void MathHtmlPanel::setHtmlBody(const wxString& htmlBody, const wxString& baseUrl) {
    htmlBody_ = htmlBody;
    htmlIsDocument_ = false;
    baseUrl_ = baseUrl.IsEmpty() ? defaultWebBaseUrl() : baseUrl;
    refreshPage();
}

void MathHtmlPanel::setHtmlDocument(const wxString& htmlDocument, const wxString& baseUrl) {
    htmlBody_ = htmlDocument;
    baseUrl_ = baseUrl.IsEmpty() ? defaultWebBaseUrl() : baseUrl;
    htmlIsDocument_ = true;
    refreshPage();
}

wxString MathHtmlPanel::buildHtmlDocument() const {
    wxString html;
    html << "<!doctype html><html lang='es'><head><meta charset='utf-8'>"
         << "<meta http-equiv='X-UA-Compatible' content='IE=edge'>"
         << "<base href='" << (baseUrl_.IsEmpty() ? defaultWebBaseUrl() : baseUrl_) << "' />"
         << "<style>"
         << "\nhtml,body{margin:0;padding:0;background:#fff;color:#1f2a37;font-family:Segoe UI,Arial,sans-serif;overflow:auto;}"
         << "body{padding:14px 18px 20px 18px;line-height:1.6;font-size:15px;}"
         << "p{margin:0 0 10px 0;}h2{margin:0 0 12px 0;color:#16324f;font-size:20px;}h3{margin:18px 0 10px 0;color:#16324f;font-size:16px;}h4{margin:14px 0 8px 0;color:#16324f;font-size:15px;}"
         << "ul,ol{margin:0 0 12px 22px;}li{margin-bottom:6px;}code{font-family:Consolas,Cascadia Mono,monospace;background:#f3f7fb;padding:2px 5px;border-radius:4px;}"
         << "pre{font-family:Cambria Math,Consolas,monospace;background:#f7fbff;border:1px solid #dde8f6;padding:10px 12px;border-radius:10px;white-space:pre-wrap;}"
         << "blockquote,.note-box{margin:12px 0;padding:12px 14px;background:#fff7ea;border-left:4px solid #e4b85f;border-radius:8px;color:#51432a;}"
         << ".description-lead{font-size:18px;line-height:1.72;color:#17365d;}"
         << ".math-block{margin:14px 0;padding:12px 16px;background:#f7fbff;border:1px solid #dde8f6;border-radius:10px;overflow-x:auto;}"
         << ".math-block pre{margin:0;background:transparent;border:none;padding:0;white-space:pre-wrap;font-family:Cambria Math,Times New Roman,serif;color:#17365d;font-size:18px;}"
         << "figure.theory-figure{margin:16px auto;text-align:center;} figure.theory-figure img{max-width:100%;height:auto;border-radius:10px;} figure.theory-figure figcaption{margin-top:8px;color:#5b6773;font-size:13px;}"
         << "img.inline-math-img{display:inline-block;vertical-align:-0.14em;height:1.38em;width:auto;margin:0 0.12em;} .math-image-block{padding:6px 10px;overflow-x:auto;} .block-math-img{display:block;max-width:min(88%,680px);max-height:112px;width:auto;height:auto;margin:0 auto;}"
         << "</style>"
         << "</head><body>"
         << htmlBody_
         << "</body></html>";
    return html;
}

void MathHtmlPanel::refreshPage() {
    const wxString html = htmlIsDocument_ ? htmlBody_ : buildHtmlDocument();
    const wxString base = baseUrl_.IsEmpty() ? defaultWebBaseUrl() : baseUrl_;

    if (webView_) {
        webView_->Show();
        htmlWindow_->Hide();
        webView_->SetPage(html, base);
    } else if (htmlWindow_) {
        htmlWindow_->Show();
        htmlWindow_->SetPage(html);
    }
    Layout();
}

} // namespace tp::presentation
