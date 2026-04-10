#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/icon.h>
#include <wx/image.h>
#include "../presentation/main_window/MainWindow.hpp"
#include "../shared/Logger.hpp"
#include "../shared/TraceLogger.hpp"
#include <filesystem>
#include <memory>

#ifdef __WXMSW__
#include <windows.h>
#endif

using namespace tp::presentation;
using namespace tp::shared;

class TallerParametricoApp : public wxApp {
public:
    bool OnInit() override;
    int OnExit() override;

private:
    std::unique_ptr<wxLocale> locale_;
};

#ifdef __WXMSW__
BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }
    return TRUE;
}
#endif

wxIMPLEMENT_APP(TallerParametricoApp);

namespace {

wxIcon LoadApplicationIcon() {
    wxIcon icon;
#ifdef __WXMSW__
    icon = wxIcon(wxT("IDI_ICON1"), wxBITMAP_TYPE_ICO_RESOURCE);
    if (icon.IsOk()) {
        return icon;
    }
#endif
#ifdef TP_SOURCE_DIR
    wxFileName fileName(wxString::FromUTF8(TP_SOURCE_DIR));
    fileName.AppendDir(wxT("src"));
    fileName.AppendDir(wxT("app"));
    fileName.AppendDir(wxT("resources"));
    fileName.SetFullName(wxT("icon.ico"));
    if (fileName.FileExists()) {
        icon.LoadFile(fileName.GetFullPath(), wxBITMAP_TYPE_ICO);
    }
#endif
    return icon;
}

void ConfigureLogging() {
    std::filesystem::create_directories("logs");
    Logger::instance().setOutputFile("logs/application.log");
    Logger::instance().setLogLevel(LogLevel::LOG_DEBUG);
    Logger::instance().enableConsoleOutput(true);
    TraceLogger::instance().setLogFile("logs/trace.log");
    TraceLogger::instance().setMinLevel(TraceLevel::DEBUG);
    TraceLogger::instance().enableConsoleOutput(true);
    TraceLogger::instance().enableFileOutput(true);
    Logger::instance().info("Arrancando Taller Paramétrico en modo Windows-first.");
    TraceLogger::instance().info(TraceEventType::STATE_CHANGE, "App", "ConfigureLogging", "Trazabilidad inicializada");
}

} // namespace

bool TallerParametricoApp::OnInit() {
#ifdef __WXMSW__
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#endif

    locale_ = std::make_unique<wxLocale>();
    locale_->Init(wxLANGUAGE_SPANISH);

    ConfigureLogging();
    wxInitAllImageHandlers();

    try {
        MainWindow* window = new MainWindow(wxT("Taller Paramétrico - Navegación y Campos 2D"));
        const wxIcon icon = LoadApplicationIcon();
        if (icon.IsOk()) {
            window->SetIcon(icon);
        }
        window->Show(true);
        Logger::instance().info("Ventana principal creada correctamente.");
        return true;
    } catch (const std::exception& e) {
        Logger::instance().error(std::string("Fallo durante el arranque: ") + e.what());
        wxMessageBox(wxString::FromUTF8(e.what()), wxT("Error al iniciar Taller Paramétrico"), wxOK | wxICON_ERROR);
        return false;
    } catch (...) {
        Logger::instance().error("Fallo durante el arranque: excepción desconocida.");
        wxMessageBox(wxT("Se produjo un error no identificado durante el arranque."), wxT("Error al iniciar Taller Paramétrico"), wxOK | wxICON_ERROR);
        return false;
    }
}

int TallerParametricoApp::OnExit() {
    Logger::instance().info("Cerrando Taller Paramétrico.");
    return wxApp::OnExit();
}
