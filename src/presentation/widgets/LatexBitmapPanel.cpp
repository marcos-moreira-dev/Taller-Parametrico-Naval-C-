#include "widgets/LatexBitmapPanel.hpp"

#include <wx/filename.h>
#include <wx/filesys.h>
#include <wx/image.h>
#include <wx/log.h>
#include <wx/sizer.h>

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

wxBitmap LoadFormulaBitmap(const wxString& path, int maxWidth) {
    if (path.IsEmpty() || !wxFileExists(path)) {
        return wxBitmap();
    }
    wxLogNull suppress;
    wxImage image(path, wxBITMAP_TYPE_ANY);
    if (!image.IsOk()) {
        return wxBitmap();
    }
    if (maxWidth > 0 && image.GetWidth() > maxWidth) {
        const double scale = static_cast<double>(maxWidth) / static_cast<double>(image.GetWidth());
        const int h = std::max(1, static_cast<int>(image.GetHeight() * scale));
        image = image.Scale(maxWidth, h, wxIMAGE_QUALITY_HIGH);
    }
    return wxBitmap(image);
}
}

LatexBitmapPanel::LatexBitmapPanel(wxWindow* parent, const wxString& latex)
    : wxPanel(parent, wxID_ANY)
    , latex_(latex)
    , imagePath_(wxEmptyString)
    , color_(wxColour(0, 0, 128))
    , imageScroll_(nullptr)
    , webView_(nullptr)
    , imageCtrl_(nullptr)
    , fallbackCtrl_(nullptr) {
    SetBackgroundColour(*wxWHITE);
    SetMinSize(wxSize(260, 200));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    imageScroll_ = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                        wxHSCROLL | wxVSCROLL | wxBORDER_NONE);
    imageScroll_->SetScrollRate(12, 12);
    imageScroll_->SetBackgroundColour(*wxWHITE);
    wxBoxSizer* imageSizer = new wxBoxSizer(wxVERTICAL);
    imageCtrl_ = new wxStaticBitmap(imageScroll_, wxID_ANY, wxBitmap());
    imageSizer->Add(imageCtrl_, 0, wxALIGN_LEFT | wxALL, 8);
    imageScroll_->SetSizer(imageSizer);
    imageScroll_->Hide();
    sizer->Add(imageScroll_, 1, wxEXPAND | wxALL, 4);

    webView_ = wxWebView::New(this, wxID_ANY);
    if (webView_) {
        webView_->SetMinSize(wxSize(-1, 170));
        sizer->Add(webView_, 1, wxEXPAND | wxALL, 4);
    }

    fallbackCtrl_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                   wxDefaultPosition, wxDefaultSize,
                                   wxTE_MULTILINE | wxTE_READONLY | wxBORDER_NONE);
    fallbackCtrl_->SetBackgroundColour(*wxWHITE);
    wxFont font(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    fallbackCtrl_->SetFont(font);
    fallbackCtrl_->SetForegroundColour(color_);
    if (webView_) {
        fallbackCtrl_->Hide();
    }
    sizer->Add(fallbackCtrl_, 1, wxEXPAND | wxALL, 6);

    SetSizer(sizer);
    refreshHtml();
}

void LatexBitmapPanel::setLatex(const wxString& latex) {
    latex_ = latex;
    SetMinSize(wxSize(220, estimatedHeight()));
    InvalidateBestSize();
    refreshHtml();
}

void LatexBitmapPanel::setImagePath(const wxString& path) {
    imagePath_ = path;
    refreshHtml();
}

void LatexBitmapPanel::setForegroundColor(const wxColour& color) {
    color_ = color;
    if (fallbackCtrl_) {
        fallbackCtrl_->SetForegroundColour(color_);
    }
    refreshHtml();
}

void LatexBitmapPanel::refreshHtml() {
    bool showingImage = false;
    if (imageCtrl_ && imageScroll_) {
        const wxBitmap bitmap = LoadFormulaBitmap(imagePath_, 1600);
        if (bitmap.IsOk()) {
            imageCtrl_->SetBitmap(bitmap);
            imageCtrl_->SetMinSize(bitmap.GetSize());
            imageScroll_->Show();
            imageScroll_->FitInside();
            showingImage = true;
        } else {
            imageScroll_->Hide();
        }
    }

    const wxString latexDoc = buildHtml();
    if (!showingImage && webView_ && imagePath_.IsEmpty()) {
        webView_->Show();
        webView_->SetPage(latexDoc, defaultWebBaseUrl());
        if (fallbackCtrl_) {
            fallbackCtrl_->Hide();
        }
    } else {
        if (webView_) {
            webView_->Hide();
        }
        if (fallbackCtrl_) {
            fallbackCtrl_->Show(!showingImage);
            fallbackCtrl_->SetValue(normalizedLatex());
        }
    }
    Layout();
}

wxString LatexBitmapPanel::buildHtml() const {
    const wxString latex = normalizedLatex();
    const wxString color = cssColor(color_);
    wxString html;
    html << "<!doctype html><html><head><meta charset='utf-8'>"
         << "<link rel='stylesheet' href='katex/katex.min.css' />"
         << "<script defer src='katex/katex.min.js'></script>"
         << "<script defer src='katex/auto-render.min.js'></script>"
         << "<style>html,body{margin:0;padding:0;background:#fff;overflow:auto;}body{padding:18px;font-family:Segoe UI,Arial,sans-serif;}"
         << ".formula{padding:16px 20px;border-radius:14px;background:#f7fbff;border:1px solid #dde8f6;color:" << color << ";min-height:96px;overflow:auto;}"
         << ".formula .katex-display{margin:0.35em 0;text-align:left !important;overflow-x:auto;overflow-y:hidden;}"
         << ".formula .katex-display>.katex{text-align:left !important;display:inline-block;}"
         << "</style></head><body><div class='formula'>$$" << latex << "$$</div>"
         << "<script>function renderAll(){if(window.renderMathInElement){renderMathInElement(document.body,{delimiters:[{left:'$$',right:'$$',display:true},{left:'\\(',right:'\\)',display:false},{left:'$',right:'$',display:false}],throwOnError:false,strict:'ignore'});}else{setTimeout(renderAll,120);}}window.addEventListener('DOMContentLoaded',renderAll);window.addEventListener('load',renderAll);</script></body></html>";
    return html;
}

wxString LatexBitmapPanel::normalizedLatex() const {
    wxArrayString lines = wxSplit(latex_, '\n');
    wxArrayString cleaned;
    for (size_t i = 0; i < lines.size(); ++i) {
        wxString line = lines[i];
        line.Trim(true);
        line.Trim(false);
        if (!line.IsEmpty()) {
            cleaned.Add(line);
        }
    }
    if (cleaned.IsEmpty()) {
        return latex_;
    }
    wxString result;
    for (size_t i = 0; i < cleaned.size(); ++i) {
        if (!result.IsEmpty()) result += "\n";
        result += cleaned[i];
    }
    return result;
}

int LatexBitmapPanel::estimatedHeight() const {
    wxArrayString lines = wxSplit(latex_, '\n');
    int count = 0;
    for (const wxString& rawLine : lines) {
        wxString line = rawLine;
        line.Trim(true);
        line.Trim(false);
        if (!line.IsEmpty()) {
            count++;
        }
    }
    int height = 108 + std::max(1, count) * 42;
    return std::max(170, height);
}

wxString LatexBitmapPanel::escapeHtml(const wxString& text) {
    wxString out = text;
    out.Replace("&", "&amp;");
    out.Replace("<", "&lt;");
    out.Replace(">", "&gt;");
    return out;
}

wxString LatexBitmapPanel::escapeForJsString(const wxString& text) {
    wxString out = text;
    out.Replace("\\", "\\\\");
    out.Replace("'", "\\'");
    out.Replace("\n", "\\n");
    return out;
}

wxString LatexBitmapPanel::cssColor(const wxColour& color) {
    return wxString::Format("rgb(%d,%d,%d)", color.Red(), color.Green(), color.Blue());
}

} // namespace tp::presentation
