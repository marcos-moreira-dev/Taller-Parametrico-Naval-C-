#include "widgets/LatexBitmapPanel.hpp"

#include <wx/filename.h>
#include <wx/filesys.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/textctrl.h>

#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>

namespace tp::presentation {

namespace {

wxString U(const char8_t* text) {
    return wxString::FromUTF8(reinterpret_cast<const char*>(text));
}

wxString AssetBaseUrl() {
#ifdef TP_SOURCE_DIR
    std::filesystem::path path = std::filesystem::path(TP_SOURCE_DIR) / "assets" / "web" / "theory_base.html";
    std::string url = "file:///" + path.generic_string();
    url = std::regex_replace(url, std::regex(" "), "%20");
    return wxString::FromUTF8(url.c_str());
#else
    return wxString();
#endif
}

std::string ReadUtf8Asset(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    std::string data = buffer.str();
    if (data.size() >= 3 && static_cast<unsigned char>(data[0]) == 0xEF &&
        static_cast<unsigned char>(data[1]) == 0xBB && static_cast<unsigned char>(data[2]) == 0xBF) {
        data.erase(0, 3);
    }
    return data;
}

std::string SanitizeInlineScript(const std::string& script) {
    return std::regex_replace(script, std::regex("</script>"), "<\\/script>");
}

} // namespace

LatexBitmapPanel::LatexBitmapPanel(wxWindow* parent, const wxString& latex)
    : wxPanel(parent, wxID_ANY)
    , latex_(latex)
    , color_(wxColour(0, 0, 128))
    , webView_(nullptr)
    , fallbackCtrl_(nullptr) {
    SetBackgroundColour(*wxWHITE);
    SetMinSize(wxSize(220, 210));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    webView_ = wxWebView::New(this, wxID_ANY, wxString(), wxDefaultPosition, wxDefaultSize, wxWebViewBackendEdge);
    if (webView_) {
        webView_->Bind(wxEVT_WEBVIEW_LOADED, &LatexBitmapPanel::OnWebViewLoaded, this);
        sizer->Add(webView_, 1, wxEXPAND | wxALL, 0);
        refreshHtml();
    } else {
        fallbackCtrl_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                       wxDefaultPosition, wxDefaultSize,
                                       wxTE_MULTILINE | wxTE_READONLY | wxBORDER_NONE);
        fallbackCtrl_->SetBackgroundColour(*wxWHITE);
        sizer->Add(fallbackCtrl_, 1, wxEXPAND | wxALL, 6);
        fallbackCtrl_->SetValue(U(u8"wxWebView no está disponible.\n\n") + latex_);
    }

    SetSizer(sizer);
}

void LatexBitmapPanel::setLatex(const wxString& latex) {
    latex_ = latex;
    SetMinSize(wxSize(220, estimatedHeight()));
    InvalidateBestSize();
    if (GetParent()) {
        GetParent()->Layout();
        if (auto* scrolled = wxDynamicCast(GetGrandParent(), wxScrolledWindow)) {
            scrolled->FitInside();
            scrolled->Layout();
        }
    }
    refreshHtml();
}

void LatexBitmapPanel::setForegroundColor(const wxColour& color) {
    color_ = color;
    refreshHtml();
}

void LatexBitmapPanel::refreshHtml() {
    if (webView_) {
        webView_->SetPage(buildHtml(), AssetBaseUrl());
    } else if (fallbackCtrl_) {
        fallbackCtrl_->SetValue(latex_);
    }
}

wxString LatexBitmapPanel::buildHtml() const {
    const wxString latex = normalizedLatex();
    const wxString color = cssColor(color_);
    const int minHeight = estimatedHeight() - 10;

    wxString html =
        "<!DOCTYPE html><html><head><meta charset='utf-8'><meta http-equiv='X-UA-Compatible' content='IE=edge'>"
        "<base href='{{BASE}}'><link rel='stylesheet' href='katex/katex.min.css'>"
        "<style>"
        "html,body{margin:0;padding:0;background:#ffffff;color:{{COLOR}};overflow:hidden;font-family:'Cambria Math','Times New Roman',serif;}"
        "body{padding:18px 22px 22px 22px;box-sizing:border-box;}"
        "#formula-root{width:100%;display:block;padding:0;box-sizing:border-box;}"
        "#formula{font-size:1.04rem;line-height:1.35;min-height:{{HEIGHT}}px;display:block;text-align:left;overflow-x:hidden;overflow-y:auto;padding:6px 8px 8px 8px;width:100%;box-sizing:border-box;}"
        "#fallback{display:none;color:#8a2d2d;font:13px 'Segoe UI',Arial,sans-serif;margin-top:8px;}"
        "#raw{display:none;white-space:pre-wrap;font:14px 'Consolas','Courier New',monospace;}"
        ".katex-display{margin:0.35em 0 0.65em 0;overflow:visible;padding:2px 0 8px 0;text-align:left !important;}"
        ".katex-display>.katex{text-align:left !important;display:inline-block;}"
        "</style></head><body>"
        "<div id='formula-root'><div id='formula'></div></div>"
        "<div id='fallback'>No se pudo cargar KaTeX. Se muestra la fórmula en texto plano.</div>"
        "<pre id='raw'>{{RAW}}</pre>"
        "</body></html>";

    html.Replace("{{BASE}}", AssetBaseUrl());
    html.Replace("{{COLOR}}", color);
    html.Replace("{{HEIGHT}}", wxString::Format("%d", minHeight));
    html.Replace("{{RAW}}", escapeHtml(latex));
    return html;
}

void LatexBitmapPanel::OnWebViewLoaded(wxWebViewEvent& event) {
    (void)event;
    if (!webView_) {
        return;
    }

    std::string katexJs;
#ifdef TP_SOURCE_DIR
    katexJs = ReadUtf8Asset(std::filesystem::path(TP_SOURCE_DIR) / "assets" / "web" / "katex" / "katex.min.js");
#endif

    if (katexJs.empty()) {
        webView_->RunScript("document.getElementById('fallback').style.display='block';document.getElementById('raw').style.display='block';");
        return;
    }

    wxString renderScript =
        wxString::FromUTF8(katexJs.c_str()) +
        "\ntry {"
        "  var src = \"" + LatexBitmapPanel::escapeForJsString(normalizedLatex()) + "\";"
        "  var host = document.getElementById('formula');"
        "  host.innerHTML = ''; host.scrollTop = 0; host.scrollLeft = 0;"
        "  if(!src || !src.trim()){ throw new Error('empty formula'); }"
        "  var node = document.createElement('div');"
        "  host.appendChild(node);"
        "  katex.render(src, node, {displayMode:true, throwOnError:false, fleqn:true, strict:'ignore'});"
        "} catch (e) {"
        "  document.getElementById('formula').style.display='none';"
        "  document.getElementById('fallback').style.display='block';"
        "  document.getElementById('raw').style.display='block';"
        "}";

    webView_->RunScript(renderScript);
}

wxString LatexBitmapPanel::normalizedLatex() const {
    wxArrayString lines = wxSplit(latex_, '\n');
    wxString result;

    for (size_t i = 0; i < lines.size(); ++i) {
        wxString line = lines[i];
        line.Trim(true);
        line.Trim(false);
        if (line.IsEmpty()) {
            continue;
        }
        if (!result.IsEmpty()) {
            result += "\n";
        }
        result += line;
    }

    if (result.Contains("\\begin{")) {
        return result;
    }

    if (result.Contains("\n")) {
        wxArrayString parts = wxSplit(result, '\n');
        wxString joined;
        for (size_t i = 0; i < parts.size(); ++i) {
            wxString line = parts[i];
            line.Trim(true);
            line.Trim(false);
            if (line.IsEmpty()) {
                continue;
            }
            if (!joined.IsEmpty()) {
                joined += " \\\\ ";
            }
            joined += line;
        }
        if (!joined.IsEmpty()) {
            result = "\\begin{aligned}" + joined + "\\end{aligned}";
        }
    }

    if (result.IsEmpty()) {
        result = latex_;
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

    int height = 110 + std::max(1, count) * 48;
    if (latex_.Contains("\\frac") || latex_.Contains("\\dfrac") || latex_.Contains("\\int") || latex_.Contains("\\oint") || latex_.Contains("\\sum") || latex_.Contains("\\nabla")) {
        height += 42;
    }
    if (latex_.Contains("\\begin{aligned}")) {
        height += 26;
    }
    if (latex_.Contains("\\qquad") || latex_.Contains("\\quad")) {
        height += 10;
    }
    return std::max(190, height);
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
    out.Replace("\"", "\\\"");
    out.Replace("\r", "");
    out.Replace("\n", "\\n");
    return out;
}

wxString LatexBitmapPanel::cssColor(const wxColour& color) {
    return wxString::Format("rgb(%d,%d,%d)", color.Red(), color.Green(), color.Blue());
}

} // namespace tp::presentation
