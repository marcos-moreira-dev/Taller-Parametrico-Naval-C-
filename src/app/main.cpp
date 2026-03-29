#include <wx/wx.h>
#include "../presentation/main_window/MainWindow.hpp"
#include "../shared/Logger.hpp"
#include <filesystem>

// Para soporte DPI-aware en Windows
#ifdef __WXMSW__
#include <windows.h>
#endif

using namespace tp::presentation;
using namespace tp::shared;

class TallerParametricoApp : public wxApp {
public:
    virtual bool OnInit() override;
    virtual int OnExit() override;
};

// Habilitar DPI awareness antes de que wxWidgets inicie
#ifdef __WXMSW__
BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        // DPI awareness moderno (Per-Monitor V2)
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }
    return TRUE;
}
#endif

wxIMPLEMENT_APP(TallerParametricoApp);

bool TallerParametricoApp::OnInit() {
    // Configurar DPI awareness si no se hizo en DllMain
    #ifdef __WXMSW__
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    #endif
    
    // Configurar locale para UTF-8 (soporte de acentos)
    wxLocale* locale = new wxLocale();
    locale->Init(wxLANGUAGE_SPANISH);
    
    // Inicializar sistema de logging
    std::filesystem::create_directories("logs");
    Logger::instance().setOutputFile("logs/application.log");
    Logger::instance().setLogLevel(LogLevel::LOG_DEBUG);
    Logger::instance().enableConsoleOutput(true);
    
    
    
    
    
    try {
        
        MainWindow* window = new MainWindow(wxT("Taller Parametrico - Navegacion y Campos 2D"));
        
        
        window->Show(true);
        
        
        return true;
    } catch (const std::exception& e) {
        
        return false;
    }
}

int TallerParametricoApp::OnExit() {
    
    
    return wxApp::OnExit();
}
