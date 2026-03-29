#include "main_window/MainWindow.hpp"
#include "widgets/SimulationCanvas.hpp"
#include "widgets/Water3DCanvas.hpp"
#include "widgets/SimpleGraph.hpp"
#include "panels/TheoryPanel.hpp"
#include "panels/EditorPanel.hpp"
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/aboutdlg.h>
#include <fstream>
#include <iomanip>
#include "ExpressionParser.hpp"
#include "Scenario.hpp"
#include "VectorField.hpp"
#include "MathExpression.hpp"
#include "Boat.hpp"
#include "Motor.hpp"
#include "integrators/Integrators.hpp"
#include "models/PhysicsModel.hpp"
#include "runner/SimulationRunner.hpp"

// Colores modernos y profesionales
namespace ModernColors {
    // Colores principales
    const wxColour PRIMARY_BG(245, 247, 250);           // Fondo principal azul-grisáceo
    const wxColour PANEL_BG(255, 255, 255);             // Paneles blancos
    const wxColour CANVAS_BG(250, 252, 255);            // Canvas azul muy claro
    
    // Colores de acento
    const wxColour ACCENT_BLUE(66, 133, 244);           // Azul principal
    const wxColour ACCENT_GREEN(52, 168, 83);           // Verde éxito
    const wxColour ACCENT_ORANGE(251, 188, 5);          // Naranja advertencia
    const wxColour ACCENT_RED(234, 67, 53);             // Rojo error
    
    // Colores de texto
    const wxColour TEXT_PRIMARY(33, 37, 41);            // Texto principal casi negro
    const wxColour TEXT_SECONDARY(108, 117, 125);       // Texto secundario gris
    const wxColour TEXT_LIGHT(255, 255, 255);           // Texto blanco
    
    // Colores de borde
    const wxColour BORDER_LIGHT(222, 226, 230);         // Bordes suaves
    const wxColour BORDER_MEDIUM(206, 212, 218);        // Bordes medios
    
    // Colores de estado
    const wxColour STATE_RUNNING(52, 168, 83);          // Verde - simulación corriendo
    const wxColour STATE_PAUSED(251, 188, 5);           // Naranja - pausado
    const wxColour STATE_STOPPED(108, 117, 125);        // Gris - detenido
}

namespace tp::presentation {

// IDs de eventos
enum {
    ID_MENU_NEW = 100,
    ID_MENU_OPEN,
    ID_MENU_SAVE,
    ID_MENU_EXPORT_IMAGE,
    ID_MENU_EXPORT_DATA,
    ID_MENU_EXIT,
    ID_MENU_VIEW_LEFT,
    ID_MENU_VIEW_RIGHT,
    ID_MENU_VIEW_BOTTOM,
    ID_MENU_VIEW_GRID,
    ID_MENU_VIEW_FIELD,
    ID_MENU_VIEW_TRAJECTORY,
    ID_MENU_MODE_THEORY,
    ID_MENU_MODE_SIMULATION,
    ID_MENU_MODE_EDITOR,
    ID_MENU_START,
    ID_MENU_PAUSE,
    ID_MENU_RESET,
    ID_MENU_STEP,
    ID_MENU_ABOUT,
    ID_MENU_HELP,
    ID_MENU_RESET_VIEW,
    ID_MENU_CLEAR_TRAJECTORY,
    ID_MENU_CAPTURE_SCREEN,
    ID_MENU_CUSTOM_FIELD,
    ID_TIMER_SIMULATION = 200,
    ID_MODE_SELECTOR = 300,
    ID_SCENARIO_LIST,
    ID_LAYER_LIST,
    ID_THRUST_SLIDER,
    ID_DIRECTION_SLIDER,
    ID_METHOD_CHOICE,
    ID_DT_SLIDER,
    ID_FIELD_TYPE,
    ID_FIELD_INTENSITY,
    ID_TOGGLE_2D_3D
};

// Tabla de eventos de MainWindow
wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_MENU(ID_MENU_NEW, MainWindow::onNew)
    EVT_MENU(ID_MENU_OPEN, MainWindow::onOpen)
    EVT_MENU(ID_MENU_SAVE, MainWindow::onSave)
    EVT_MENU(ID_MENU_EXPORT_IMAGE, MainWindow::onExportImage)
    EVT_MENU(ID_MENU_EXPORT_DATA, MainWindow::onExportData)
    EVT_MENU(ID_MENU_EXIT, MainWindow::onExit)
    EVT_MENU(ID_MENU_VIEW_LEFT, MainWindow::onViewLeftPanel)
    EVT_MENU(ID_MENU_VIEW_RIGHT, MainWindow::onViewRightPanel)
    EVT_MENU(ID_MENU_VIEW_BOTTOM, MainWindow::onViewBottomPanel)
    EVT_MENU(ID_MENU_VIEW_GRID, MainWindow::onViewGrid)
    EVT_MENU(ID_MENU_VIEW_FIELD, MainWindow::onViewField)
    EVT_MENU(ID_MENU_VIEW_TRAJECTORY, MainWindow::onViewTrajectory)
    EVT_MENU(ID_MENU_MODE_THEORY, MainWindow::onModeTheory)
    EVT_MENU(ID_MENU_MODE_SIMULATION, MainWindow::onModeSimulation)
    EVT_MENU(ID_MENU_MODE_EDITOR, MainWindow::onModeEditor)
    EVT_MENU(ID_MENU_START, MainWindow::onStartSimulation)
    EVT_MENU(ID_MENU_PAUSE, MainWindow::onPauseSimulation)
    EVT_MENU(ID_MENU_RESET, MainWindow::onResetSimulation)
    EVT_MENU(ID_MENU_STEP, MainWindow::onStepSimulation)
    EVT_MENU(ID_MENU_ABOUT, MainWindow::onAbout)
    EVT_MENU(ID_MENU_HELP, MainWindow::onHelp)
    EVT_MENU(ID_MENU_RESET_VIEW, MainWindow::onResetView)
    EVT_MENU(ID_MENU_CLEAR_TRAJECTORY, MainWindow::onClearTrajectory)
    EVT_MENU(ID_MENU_CAPTURE_SCREEN, MainWindow::onCaptureScreen)
    EVT_MENU(ID_MENU_CUSTOM_FIELD, MainWindow::onCustomField)
    EVT_TIMER(ID_TIMER_SIMULATION, MainWindow::onTimer)
    EVT_MENU(ID_TOGGLE_3D, MainWindow::onToggle3D)
    EVT_CLOSE(MainWindow::OnClose)
wxEND_EVENT_TABLE()

//=============================================================================
// MAIN WINDOW
//=============================================================================

MainWindow::MainWindow(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1600, 1000),
              wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
    , leftPanel_(nullptr)
    , rightPanel_(nullptr)
    , bottomPanel_(nullptr)
    , canvas_(nullptr)
    , canvas3D_(nullptr)
    , is3DMode_(false)
    , currentMode_(AppMode::Simulation)
    , isRunning_(false)
    , isPaused_(false)
    , timer_(nullptr)
    , currentTime_(0.0)
    , currentSpeed_(0.0)
    , currentEnergy_(0.0)
    , totalDistance_(0.0)
    , isModified_(false)
    , currentScenarioName_(wxT("Nuevo Escenario"))
{
    // IMPORTANTE: Primero configurar el frame básico
    SetBackgroundColour(ModernColors::PRIMARY_BG);
    
    // Configurar menú PRIMERO (antes de AUI)
    setupMenu();
    
    // Configurar toolbar
    setupToolbar();
    
    // Ahora inicializar AUI manager
    auiManager_.SetManagedWindow(this);
    
    // Configurar layout AUI
    setupAuiLayout();
    
    // Configurar barra de estado
    setupStatusBar();
    
    // Crear timer
    timer_ = new wxTimer(this, ID_TIMER_SIMULATION);
    
    // Crear experimento inicial
    experimentService_.createNewExperiment();
    
    // Configurar canvas
    if (canvas_) {
        auto& config = experimentService_.getConfig();
        canvas_->setScenario(&config.scenario);
        canvas_->setField(&config.field);
        canvas_->setBoat(&config.boat);
    }
    
    // Actualizar título de ventana
    updateWindowTitle();
    
    // Centrar ventana
    Centre();
}

MainWindow::~MainWindow()
{
    if (timer_) {
        timer_->Stop();
        delete timer_;
    }
    auiManager_.UnInit();
}

void MainWindow::setupMenu()
{
    wxMenuBar* menuBar = new wxMenuBar();
    
    // Menú Archivo
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(ID_MENU_NEW, wxT("&Nuevo Escenario\tCtrl+N"), wxT("Crear un nuevo escenario"));
    fileMenu->Append(ID_MENU_OPEN, wxT("&Abrir...\tCtrl+O"), wxT("Abrir un escenario existente"));
    fileMenu->Append(ID_MENU_SAVE, wxT("&Guardar\tCtrl+S"), wxT("Guardar el escenario actual"));
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_MENU_EXPORT_IMAGE, wxT("Exportar &Imagen..."), wxT("Exportar vista actual como imagen"));
    fileMenu->Append(ID_MENU_EXPORT_DATA, wxT("Exportar &Datos..."), wxT("Exportar datos de simulación a CSV"));
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_MENU_EXIT, wxT("&Salir\tAlt+F4"), wxT("Salir de la aplicación"));
    menuBar->Append(fileMenu, wxT("&Archivo"));
    
    // Menú Ver
    wxMenu* viewMenu = new wxMenu();
    viewMenu->AppendCheckItem(ID_MENU_VIEW_LEFT, wxT("Panel &Izquierdo\tF4"), wxT("Mostrar/ocultar panel izquierdo"));
    viewMenu->Check(ID_MENU_VIEW_LEFT, true);
    viewMenu->AppendCheckItem(ID_MENU_VIEW_RIGHT, wxT("Panel &Derecho\tF5"), wxT("Mostrar/ocultar panel derecho"));
    viewMenu->Check(ID_MENU_VIEW_RIGHT, true);
    viewMenu->AppendCheckItem(ID_MENU_VIEW_BOTTOM, wxT("Panel &Inferior\tF6"), wxT("Mostrar/ocultar panel inferior"));
    viewMenu->Check(ID_MENU_VIEW_BOTTOM, true);
    viewMenu->AppendSeparator();
    viewMenu->AppendCheckItem(ID_MENU_VIEW_GRID, wxT("Mostrar &Grilla\tCtrl+G"), wxT("Mostrar/ocultar grilla"));
    viewMenu->Check(ID_MENU_VIEW_GRID, true);
    viewMenu->AppendCheckItem(ID_MENU_VIEW_FIELD, wxT("Mostrar Campo &Vectorial\tCtrl+V"), wxT("Mostrar/ocultar campo vectorial"));
    viewMenu->Check(ID_MENU_VIEW_FIELD, true);
    viewMenu->AppendCheckItem(ID_MENU_VIEW_TRAJECTORY, wxT("Mostrar &Trayectoria\tCtrl+T"), wxT("Mostrar/ocultar trayectoria"));
    viewMenu->Check(ID_MENU_VIEW_TRAJECTORY, true);
    menuBar->Append(viewMenu, wxT("&Ver"));
    
    // Menú Modo
    wxMenu* modeMenu = new wxMenu();
    modeMenu->AppendRadioItem(ID_MENU_MODE_THEORY, wxT("&Teoría\tF1"), wxT("Modo teoría educativa"));
    modeMenu->AppendRadioItem(ID_MENU_MODE_SIMULATION, wxT("&Simulación\tF2"), wxT("Modo simulación"));
    modeMenu->AppendRadioItem(ID_MENU_MODE_EDITOR, wxT("&Editor\tF3"), wxT("Modo editor de escenarios"));
    modeMenu->Check(ID_MENU_MODE_SIMULATION, true);
    menuBar->Append(modeMenu, wxT("&Modo"));
    
    // Menú Simulación
    wxMenu* simMenu = new wxMenu();
    simMenu->Append(ID_MENU_START, wxT("&Iniciar\tSpace"), wxT("Iniciar simulación"));
    simMenu->Append(ID_MENU_PAUSE, wxT("&Pausar\tP"), wxT("Pausar/reanudar simulación"));
    simMenu->Append(ID_MENU_RESET, wxT("&Reiniciar\tR"), wxT("Reiniciar simulación"));
    simMenu->Append(ID_MENU_STEP, wxT("Paso a &Paso\tCtrl+Right"), wxT("Avanzar un paso de simulación"));
    menuBar->Append(simMenu, wxT("Si&mulación"));
    
    // Menú Herramientas
    wxMenu* toolsMenu = new wxMenu();
    toolsMenu->Append(ID_MENU_RESET_VIEW, wxT("&Reiniciar Vista"), wxT("Restaurar layout por defecto"));
    toolsMenu->AppendSeparator();
    toolsMenu->Append(ID_MENU_CLEAR_TRAJECTORY, wxT("&Limpiar Trayectoria"), wxT("Borrar trayectoria actual"));
    toolsMenu->Append(ID_MENU_CAPTURE_SCREEN, wxT("&Capturar Pantalla"), wxT("Guardar imagen del canvas"));
    toolsMenu->AppendSeparator();
    toolsMenu->Append(ID_MENU_CUSTOM_FIELD, wxT("Campo &Vectorial Personalizado..."), wxT("Definir campo en formato LaTeX"));
    menuBar->Append(toolsMenu, wxT("&Herramientas"));
    
    // Menú Ayuda
    wxMenu* helpMenu = new wxMenu();
    helpMenu->Append(ID_MENU_HELP, wxT("&Guía Rápida\tF1"), wxT("Mostrar ayuda"));
    helpMenu->AppendSeparator();
    helpMenu->Append(ID_MENU_ABOUT, wxT("&Acerca de..."), wxT("Información sobre la aplicación"));
    menuBar->Append(helpMenu, wxT("A&yuda"));
    
    SetMenuBar(menuBar);
}

void MainWindow::setupToolbar()
{
    // Toolbar moderno con fondo claro y separadores visuales
    wxToolBar* toolbar = CreateToolBar(wxTB_HORIZONTAL | wxTB_FLAT | wxTB_TEXT);
    toolbar->SetBackgroundColour(ModernColors::PANEL_BG);
    toolbar->SetMargins(8, 8);
    
    // Grupo: Archivo
    toolbar->AddTool(ID_MENU_NEW, wxT("Nuevo"), 
                     wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR, wxSize(28, 28)),
                     wxT("Crear nuevo escenario (Ctrl+N)"));
    toolbar->AddTool(ID_MENU_OPEN, wxT("Abrir"),
                     wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_TOOLBAR, wxSize(28, 28)),
                     wxT("Abrir escenario (Ctrl+O)"));
    toolbar->AddTool(ID_MENU_SAVE, wxT("Guardar"),
                     wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_TOOLBAR, wxSize(28, 28)),
                     wxT("Guardar escenario (Ctrl+S)"));
    
    // Separador visual
    toolbar->AddSeparator();
    toolbar->AddSeparator();
    
    // Grupo: Modos
    toolbar->AddTool(ID_MENU_MODE_THEORY, wxT("Teoría"),
                     wxArtProvider::GetBitmap(wxART_HELP_BOOK, wxART_TOOLBAR, wxSize(28, 28)),
                     wxT("Modo teoría educativa (F1)"));
    toolbar->AddTool(ID_MENU_MODE_SIMULATION, wxT("Simulación"),
                     wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE, wxART_TOOLBAR, wxSize(28, 28)),
                     wxT("Modo simulación (F2)"));
    toolbar->AddTool(ID_MENU_MODE_EDITOR, wxT("Editor"),
                     wxArtProvider::GetBitmap(wxART_EDIT, wxART_TOOLBAR, wxSize(28, 28)),
                     wxT("Modo editor de escenarios (F3)"));
    
    toolbar->AddSeparator();
    toolbar->AddSeparator();
    
    // Grupo: Control de Simulación
    toolbar->AddTool(ID_MENU_START, wxT("▶ Iniciar"),
                     wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_TOOLBAR, wxSize(28, 28)),
                     wxT("Iniciar simulación (Espacio)"));
    toolbar->AddTool(ID_MENU_PAUSE, wxT("⏸ Pausar"),
                     wxArtProvider::GetBitmap(wxART_STOP, wxART_TOOLBAR, wxSize(28, 28)),
                     wxT("Pausar/Reanudar simulación (P)"));
    toolbar->AddTool(ID_MENU_RESET, wxT("⏹ Reiniciar"),
                     wxArtProvider::GetBitmap(wxART_UNDO, wxART_TOOLBAR, wxSize(28, 28)),
                     wxT("Reiniciar simulación (R)"));
    
    toolbar->AddSeparator();
    
    toolbar->AddTool(ID_TOGGLE_3D, wxT("3D"),
                     wxArtProvider::GetBitmap(wxART_HELP_SETTINGS, wxART_TOOLBAR, wxSize(28, 28)),
                     wxT("Alternar vista 3D (Ctrl+3)"));
    
    toolbar->Realize();
}

void MainWindow::setupAuiLayout()
{
    // Crear panel izquierdo (navegación)
    leftPanel_ = new LeftPanel(this, this);
    wxAuiPaneInfo leftInfo;
    leftInfo.Name(wxT("leftPanel"))
           .Caption(wxT("Navegación"))
           .Left()
           .Layer(1)
           .Position(0)
           .BestSize(250, 800)
           .MinSize(200, 500)
           .CloseButton(true)
           .MaximizeButton(false);
    auiManager_.AddPane(leftPanel_, leftInfo);
    
    // Crear panel derecho (inspector)
    rightPanel_ = new RightPanel(this, this);
    wxAuiPaneInfo rightInfo;
    rightInfo.Name(wxT("rightPanel"))
            .Caption(wxT("Propiedades"))
            .Right()
            .Layer(1)
            .Position(0)
            .BestSize(320, 800)
            .MinSize(280, 500)
            .CloseButton(true)
            .MaximizeButton(false);
    auiManager_.AddPane(rightPanel_, rightInfo);
    
    experimentService_.addConfigChangeListener([this]() {
        if (rightPanel_) {
            rightPanel_->updateProperties();
        }
    });
    
    // Crear panel inferior (resultados)
    bottomPanel_ = new BottomPanel(this, this);
    wxAuiPaneInfo bottomInfo;
    bottomInfo.Name(wxT("bottomPanel"))
             .Caption(wxT("Resultados"))
             .Bottom()
             .Layer(1)
             .Position(0)
             .BestSize(1200, 220)
             .MinSize(600, 150)
             .CloseButton(true)
             .MaximizeButton(false);
    auiManager_.AddPane(bottomPanel_, bottomInfo);
    
    // Crear panel central con múltiples vistas
    // 1. Canvas para simulación 2D
    canvas_ = new SimulationCanvas(this);
    wxAuiPaneInfo canvasInfo;
    canvasInfo.Name(wxT("canvas"))
              .Caption(wxT("Escenario"))
              .Center()
              .Layer(0)
              .Position(0)
              .BestSize(1400, 800)
              .MinSize(600, 400);
    auiManager_.AddPane(canvas_, canvasInfo);
    
    // 2. Canvas para simulación 3D
    canvas3D_ = new Water3DCanvas(this);
    wxAuiPaneInfo canvas3DInfo;
    canvas3DInfo.Name(wxT("canvas3D"))
                .Caption(wxT("Escenario 3D"))
                .Center()
                .Layer(0)
                .Position(0)
                .BestSize(1400, 800)
                .MinSize(600, 400)
                .Hide();
    auiManager_.AddPane(canvas3D_, canvas3DInfo);

    if (leftPanel_) {
        leftPanel_->applyLayerVisibility();
    }
    
    // 2. Panel de teoría
    theoryPanel_ = new TheoryPanel(this);
    theoryPanel_->setScenarioCallback([this](const wxString& scenarioName) {
        onTheoryScenarioRequest(scenarioName);
    });
    wxAuiPaneInfo theoryInfo;
    theoryInfo.Name(wxT("theoryPanel"))
               .Caption(wxT("Teoría"))
               .Center()
               .Layer(0)
               .Position(0)
               .BestSize(800, 600)
               .MinSize(400, 300)
               .Hide();
    auiManager_.AddPane(theoryPanel_, theoryInfo);
    
    // 3. Panel de editor
    editorPanel_ = new EditorPanel(this, this);
    wxAuiPaneInfo editorInfo;
    editorInfo.Name(wxT("editorPanel"))
              .Caption(wxT("Editor"))
              .Center()
              .Layer(0)
              .Position(0)
              .BestSize(800, 600)
              .MinSize(400, 300)
              .Hide();
    auiManager_.AddPane(editorPanel_, editorInfo);
    
    // Aplicar layout
    auiManager_.Update();
}

void MainWindow::setupStatusBar()
{
    CreateStatusBar(5);
    int widths[] = {250, 180, 180, 150, 120};
    SetStatusWidths(5, widths);
    
    // Configurar colores de la barra de estado
    SetStatusText(wxT("◉ Listo para simular"), 0);
    SetStatusText(wxT("📍 Modo: Simulación"), 1);
    SetStatusText(wxT("⏱ t: 0.00s"), 2);
    SetStatusText(wxT("📊 v: 0.00 m/s"), 3);
    SetStatusText(wxT("🔧 Método: Euler"), 4);
}

void MainWindow::setMode(AppMode mode)
{
    if (currentMode_ == mode) return;
    
    currentMode_ = mode;
    
    // Actualizar menú
    wxMenuBar* menuBar = GetMenuBar();
    if (menuBar) {
        menuBar->Check(ID_MENU_MODE_THEORY, mode == AppMode::Theory);
        menuBar->Check(ID_MENU_MODE_SIMULATION, mode == AppMode::Simulation);
        menuBar->Check(ID_MENU_MODE_EDITOR, mode == AppMode::Editor);
    }
    
    // Actualizar barra de estado
    wxString modeText;
    switch (mode) {
        case AppMode::Theory: modeText = wxT("Modo: Teoría"); break;
        case AppMode::Simulation: modeText = wxT("Modo: Simulación"); break;
        case AppMode::Editor: modeText = wxT("Modo: Editor"); break;
    }
    SetStatusText(modeText, 1);
    
    // Cambiar panel central visible
    if (auiManager_.GetPane(canvas_).IsOk()) {
        auiManager_.GetPane(canvas_).Show(mode == AppMode::Simulation);
    }
    if (auiManager_.GetPane(theoryPanel_).IsOk()) {
        auiManager_.GetPane(theoryPanel_).Show(mode == AppMode::Theory);
    }
    if (auiManager_.GetPane(editorPanel_).IsOk()) {
        auiManager_.GetPane(editorPanel_).Show(mode == AppMode::Editor);
    }
    auiManager_.Update();
    
    // Notificar a componentes
    if (canvas_) {
        canvas_->setMode(mode);
    }
    if (rightPanel_) {
        rightPanel_->updateProperties();
    }
}

void MainWindow::updateResults()
{
    if (bottomPanel_) {
        bottomPanel_->updateResults();
    }
}

void MainWindow::updateCanvas()
{
    auto& config = experimentService_.getConfig();
    double liveBoatX = config.boat.getPosition().x;
    double liveBoatY = config.boat.getPosition().y;

    if (!trajectoryPoints_.empty()) {
        liveBoatX = trajectoryPoints_.back().x;
        liveBoatY = trajectoryPoints_.back().y;
    }

    if (canvas_) {
        canvas_->setScenario(&config.scenario);
        canvas_->setField(&config.field);
        canvas_->setBoat(&config.boat);
        canvas_->updateBoatPosition(liveBoatX, liveBoatY);
        canvas_->Refresh();
    }

    if (canvas3D_) {
        canvas3D_->setScenario(&config.scenario);
        canvas3D_->setVectorField(&config.field);
        canvas3D_->setBoat(&config.boat);
        canvas3D_->setBoatPosition(liveBoatX, liveBoatY);
        canvas3D_->setSimulationTime(currentTime_);
        canvas3D_->setTrajectory(trajectoryPoints_);
        if (is3DMode_) {
            canvas3D_->refreshCanvas();
        }
    }

    if (leftPanel_) {
        leftPanel_->applyLayerVisibility();
    }
}

// Funciones del editor - delegar al canvas
void MainWindow::updateEditorBrushSize(int size) {
    if (canvas_) {
        canvas_->setBrushSize(size);
    }
}

void MainWindow::updateEditorSnap(bool snap) {
    if (canvas_) {
        canvas_->setSnapToGrid(snap);
    }
}

void MainWindow::setEditorMode(bool editor) {
    if (canvas_) {
        canvas_->setEditorMode(editor);
    }
}

// Funciónes auxiliares refactorizadas

bool MainWindow::confirmSaveChanges(const wxString& action)
{
    if (!isModified_) return true;
    
    int result = wxMessageBox(wxT("¿Desea guardar los cambios antes de ") + action + wxT("?"),
                              wxT("Confirmar"), wxYES_NO | wxCANCEL | wxICON_QUESTION);
    if (result == wxCANCEL) return false;
    if (result == wxYES) {
        wxCommandEvent dummyEvent;
        onSave(dummyEvent);
    }
    return true;
}

void MainWindow::stripExtension(wxString& filename, const wxString& ext)
{
    if (filename.EndsWith(ext)) {
        filename = filename.Left(filename.Length() - ext.Length());
    }
}

void MainWindow::onNew(wxCommandEvent& event)
{
    (void)event;
    
    if (!confirmSaveChanges(wxT("crear un nuevo escenario"))) return;
    
    experimentService_.createNewExperiment();
    currentScenarioName_ = wxT("Nuevo Escenario");
    isModified_ = false;
    updateWindowTitle();
    resetSimulation();
    
    if (bottomPanel_) {
        bottomPanel_->addEvent(wxT("Nuevo escenario creado"));
    }
}

void MainWindow::onOpen(wxCommandEvent& event)
{
    (void)event;
    
    if (!confirmSaveChanges(wxT("abrir otro escenario"))) return;
    
    wxFileDialog openDialog(this, wxT("Abrir escenario"), wxT(""), wxT(""),
                           wxT("Escenarios (*.scenario)|*.scenario|Todos los archivos (*.*)|*.*"),
                           wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openDialog.ShowModal() == wxID_OK) {
        wxString path = openDialog.GetPath();
        experimentService_.loadExperiment(path.ToStdString());
        
        currentScenarioName_ = openDialog.GetFilename();
        stripExtension(currentScenarioName_, wxT(".scenario"));
        
        isModified_ = false;
        updateWindowTitle();
        resetSimulation();
        
        if (bottomPanel_) {
            bottomPanel_->addEvent(wxT("Escenario abierto: ") + path);
        }
    }
}

void MainWindow::onSave(wxCommandEvent& event)
{
    (void)event;
    wxFileDialog saveDialog(this, wxT("Guardar escenario"), wxT(""), currentScenarioName_,
                           wxT("Escenarios (*.scenario)|*.scenario"),
                           wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveDialog.ShowModal() == wxID_OK) {
        wxString path = saveDialog.GetPath();
        experimentService_.saveExperiment(path.ToStdString());
        
        currentScenarioName_ = saveDialog.GetFilename();
        stripExtension(currentScenarioName_, wxT(".scenario"));
        
        isModified_ = false;
        updateWindowTitle();
        
        if (bottomPanel_) {
            bottomPanel_->addEvent(wxT("Escenario guardado: ") + path);
        }
    }
}

void MainWindow::onExportImage(wxCommandEvent& event)
{
    (void)event;
    wxFileDialog exportDialog(this, wxT("Exportar imagen"), wxT(""), wxT("captura.png"),
                             wxT("PNG (*.png)|*.png|JPEG (*.jpg)|*.jpg|Bitmap (*.bmp)|*.bmp"),
                             wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (exportDialog.ShowModal() == wxID_OK && canvas_) {
        wxString path = exportDialog.GetPath();
        wxBitmapType type = wxBITMAP_TYPE_PNG;
        
        if (path.EndsWith(wxT(".jpg")) || path.EndsWith(wxT(".jpeg"))) {
            type = wxBITMAP_TYPE_JPEG;
        } else if (path.EndsWith(wxT(".bmp"))) {
            type = wxBITMAP_TYPE_BMP;
        }
        
        // Capturar el canvas
        wxClientDC dc(canvas_);
        wxSize size = canvas_->GetClientSize();
        wxBitmap bitmap(size);
        wxMemoryDC memDC;
        memDC.SelectObject(bitmap);
        memDC.Blit(0, 0, size.GetWidth(), size.GetHeight(), &dc, 0, 0);
        memDC.SelectObject(wxNullBitmap);
        
        if (bitmap.SaveFile(path, type)) {
            wxMessageBox(wxT("Imagen exportada exitosamente a:\n") + path, 
                        wxT("Exportar Imagen"), wxOK | wxICON_INFORMATION);
            if (bottomPanel_) {
                bottomPanel_->addEvent(wxT("Imagen exportada: ") + path);
            }
        } else {
            wxMessageBox(wxT("Error al guardar la imagen"), 
                        wxT("Error"), wxOK | wxICON_ERROR);
        }
    }
}

void MainWindow::onExportData(wxCommandEvent& event)
{
    (void)event;
    wxFileDialog exportDialog(this, wxT("Exportar datos"), wxT(""), wxT("datos.csv"),
                             wxT("CSV (*.csv)|*.csv|Texto (*.txt)|*.txt"),
                             wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (exportDialog.ShowModal() == wxID_OK) {
        wxString path = exportDialog.GetPath();
        
        std::ofstream file(path.ToStdString());
        if (file.is_open()) {
            // Encabezados CSV
            file << "Tiempo(s),PosX,PosY,VelX,VelY,Velocidad(m/s),Energia(J)\n";
            
            // Datos de trayectoria
            for (const auto& point : trajectoryPoints_) {
                file << std::fixed << std::setprecision(3)
                     << point.time << ","
                     << point.x << ","
                     << point.y << ","
                     << "0.000,0.000,"
                     << currentSpeed_ << ","
                     << currentEnergy_ << "\n";
            }
            
            file.close();
            
            wxMessageBox(wxT("Datos exportados exitosamente a:\n") + path + 
                        wxT("\n\nTotal de puntos: ") + wxString::Format(wxT("%d"), (int)trajectoryPoints_.size()), 
                        wxT("Exportar Datos"), wxOK | wxICON_INFORMATION);
            if (bottomPanel_) {
                bottomPanel_->addEvent(wxT("Datos exportados: ") + path);
            }
        } else {
            wxMessageBox(wxT("No se pudo crear el archivo"), 
                        wxT("Error"), wxOK | wxICON_ERROR);
        }
    }
}

void MainWindow::onExit(wxCommandEvent& event)
{
    (void)event;
    Close(true);
}

void MainWindow::onViewLeftPanel(wxCommandEvent& event)
{
    wxAuiPaneInfo& pane = auiManager_.GetPane(leftPanel_);
    pane.Show(event.IsChecked());
    auiManager_.Update();
}

void MainWindow::onViewRightPanel(wxCommandEvent& event)
{
    wxAuiPaneInfo& pane = auiManager_.GetPane(rightPanel_);
    pane.Show(event.IsChecked());
    auiManager_.Update();
}

void MainWindow::onViewBottomPanel(wxCommandEvent& event)
{
    wxAuiPaneInfo& pane = auiManager_.GetPane(bottomPanel_);
    pane.Show(event.IsChecked());
    auiManager_.Update();
}

void MainWindow::onViewGrid(wxCommandEvent& event)
{
    if (canvas_) {
        canvas_->setShowGrid(event.IsChecked());
    }
}

void MainWindow::onViewField(wxCommandEvent& event)
{
    if (canvas_) {
        canvas_->setShowField(event.IsChecked());
    }
}

void MainWindow::onViewTrajectory(wxCommandEvent& event)
{
    if (canvas_) {
        canvas_->setShowTrajectory(event.IsChecked());
    }
}

void MainWindow::onModeTheory(wxCommandEvent& event)
{
    setMode(AppMode::Theory);
}

void MainWindow::onModeSimulation(wxCommandEvent& event)
{
    setMode(AppMode::Simulation);
}

void MainWindow::onModeEditor(wxCommandEvent& event)
{
    setMode(AppMode::Editor);
}

void MainWindow::onTheoryScenarioRequest(const wxString& scenarioName)
{
    setMode(AppMode::Simulation);
    
    auto& config = experimentService_.getConfig();
    
    int index = -1;
    if (scenarioName.Contains(wxT("Río")) || scenarioName.Contains(wxT("1."))) {
        index = 0;
    } else if (scenarioName.Contains(wxT("Remolino")) || scenarioName.Contains(wxT("2."))) {
        index = 1;
    } else if (scenarioName.Contains(wxT("Canal")) || scenarioName.Contains(wxT("3."))) {
        index = 2;
    } else if (scenarioName.Contains(wxT("Turbulenta")) || scenarioName.Contains(wxT("4."))) {
        index = 3;
    } else if (scenarioName.Contains(wxT("Laberinto")) || scenarioName.Contains(wxT("5."))) {
        index = 4;
    } else if (scenarioName.Contains(wxT("Rápidos")) || scenarioName.Contains(wxT("6."))) {
        index = 5;
    }
    
    if (index >= 0 && leftPanel_) {
        leftPanel_->updateScenarioList();
        if (leftPanel_->getSelectedScenarioIndex() != index) {
            // Dispatch event to select the scenario
            wxCommandEvent evt(wxEVT_COMMAND_LISTBOX_SELECTED, ID_SCENARIO_LIST);
            evt.SetInt(index);
            ProcessWindowEvent(evt);
        }
    }
    
    // Reset simulation state
    resetSimulation();
    updateCanvas();
    SetStatusText(wxT("Escenario cargado desde teoría"), 0);
}

void MainWindow::onStartSimulation(wxCommandEvent& event)
{
    (void)event;
    if (!isRunning_) {
        trajectoryPoints_.clear();
        if (canvas_) {
            canvas_->clearTrajectory();
        }
        if (canvas3D_) {
            canvas3D_->clearTrajectory();
        }
        if (bottomPanel_) {
            bottomPanel_->clearGraphs();
        }
        isRunning_ = true;
        isPaused_ = false;
        // Obtener paso temporal del slider (conversión de segundos a ms)
        int dtMs = static_cast<int>(getTimeStep() * 1000.0);
        timer_->Start(dtMs);
        SetStatusText(wxT("Simulación en curso"), 0);
        if (bottomPanel_) {
            bottomPanel_->addEvent(wxT("Simulación iniciada"));
        }
    }
}

void MainWindow::onPauseSimulation(wxCommandEvent& event)
{
    (void)event;
    if (isRunning_) {
        isPaused_ = !isPaused_;
        if (isPaused_) {
            timer_->Stop();
            SetStatusText(wxT("Simulación pausada"), 0);
        } else {
            timer_->Start(33);
            SetStatusText(wxT("Simulación en curso"), 0);
        }
    }
}

void MainWindow::onResetSimulation(wxCommandEvent& event)
{
    (void)event;
    resetSimulation();
}

void MainWindow::onStepSimulation(wxCommandEvent& event)
{
    (void)event;
    if (isRunning_ && isPaused_) {
        runSimulationStep();
    }
}

void MainWindow::onMethodChange(wxCommandEvent& event)
{
    (void)event;
    wxString methods[] = {wxT("Euler"), wxT("Euler Mejorado"), wxT("RK4")};
    int methodIdx = 0;
    if (rightPanel_) {
        methodIdx = rightPanel_->getMethod();
    }
    SetStatusText(wxT("Método: ") + methods[methodIdx], 3);
}

void MainWindow::onDtChange(wxCommandEvent& event)
{
    (void)event;
    if (timer_ && timer_->IsRunning() && rightPanel_) {
        int dtMs = static_cast<int>(rightPanel_->getTimeStep() * 1000.0);
        timer_->Start(dtMs);
    }
}

void MainWindow::onAbout(wxCommandEvent& event)
{
    (void)event;
    wxAboutDialogInfo info;
    info.SetName(wxT("Taller Paramétrico de Navegación y Campos 2D"));
    info.SetVersion(wxT("0.1.0"));
    info.SetDescription(wxT("Una aplicación educativa para explorar campos vectoriales, ")
                       wxT("trayectorias y métodos numéricos en escenarios 2D."));
    info.SetCopyright(wxT("© 2025 - Proyecto de C++"));
    wxAboutBox(info);
}

void MainWindow::onHelp(wxCommandEvent& event)
{
    (void)event;
    wxMessageBox(
        wxT("Guía Rápida:\n\n")
        wxT("F1 - Modo Teoría\n")
        wxT("F2 - Modo Simulación\n")
        wxT("F3 - Modo Editor\n")
        wxT("F4 - Mostrar/ocultar panel izquierdo\n")
        wxT("F5 - Mostrar/ocultar panel derecho\n")
        wxT("F6 - Mostrar/ocultar panel inferior\n")
        wxT("Espacio - Iniciar simulación\n")
        wxT("P - Pausar simulación\n")
        wxT("R - Reiniciar simulación"),
        wxT("Ayuda"),
        wxOK | wxICON_INFORMATION
    );
}

void MainWindow::onResetView(wxCommandEvent& event)
{
    (void)event;
    // Restaurar layout por defecto
    if (auiManager_.GetPane(leftPanel_).IsOk()) {
        auiManager_.GetPane(leftPanel_).Show(true);
    }
    if (auiManager_.GetPane(rightPanel_).IsOk()) {
        auiManager_.GetPane(rightPanel_).Show(true);
    }
    if (auiManager_.GetPane(bottomPanel_).IsOk()) {
        auiManager_.GetPane(bottomPanel_).Show(true);
    }
    auiManager_.Update();
    
    // Actualizar checks del menú Ver
    wxMenuBar* menuBar = GetMenuBar();
    if (menuBar) {
        menuBar->Check(ID_MENU_VIEW_LEFT, true);
        menuBar->Check(ID_MENU_VIEW_RIGHT, true);
        menuBar->Check(ID_MENU_VIEW_BOTTOM, true);
    }
    
    if (bottomPanel_) {
        bottomPanel_->addEvent(wxT("Vista reiniciada a valores por defecto"));
    }
}

void MainWindow::onClearTrajectory(wxCommandEvent& event)
{
    (void)event;
    trajectoryPoints_.clear();
    if (canvas_) {
        canvas_->clearTrajectory();
    }
    if (bottomPanel_) {
        bottomPanel_->addEvent(wxT("Trayectoria limpiada"));
    }
}

void MainWindow::onCaptureScreen(wxCommandEvent& event)
{
    (void)event;
    // Usar la misma función que Exportar Imagen
    wxCommandEvent dummyEvent;
    onExportImage(dummyEvent);
}

void MainWindow::OnClose(wxCloseEvent& event) {
    if (canvas3D_) {
        canvas3D_->stopAnimation();
    }
    event.Skip();
}

void MainWindow::onToggle3D(wxCommandEvent& event)
{
    (void)event;
    is3DMode_ = !is3DMode_;
    
    if (is3DMode_) {
        auiManager_.GetPane(canvas_).Hide();
        auiManager_.GetPane(canvas3D_).Show();

        updateCanvas();
        if (leftPanel_) {
            leftPanel_->applyLayerVisibility();
        }
        
        auiManager_.Update();
        Layout();
        canvas3D_->Show();
        canvas3D_->Refresh();
        canvas3D_->Update();
        canvas3D_->startAnimation();
    } else {
        auiManager_.GetPane(canvas3D_).Hide();
        auiManager_.GetPane(canvas_).Show();
        canvas3D_->stopAnimation();
    }
    
    auiManager_.Update();
    Refresh();
}

void MainWindow::onCustomField(wxCommandEvent& event)
{
    (void)event;
    
    wxDialog dialog(this, wxID_ANY, wxT("Campo Vectorial Personalizado"), 
                    wxDefaultPosition, wxSize(500, 400));
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Instrucciones
    mainSizer->Add(new wxStaticText(&dialog, wxID_ANY, 
        wxT("Ingrese las componentes del campo vectorial F(x,y) = (Fx, Fy)")), 
        0, wxALL, 10);
    
    // Fx
    wxBoxSizer* fxSizer = new wxBoxSizer(wxHORIZONTAL);
    fxSizer->Add(new wxStaticText(&dialog, wxID_ANY, wxT("Fx(x,y) = ")), 0, wxALIGN_CENTER_VERTICAL);
    wxTextCtrl* fxCtrl = new wxTextCtrl(&dialog, wxID_ANY, wxT("0"));
    fxSizer->Add(fxCtrl, 1, wxEXPAND);
    mainSizer->Add(fxSizer, 0, wxEXPAND | wxALL, 5);
    
    // Fy
    wxBoxSizer* fySizer = new wxBoxSizer(wxHORIZONTAL);
    fySizer->Add(new wxStaticText(&dialog, wxID_ANY, wxT("Fy(x,y) = ")), 0, wxALIGN_CENTER_VERTICAL);
    wxTextCtrl* fyCtrl = new wxTextCtrl(&dialog, wxID_ANY, wxT("0"));
    fySizer->Add(fyCtrl, 1, wxEXPAND);
    mainSizer->Add(fySizer, 0, wxEXPAND | wxALL, 5);
    
    // Ejemplos predefinidos
    mainSizer->Add(new wxStaticText(&dialog, wxID_ANY, wxT("Ejemplos:")), 0, wxALL, 5);
    wxListBox* examples = new wxListBox(&dialog, wxID_ANY);
    examples->Append(wxT("Uniforme: Fx=1, Fy=0"));
    examples->Append(wxT("Rotacional: Fx=-y, Fy=x"));
    examples->Append(wxT("Radial: Fx=x, Fy=y"));
    examples->Append(wxT("Gradiente: Fx=x, Fy=-y"));
    examples->Append(wxT("Caracol: Fx=-0.08*(x-25)-0.25*(y-25), Fy=-0.08*(y-25)+0.25*(x-25)"));
    examples->Bind(wxEVT_LISTBOX, [fxCtrl, fyCtrl, examples](wxCommandEvent& evt) {
        int sel = evt.GetSelection();
        if (sel == wxNOT_FOUND) return;
        switch (sel) {
            case 0: fxCtrl->SetValue(wxT("1")); fyCtrl->SetValue(wxT("0")); break;
            case 1: fxCtrl->SetValue(wxT("-(y-25)")); fyCtrl->SetValue(wxT("x-25")); break;
            case 2: fxCtrl->SetValue(wxT("x-25")); fyCtrl->SetValue(wxT("y-25")); break;
            case 3: fxCtrl->SetValue(wxT("x")); fyCtrl->SetValue(wxT("-y")); break;
            case 4: fxCtrl->SetValue(wxT("-0.08*(x-25)-0.25*(y-25)")); fyCtrl->SetValue(wxT("-0.08*(y-25)+0.25*(x-25)")); break;
            default: break;
        }
    });
    mainSizer->Add(examples, 1, wxEXPAND | wxALL, 5);
    
    // Botónes
    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* okBtn = new wxButton(&dialog, wxID_OK, wxT("Aplicar"));
    wxButton* cancelBtn = new wxButton(&dialog, wxID_CANCEL, wxT("Cancelar"));
    btnSizer->Add(okBtn, 0, wxALL, 5);
    btnSizer->Add(cancelBtn, 0, wxALL, 5);
    mainSizer->Add(btnSizer, 0, wxALIGN_CENTER | wxALL, 10);
    
    dialog.SetSizer(mainSizer);
    
    if (dialog.ShowModal() == wxID_OK) {
        wxString fxStr = fxCtrl->GetValue();
        wxString fyStr = fyCtrl->GetValue();
        
        std::string fxExpr = std::string(fxStr.mb_str());
        std::string fyExpr = std::string(fyStr.mb_str());
        
        tp::domain::MathExpression parser;
        
        if (parser.parse(fxExpr, fyExpr)) {
            auto& config = experimentService_.getConfig();
            config.field = tp::domain::VectorField([parser](double x, double y) {
                return parser.evaluate(x, y);
            });
            config.fieldView.type = tp::application::FieldPresetType::Custom;
            config.fieldView.intensity = 0.0;
            config.fieldView.centerX = 25.0;
            config.fieldView.centerY = 25.0;
            
            wxMessageBox(wxT("Campo personalizado aplicado:\nFx = ") + fxStr + wxT("\nFy = ") + fyStr,
                         wxT("Campo Vectorial"), wxOK | wxICON_INFORMATION);
            
            if (bottomPanel_) {
                bottomPanel_->addEvent(wxT("Campo personalizado aplicado"));
            }
            
            updateCanvas();
        } else {
            wxMessageBox(wxT("Error: No se pudo parsear la expresión.\nVerifique la sintaxis."),
                         wxT("Error"), wxOK | wxICON_ERROR);
        }
    }
}

void MainWindow::onTimer(wxTimerEvent& event)
{
    (void)event;
    runSimulationStep();
}

void MainWindow::runSimulationStep()
{
    if (!isRunning_ || isPaused_) return;
    
    auto& config = experimentService_.getConfig();
    
    // ACTUALIZAR VALORES EN TIEMPO REAL desde los controles del RightPanel
    if (rightPanel_) {
        // Leer valores actuales de los sliders
        double thrust = rightPanel_->getThrustValue();
        double direction = rightPanel_->getDirectionValue();
        
        // Si está activado el empuje automático, calcular dirección óptima
        if (rightPanel_->isAutoThrustEnabled()) {
            // El bote apunta automáticamente hacia la dirección del campo
            // para maximizar la velocidad resultante
            Vec2d fieldVec = config.field.getValue(config.boat.getPosition().x, 
                                                    config.boat.getPosition().y);
            if (fieldVec.magnitude() > 0.1) {
                // Calcular ángulo del campo y añadir un offset para navegar eficientemente
                double fieldAngle = std::atan2(fieldVec.y, fieldVec.x);
                // El bote navega a 30 grados de la corriente para avanzar
                direction = fieldAngle; // Navegar con la corriente
            }
        }
        
        // Actualizar motor en tiempo real
        config.motor = tp::domain::Motor::constant(thrust, direction);
        
        // Actualizar campo en tiempo real
        int fieldType = rightPanel_->getFieldType();
        double intensity = rightPanel_->getFieldIntensity();
        double centerX = config.fieldView.centerX;
        double centerY = config.fieldView.centerY;
        
        // Recalcular campo según tipo selecciónado
        switch (fieldType) {
            case 0: // Uniforme
                config.field = tp::domain::VectorField::uniform(intensity, direction);
                break;
            case 1: // Lineal
                config.field = tp::domain::VectorField::linear(intensity / 25.0, 0.0, 0.0, 0.0);
                break;
            case 2: // Radial
                config.field = tp::domain::VectorField::radial(centerX, centerY, intensity);
                break;
            case 3: // Rotacional
                config.field = tp::domain::VectorField::rotational(centerX, centerY, intensity);
                break;
            case 4: // Personalizado
                break;
            default:
                break;
        }
        
        // Actualizar método numérico en tiempo real
        int method = rightPanel_->getMethod();
        switch (method) {
            case 0: config.method = tp::shared::IntegrationMethod::Euler; break;
            case 1: config.method = tp::shared::IntegrationMethod::ImprovedEuler; break;
            case 2: config.method = tp::shared::IntegrationMethod::RK4; break;
        }
        
        // Actualizar paso temporal en tiempo real
        config.timeStep = rightPanel_->getTimeStep();
    }
    
    // Usar el paso temporal actualizado
    double dt = config.timeStep > 0 ? config.timeStep : 0.033;
    
    // Crear runner para un solo paso
    tp::simulation::SimulationRunner runner(config.boat, config.field, config.motor, config.scenario);
    runner.setTimeStep(dt);
    runner.setMaxTime(currentTime_ + dt + 0.001);
    
    // Selecciónar integrador según configuración ACTUAL
    switch (config.method) {
        case tp::shared::IntegrationMethod::ImprovedEuler:
            runner.setIntegrator(std::make_unique<tp::simulation::ImprovedEulerIntegrator>());
            break;
        case tp::shared::IntegrationMethod::RK4:
            runner.setIntegrator(std::make_unique<tp::simulation::RK4Integrator>());
            break;
        case tp::shared::IntegrationMethod::Euler:
        default:
            runner.setIntegrator(std::make_unique<tp::simulation::EulerIntegrator>());
            break;
    }
    
    // Ejecutar simulación
    auto result = runner.run();
    
    if (!result.states.empty()) {
        const auto& state = result.states.back();
        
        // Actualizar datos de simulación
        currentTime_ = state.time;
        currentSpeed_ = state.velocity.magnitude();
        currentEnergy_ = state.kineticEnergy;
        totalDistance_ = result.totalDistance;
        
        // Calcular desplazamiento neto (distancia en línea recta desde el inicio)
        double dx = state.position.x - initialPosition_.x;
        double dy = state.position.y - initialPosition_.y;
        netDisplacement_ = std::sqrt(dx * dx + dy * dy);
        
        // Calcular eficiencia de ruta (desplazamiento neto / distancia total)
        if (totalDistance_ > 0.01) {
            routeEfficiency_ = netDisplacement_ / totalDistance_;
        } else {
            routeEfficiency_ = 1.0;
        }
        
        // Actualizar posición del bote en el canvas
        if (canvas_) {
            canvas_->updateBoatPosition(state.position.x, state.position.y);
            
            // Agregar punto a la trayectoria
            TrajectoryPoint point;
            point.x = state.position.x;
            point.y = state.position.y;
            point.time = state.time;
            trajectoryPoints_.push_back(point);
            canvas_->setTrajectory(trajectoryPoints_);
            
            // Sistema de partículas: emitir estela del bote
            auto* particleSystem = canvas_->getParticleSystem();
            if (particleSystem) {
                Vec2d boatVel = state.velocity;
                double speed = currentSpeed_;
                
                // Emitir estela (wake) detrás del bote
                particleSystem->emitWake(state.position, boatVel, speed);
                
                // Emitir espuma alrededor del bote si hay velocidad
                if (speed > 1.0) {
                    particleSystem->emitFoam(state.position, speed * 0.2, 2);
                }
                
                // Actualizar sistema de partículas
                particleSystem->update(dt);
                
                // Aplicar efecto del campo vectorial (turbulencia)
                Vec2d fieldVec = config.field.getValue(state.position.x, state.position.y);
                if (fieldVec.magnitude() > 2.0) {
                    particleSystem->applyFieldEffect(state.position, fieldVec);
                }
            }
        }

        if (canvas3D_ && is3DMode_) {
            canvas3D_->setVectorField(&config.field);
            canvas3D_->setBoat(&config.boat);
            canvas3D_->setBoatPosition(state.position.x, state.position.y);
            canvas3D_->setSimulationTime(state.time);
            canvas3D_->setTrajectory(trajectoryPoints_);
            canvas3D_->refreshCanvas();
        }
        
        // Actualizar barra de estado con coordenadas
        wxString coords;
        coords.Printf(wxT("Coords: (%.1f, %.1f)"), state.position.x, state.position.y);
        SetStatusText(coords, 2);
        
        // Verificar si se alcanzó el tiempo máximo
        if (currentTime_ >= config.maxTime) {
            timer_->Stop();
            isRunning_ = false;
            SetStatusText(wxT("Simulación completada"), 0);
            if (bottomPanel_) {
                bottomPanel_->addEvent(wxString::Format(
                    wxT("Simulación finalizada - Tiempo: %.2f s, Distancia: %.2f m"),
                    currentTime_, totalDistance_
                ));
            }
        }
        
        // Verificar colisión
        if (state.collision) {
            timer_->Stop();
            isRunning_ = false;
            SetStatusText(wxT("Colisión detectada"), 0);
            
            if (bottomPanel_) {
                bottomPanel_->addEvent(wxT("¡Colisión detectada!"));
            }
            
            if (canvas_) {
                auto* particleSystem = canvas_->getParticleSystem();
                if (particleSystem) {
                    particleSystem->emitSplash(state.position, state.velocity, 20);
                }
            }
            
            wxMessageBox(wxT("¡Colisión detectada!"), wxT("Simulación"), wxICON_WARNING);
        }
    }
    
    // Actualizar UI de resultados
    updateResults();
    updateCanvas();
}

void MainWindow::resetSimulation()
{
    if (timer_) timer_->Stop();
    isRunning_ = false;
    isPaused_ = false;
    
    currentTime_ = 0.0;
    currentSpeed_ = 0.0;
    currentEnergy_ = 0.0;
    totalDistance_ = 0.0;
    netDisplacement_ = 0.0;
    routeEfficiency_ = 1.0;
    
    // Limpiar trayectoria acumulada
    trajectoryPoints_.clear();
    
    // Resetear bote y guardar posición inicial
    auto& config = getExperimentService().getConfig();
    config.boat.reset();
    config.boat.setPosition(tp::shared::Vec2d(10.0, 25.0));
    initialPosition_ = tp::shared::Vec2d(10.0, 25.0);
    
    // Limpiar trayectoria en canvas
    if (canvas_) {
        canvas_->clearTrajectory();
        canvas_->updateBoatPosition(10.0, 25.0);
    }
    if (canvas3D_) {
        canvas3D_->clearTrajectory();
    }
    if (bottomPanel_) {
        bottomPanel_->clearGraphs();
    }
    
    updateResults();
    updateCanvas();
    SetStatusText(wxT("Simulación reiniciada"), 0);
}

void MainWindow::updateWindowTitle()
{
    wxString title = wxT("Taller Paramétrico - ");
    title += currentScenarioName_;
    if (isModified_) {
        title += wxT(" *");
    }
    SetTitle(title);
}

void MainWindow::markAsModified(bool modified)
{
    if (isModified_ != modified) {
        isModified_ = modified;
        updateWindowTitle();
    }
}

//=============================================================================
// LEFT PANEL - Navegación y escenarios
//=============================================================================

wxBEGIN_EVENT_TABLE(LeftPanel, wxPanel)
    EVT_LISTBOX(ID_SCENARIO_LIST, LeftPanel::onScenarioSelect)
    EVT_CHECKLISTBOX(wxID_ANY, LeftPanel::onLayerToggle)
wxEND_EVENT_TABLE()

LeftPanel::LeftPanel(wxWindow* parent, MainWindow* mainWindow)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME)
    , mainWindow_(mainWindow)
    , notebook_(nullptr)
    , scenarioList_(nullptr)
    , layerList_(nullptr)
    , modeSelector_(nullptr)
{
    SetBackgroundColour(ModernColors::PANEL_BG);
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Título
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("NAVEGACIÓN"));
    title->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(title, 0, wxALL, 10);
    SetMinSize(wxSize(285, -1));
    
    // Notebook para organizar secciones
    notebook_ = new wxNotebook(this, wxID_ANY);
     
    // Pestaña: Escenarios
    wxPanel* scenarioPanel = new wxPanel(notebook_);
    wxBoxSizer* scenarioSizer = new wxBoxSizer(wxVERTICAL);
    
    scenarioList_ = new wxListBox(scenarioPanel, ID_SCENARIO_LIST);
    
    // Cargar escenarios desde ExperimentService
    if (mainWindow_) {
        auto& expSvc = mainWindow_->getExperimentService();
        for (size_t i = 0; i < expSvc.getScenarioPresetCount(); ++i) {
            scenarioList_->Append(wxString::FromUTF8(expSvc.getScenarioPresetName(i).c_str()));
        }
    }
    scenarioList_->SetSelection(0);
    scenarioSizer->Add(scenarioList_, 1, wxEXPAND | wxALL, 5);
    
    wxButton* loadBtn = new wxButton(scenarioPanel, wxID_ANY, wxT("Cargar"));
    scenarioSizer->Add(loadBtn, 0, wxEXPAND | wxALL, 5);
    
    scenarioPanel->SetSizer(scenarioSizer);
    notebook_->AddPage(scenarioPanel, wxT("Escenarios"));
    
    // Pestaña: Capas
    wxPanel* layersPanel = new wxPanel(notebook_);
    wxBoxSizer* layersSizer = new wxBoxSizer(wxVERTICAL);
    
    layerList_ = new wxCheckListBox(layersPanel, wxID_ANY);
    layerList_->Append(wxT("Grilla"));
    layerList_->Append(wxT("Campo vectorial"));
    layerList_->Append(wxT("Trayectoria"));
    layerList_->Append(wxT("Bote"));
    layerList_->Append(wxT("Obstáculos"));
    layerList_->Check(0, true);
    layerList_->Check(1, true);
    layerList_->Check(2, true);
    layerList_->Check(3, true);
    layerList_->Check(4, true);
    layerList_->Bind(wxEVT_CHECKLISTBOX, &LeftPanel::onLayerToggle, this);
    
    layersSizer->Add(layerList_, 1, wxEXPAND | wxALL, 5);
    layersPanel->SetSizer(layersSizer);
    notebook_->AddPage(layersPanel, wxT("Capas"));
    
    mainSizer->Add(notebook_, 1, wxEXPAND | wxALL, 5);
    
    SetSizer(mainSizer);
}

void LeftPanel::updateScenarioList()
{
    if (!scenarioList_ || !mainWindow_) return;
    
    scenarioList_->Clear();
    
    auto& expSvc = mainWindow_->getExperimentService();
    for (size_t i = 0; i < expSvc.getScenarioPresetCount(); ++i) {
        scenarioList_->Append(wxString::FromUTF8(expSvc.getScenarioPresetName(i).c_str()));
    }
    
    if (scenarioList_->GetCount() > 0) {
        scenarioList_->SetSelection(0);
    }
}

int LeftPanel::getSelectedScenarioIndex() const
{
    if (scenarioList_) {
        return scenarioList_->GetSelection();
    }
    return -1;
}

void LeftPanel::applyLayerVisibility()
{
    wxCommandEvent dummy;
    onLayerToggle(dummy);
}

void LeftPanel::onScenarioSelect(wxCommandEvent& event)
{
    (void)event;
    int sel = getSelectedScenarioIndex();
    if (sel < 0 || !mainWindow_) return;

    auto& service = mainWindow_->getExperimentService();
    auto preservedField = service.getConfig().field;
    auto preservedFieldView = service.getConfig().fieldView;
    
    // Usar el servicio de experimentos para aplicar el preset
    service.applyScenarioPreset(static_cast<size_t>(sel));
    service.getConfig().field = preservedField;
    service.getConfig().fieldView = preservedFieldView;
    service.notifyConfigChange();
    
    // Reiniciar simulación
    auto& config = service.getConfig();
    config.boat.reset();
    
    // Actualizar nombre del escenario
    mainWindow_->setScenarioName(wxString::FromUTF8(
        mainWindow_->getExperimentService().getScenarioPresetName(static_cast<size_t>(sel)).c_str()));
    mainWindow_->setModified(true);
    
    // Actualizar canvas
    mainWindow_->updateCanvas();
    applyLayerVisibility();
    
    // Agregar evento
    if (mainWindow_->getBottomPanel()) {
        const wxString presetName = wxString::FromUTF8(
            mainWindow_->getExperimentService().getScenarioPresetName(static_cast<size_t>(sel)).c_str());
        mainWindow_->getBottomPanel()->addEvent(wxT("Escenario cargado: ") + presetName);
    }
}

void LeftPanel::onModeSelect(wxCommandEvent& event)
{
    (void)event;
    if (!mainWindow_) return;
    
    int sel = modeSelector_->GetSelection();
    switch (sel) {
        case 0: mainWindow_->setMode(AppMode::Theory); break;
        case 1: mainWindow_->setMode(AppMode::Simulation); break;
        case 2: mainWindow_->setMode(AppMode::Editor); break;
    }
}

void LeftPanel::onLayerToggle(wxCommandEvent& event)
{
    (void)event;
    if (!mainWindow_ || !layerList_) return;

    const bool showGrid = layerList_->IsChecked(0);
    const bool showField = layerList_->IsChecked(1);
    const bool showTrajectory = layerList_->IsChecked(2);
    const bool showBoat = layerList_->IsChecked(3);
    const bool showObstacles = layerList_->IsChecked(4);

    if (mainWindow_->getCanvas()) {
        mainWindow_->getCanvas()->setShowGrid(showGrid);
        mainWindow_->getCanvas()->setShowField(showField);
        mainWindow_->getCanvas()->setShowTrajectory(showTrajectory);
        mainWindow_->getCanvas()->setShowBoat(showBoat);
        mainWindow_->getCanvas()->setShowObstacles(showObstacles);
    }

    if (mainWindow_->getCanvas3D()) {
        mainWindow_->getCanvas3D()->setShowGrid(showGrid);
        mainWindow_->getCanvas3D()->setShowField(showField);
        mainWindow_->getCanvas3D()->setShowTrajectory(showTrajectory);
        mainWindow_->getCanvas3D()->setShowBoat(showBoat);
        mainWindow_->getCanvas3D()->setShowObstacles(showObstacles);
    }
}

//=============================================================================
// RIGHT PANEL - Inspector de propiedades
//=============================================================================

wxBEGIN_EVENT_TABLE(RightPanel, wxPanel)
wxEND_EVENT_TABLE()

RightPanel::RightPanel(wxWindow* parent, MainWindow* mainWindow)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME)
    , mainWindow_(mainWindow)
    , notebook_(nullptr)
    , massCtrl_(nullptr)
    , dragCtrl_(nullptr)
    , sizeCtrl_(nullptr)
    , initialXCtrl_(nullptr)
    , initialYCtrl_(nullptr)
    , initialAngleCtrl_(nullptr)
    , thrustSlider_(nullptr)
    , directionSlider_(nullptr)
    , thrustValueText_(nullptr)
    , directionValueText_(nullptr)
    , autoThrustCheck_(nullptr)
    , fieldTypeChoice_(nullptr)
    , fieldIntensitySlider_(nullptr)
    , fieldIntensityValue_(nullptr)
    , fieldCenterXCtrl_(nullptr)
    , fieldCenterYCtrl_(nullptr)
    , methodChoice_(nullptr)
    , dtSlider_(nullptr)
    , durationCtrl_(nullptr)
{
    SetBackgroundColour(ModernColors::PANEL_BG);
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Título
    wxStaticText* title = new wxStaticText(this, wxID_ANY, wxT("PROPIEDADES"));
    title->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(title, 0, wxALL, 10);
    
    // Notebook para organizar secciones
    notebook_ = new wxNotebook(this, wxID_ANY);
    
    // Pestaña: Motor
    wxPanel* motorPanel = new wxPanel(notebook_);
    motorPanel->SetBackgroundColour(ModernColors::PANEL_BG);
    wxBoxSizer* motorSizer = new wxBoxSizer(wxVERTICAL);
    
    // Empuje con valor en tiempo real
    wxBoxSizer* thrustSizer = new wxBoxSizer(wxHORIZONTAL);
    thrustSizer->Add(new wxStaticText(motorPanel, wxID_ANY, wxT("Empuje:")), 1, wxALIGN_CENTER_VERTICAL);
    thrustValueText_ = new wxStaticText(motorPanel, wxID_ANY, wxT("5.0 N"));
    thrustValueText_->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    thrustValueText_->SetForegroundColour(ModernColors::ACCENT_BLUE);
    thrustSizer->Add(thrustValueText_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    motorSizer->Add(thrustSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    
    thrustSlider_ = new wxSlider(motorPanel, ID_THRUST_SLIDER, 50, 0, 100);
    thrustSlider_->Bind(wxEVT_SLIDER, [this](wxCommandEvent& evt) {
        double val = evt.GetInt() / 10.0;
        if (thrustValueText_) {
            thrustValueText_->SetLabel(wxString::Format(wxT("%.1f N"), val));
        }
    });
    thrustSlider_->Bind(wxEVT_SLIDER, &RightPanel::onMotorParamChange, this);
    motorSizer->Add(thrustSlider_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    // Dirección con valor en tiempo real
    wxBoxSizer* dirSizer = new wxBoxSizer(wxHORIZONTAL);
    dirSizer->Add(new wxStaticText(motorPanel, wxID_ANY, wxT("Dirección:")), 1, wxALIGN_CENTER_VERTICAL);
    directionValueText_ = new wxStaticText(motorPanel, wxID_ANY, wxT("0°"));
    directionValueText_->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    directionValueText_->SetForegroundColour(ModernColors::ACCENT_BLUE);
    dirSizer->Add(directionValueText_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    motorSizer->Add(dirSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    
    directionSlider_ = new wxSlider(motorPanel, ID_DIRECTION_SLIDER, 0, 0, 360);
    directionSlider_->Bind(wxEVT_SLIDER, [this](wxCommandEvent& evt) {
        if (directionValueText_) {
            directionValueText_->SetLabel(wxString::Format(wxT("%d°"), evt.GetInt()));
        }
    });
    directionSlider_->Bind(wxEVT_SLIDER, &RightPanel::onMotorParamChange, this);
    motorSizer->Add(directionSlider_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    motorSizer->AddStretchSpacer();
    motorPanel->SetSizer(motorSizer);
    notebook_->AddPage(motorPanel, wxT("Motor"));
    
    // Pestaña: Campo
    wxPanel* fieldPanel = new wxPanel(notebook_);
    fieldPanel->SetBackgroundColour(ModernColors::PANEL_BG);
    wxBoxSizer* fieldSizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* fieldTypeLabel = new wxStaticText(fieldPanel, wxID_ANY, wxT("📊 Tipo de campo:"));
    fieldTypeLabel->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    fieldSizer->Add(fieldTypeLabel, 0, wxLEFT | wxRIGHT | wxTOP, 10);
    
    wxString fieldTypes[] = {wxT("➡️ Uniforme"), wxT("📈 Lineal"), wxT("🎯 Radial"), wxT("🌀 Rotacional"), wxT("⚙️ Personalizado")};
    fieldTypeChoice_ = new wxChoice(fieldPanel, ID_FIELD_TYPE, wxDefaultPosition, wxDefaultSize, 5, fieldTypes);
    fieldTypeChoice_->SetSelection(0);
    fieldTypeChoice_->SetBackgroundColour(ModernColors::PANEL_BG);
    fieldTypeChoice_->Bind(wxEVT_CHOICE, &RightPanel::onFieldTypeChange, this);
    fieldSizer->Add(fieldTypeChoice_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    // Intensidad con valor en tiempo real
    wxBoxSizer* intensitySizer = new wxBoxSizer(wxHORIZONTAL);
    intensitySizer->Add(new wxStaticText(fieldPanel, wxID_ANY, wxT("Intensidad:")), 1, wxALIGN_CENTER_VERTICAL);
    fieldIntensityValue_ = new wxStaticText(fieldPanel, wxID_ANY, wxT("5.0"));
    fieldIntensityValue_->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    fieldIntensityValue_->SetForegroundColour(ModernColors::ACCENT_ORANGE);
    intensitySizer->Add(fieldIntensityValue_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    fieldSizer->Add(intensitySizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    
    fieldIntensitySlider_ = new wxSlider(fieldPanel, ID_FIELD_INTENSITY, 50, 0, 100);
    fieldIntensitySlider_->Bind(wxEVT_SLIDER, [this](wxCommandEvent& evt) {
        double val = evt.GetInt() / 10.0;
        if (fieldIntensityValue_) {
            fieldIntensityValue_->SetLabel(wxString::Format(wxT("%.1f"), val));
        }
    });
    fieldIntensitySlider_->Bind(wxEVT_SLIDER, &RightPanel::onFieldParamChange, this);
    fieldSizer->Add(fieldIntensitySlider_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    wxFlexGridSizer* centerGrid = new wxFlexGridSizer(2, 5, 5);
    centerGrid->AddGrowableCol(1);
    centerGrid->Add(new wxStaticText(fieldPanel, wxID_ANY, wxT("Centro X:")));
    fieldCenterXCtrl_ = new wxSpinCtrl(fieldPanel, wxID_ANY, wxT("25"));
    fieldCenterXCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onFieldParamChange, this);
    centerGrid->Add(fieldCenterXCtrl_, 1, wxEXPAND);
    centerGrid->Add(new wxStaticText(fieldPanel, wxID_ANY, wxT("Centro Y:")));
    fieldCenterYCtrl_ = new wxSpinCtrl(fieldPanel, wxID_ANY, wxT("25"));
    fieldCenterYCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onFieldParamChange, this);
    centerGrid->Add(fieldCenterYCtrl_, 1, wxEXPAND);
    fieldSizer->Add(centerGrid, 0, wxEXPAND | wxALL, 5);
    
    fieldSizer->AddStretchSpacer();
    fieldPanel->SetSizer(fieldSizer);
    notebook_->AddPage(fieldPanel, wxT("Campo"));
    
    // Pestaña: Bote
    wxPanel* boatPanel = new wxPanel(notebook_);
    wxBoxSizer* boatSizer = new wxBoxSizer(wxVERTICAL);
    
    wxFlexGridSizer* boatGrid = new wxFlexGridSizer(2, 5, 5);
    boatGrid->AddGrowableCol(1);
    
    boatGrid->Add(new wxStaticText(boatPanel, wxID_ANY, wxT("Masa (kg):")));
    massCtrl_ = new wxSpinCtrl(boatPanel, wxID_ANY, wxT("10"));
    massCtrl_->SetRange(1, 1000);
    massCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onBoatParamChange, this);
    boatGrid->Add(massCtrl_, 1, wxEXPAND);
    
    boatGrid->Add(new wxStaticText(boatPanel, wxID_ANY, wxT("Arrastre:")));
    dragCtrl_ = new wxSpinCtrl(boatPanel, wxID_ANY, wxT("1"));
    dragCtrl_->SetRange(0, 100);
    dragCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onBoatParamChange, this);
    boatGrid->Add(dragCtrl_, 1, wxEXPAND);
    
    boatGrid->Add(new wxStaticText(boatPanel, wxID_ANY, wxT("Tamaño:")));
    sizeCtrl_ = new wxSpinCtrl(boatPanel, wxID_ANY, wxT("5"));
    sizeCtrl_->SetRange(1, 20);
    sizeCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onBoatParamChange, this);
    boatGrid->Add(sizeCtrl_, 1, wxEXPAND);
    
    boatGrid->Add(new wxStaticText(boatPanel, wxID_ANY, wxT("Pos. Inicial X:")));
    initialXCtrl_ = new wxSpinCtrl(boatPanel, wxID_ANY, wxT("10"));
    initialXCtrl_->SetRange(0, 100);
    initialXCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onBoatParamChange, this);
    boatGrid->Add(initialXCtrl_, 1, wxEXPAND);
    
    boatGrid->Add(new wxStaticText(boatPanel, wxID_ANY, wxT("Pos. Inicial Y:")));
    initialYCtrl_ = new wxSpinCtrl(boatPanel, wxID_ANY, wxT("25"));
    initialYCtrl_->SetRange(0, 100);
    initialYCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onBoatParamChange, this);
    boatGrid->Add(initialYCtrl_, 1, wxEXPAND);
    
    boatGrid->Add(new wxStaticText(boatPanel, wxID_ANY, wxT("Ángulo inicial:")));
    initialAngleCtrl_ = new wxSpinCtrl(boatPanel, wxID_ANY, wxT("0"));
    initialAngleCtrl_->SetRange(0, 360);
    initialAngleCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onBoatParamChange, this);
    boatGrid->Add(initialAngleCtrl_, 1, wxEXPAND);
    
    boatSizer->Add(boatGrid, 0, wxEXPAND | wxALL, 10);
    boatSizer->AddStretchSpacer();
    boatPanel->SetSizer(boatSizer);
    notebook_->AddPage(boatPanel, wxT("Bote"));
    
    // Pestaña: Numérico
    wxPanel* numPanel = new wxPanel(notebook_);
    numPanel->SetBackgroundColour(ModernColors::PANEL_BG);
    wxBoxSizer* numSizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* methodLabel = new wxStaticText(numPanel, wxID_ANY, wxT("🔢 Método de integración:"));
    methodLabel->SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    numSizer->Add(methodLabel, 0, wxLEFT | wxRIGHT | wxTOP, 10);
    
    wxString methods[] = {wxT("📐 Euler (rápido)"), wxT("📐 Euler Mejorado"), wxT("📐 RK4 (preciso)")};
    methodChoice_ = new wxChoice(numPanel, ID_METHOD_CHOICE, wxDefaultPosition, wxDefaultSize, 3, methods);
    methodChoice_->SetSelection(0);
    methodChoice_->SetBackgroundColour(ModernColors::PANEL_BG);
    methodChoice_->Bind(wxEVT_CHOICE, &RightPanel::onNumericalParamChange, this);
    numSizer->Add(methodChoice_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    // Paso temporal con valor en tiempo real
    wxBoxSizer* dtSizer = new wxBoxSizer(wxHORIZONTAL);
    dtSizer->Add(new wxStaticText(numPanel, wxID_ANY, wxT("⏱️ Paso temporal:")), 1, wxALIGN_CENTER_VERTICAL);
    wxStaticText* dtValue = new wxStaticText(numPanel, wxID_ANY, wxT("33 ms"));
    dtValue->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    dtValue->SetForegroundColour(ModernColors::ACCENT_GREEN);
    dtSizer->Add(dtValue, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
    numSizer->Add(dtSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    
    dtSlider_ = new wxSlider(numPanel, ID_DT_SLIDER, 33, 10, 100);
    dtSlider_->Bind(wxEVT_SLIDER, [dtValue](wxCommandEvent& evt) {
        dtValue->SetLabel(wxString::Format(wxT("%d ms"), evt.GetInt()));
    });
    dtSlider_->Bind(wxEVT_SLIDER, &RightPanel::onNumericalParamChange, this);
    numSizer->Add(dtSlider_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    wxBoxSizer* durationSizer = new wxBoxSizer(wxHORIZONTAL);
    durationSizer->Add(new wxStaticText(numPanel, wxID_ANY, wxT("⏳ Duración máxima:")), 1, wxALIGN_CENTER_VERTICAL);
    numSizer->Add(durationSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    
    durationCtrl_ = new wxSpinCtrl(numPanel, wxID_ANY, wxT("60"));
    durationCtrl_->SetRange(1, 3600);
    durationCtrl_->SetBackgroundColour(ModernColors::PANEL_BG);
    durationCtrl_->Bind(wxEVT_SPINCTRL, &RightPanel::onDurationChange, this);
    numSizer->Add(durationCtrl_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    // Info box
    wxStaticText* infoText = new wxStaticText(numPanel, wxID_ANY, 
        wxT("💡 Tip: RK4 es más preciso pero más lento.\n   Euler es rápido pero puede acumular error."));
    infoText->SetForegroundColour(ModernColors::TEXT_SECONDARY);
    infoText->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL));
    numSizer->Add(infoText, 0, wxALL, 10);
    
    numSizer->AddStretchSpacer();
    numPanel->SetSizer(numSizer);
    notebook_->AddPage(numPanel, wxT("Numérico"));
    
    // Pestaña: Escenario
    wxPanel* scenarioTabPanel = new wxPanel(notebook_);
    scenarioTabPanel->SetBackgroundColour(ModernColors::PANEL_BG);
    wxBoxSizer* scenarioTabSizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* scenarioTitle = new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("🗺️ Escenario Actual"));
    scenarioTitle->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    scenarioTabSizer->Add(scenarioTitle, 0, wxLEFT | wxRIGHT | wxTOP, 10);
    
    wxFlexGridSizer* scenarioGrid = new wxFlexGridSizer(2, 5, 5);
    scenarioGrid->AddGrowableCol(1);
    
    scenarioGrid->Add(new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("Ancho:")));
    wxStaticText* scenarioWidthText = new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("50"));
    scenarioWidthText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    scenarioWidthText->SetForegroundColour(ModernColors::ACCENT_BLUE);
    scenarioGrid->Add(scenarioWidthText, 1, wxEXPAND);
    
    scenarioGrid->Add(new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("Alto:")));
    wxStaticText* scenarioHeightText = new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("50"));
    scenarioHeightText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    scenarioHeightText->SetForegroundColour(ModernColors::ACCENT_BLUE);
    scenarioGrid->Add(scenarioHeightText, 1, wxEXPAND);
    
    scenarioGrid->Add(new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("Celdas agua:")));
    wxStaticText* waterCellsText = new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("2500"));
    waterCellsText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    waterCellsText->SetForegroundColour(ModernColors::ACCENT_GREEN);
    scenarioGrid->Add(waterCellsText, 1, wxEXPAND);
    
    scenarioGrid->Add(new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("Celdas tierra:")));
    wxStaticText* landCellsText = new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("0"));
    landCellsText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    landCellsText->SetForegroundColour(ModernColors::ACCENT_ORANGE);
    scenarioGrid->Add(landCellsText, 1, wxEXPAND);
    
    scenarioGrid->Add(new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("Obstáculos:")));
    wxStaticText* obstaclesText = new wxStaticText(scenarioTabPanel, wxID_ANY, wxT("0"));
    obstaclesText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    obstaclesText->SetForegroundColour(ModernColors::ACCENT_RED);
    scenarioGrid->Add(obstaclesText, 1, wxEXPAND);
    
    scenarioTabSizer->Add(scenarioGrid, 0, wxEXPAND | wxALL, 10);
    
    wxStaticText* scenarioInfo = new wxStaticText(scenarioTabPanel, wxID_ANY, 
        wxT("💡 Usa el Editor (F3) para modificar el escenario\ngrafícamente."));
    scenarioInfo->SetForegroundColour(ModernColors::TEXT_SECONDARY);
    scenarioInfo->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL));
    scenarioTabSizer->Add(scenarioInfo, 0, wxALL, 10);
    
    scenarioTabSizer->AddStretchSpacer();
    scenarioTabPanel->SetSizer(scenarioTabSizer);
    notebook_->AddPage(scenarioTabPanel, wxT("Escenario"));
    
    mainSizer->Add(notebook_, 1, wxEXPAND | wxALL, 5);
    
    SetSizer(mainSizer);
}

void RightPanel::updateProperties()
{
    if (!mainWindow_) return;
    
    auto& config = mainWindow_->getExperimentService().getConfig();
    
    // Sincronizar controles del bote
    if (massCtrl_) massCtrl_->SetValue(static_cast<int>(config.boat.getMass()));
    if (dragCtrl_) dragCtrl_->SetValue(static_cast<int>(config.boat.getDragCoefficient() * 100));
    if (sizeCtrl_) sizeCtrl_->SetValue(static_cast<int>(config.boat.getSize()));
    if (initialXCtrl_) initialXCtrl_->SetValue(static_cast<int>(config.boat.getPosition().x));
    if (initialYCtrl_) initialYCtrl_->SetValue(static_cast<int>(config.boat.getPosition().y));
    if (initialAngleCtrl_) initialAngleCtrl_->SetValue(static_cast<int>(config.boat.getOrientation() * 180 / 3.14159));
    
    // Sincronizar controles del motor
    if (thrustSlider_) {
        double thrust = config.motor.getThrustMagnitude(0.0);
        thrustSlider_->SetValue(std::clamp(static_cast<int>(thrust * 10), 0, 100));
        if (thrustValueText_) {
            thrustValueText_->SetLabel(wxString::Format(wxT("%.1f N"), thrust));
        }
    }
    if (directionSlider_) {
        double dir = config.motor.getDirection();
        directionSlider_->SetValue(static_cast<int>(dir * 180 / 3.14159));
        if (directionValueText_) {
            directionValueText_->SetLabel(wxString::Format(wxT("%d°"), static_cast<int>(dir * 180 / 3.14159)));
        }
    }
    
    syncFieldControlsFromConfig();
    
    // Sincronizar controles numéricos
    if (dtSlider_) {
        dtSlider_->SetValue(static_cast<int>(config.timeStep * 1000));
    }
    if (durationCtrl_) {
        durationCtrl_->SetValue(static_cast<int>(config.maxTime));
    }
}

void RightPanel::syncFieldControlsFromConfig()
{
    if (!fieldTypeChoice_ || !fieldIntensitySlider_ || !mainWindow_) {
        return;
    }

    auto& config = mainWindow_->getExperimentService().getConfig();
    fieldTypeChoice_->SetSelection(static_cast<int>(config.fieldView.type));
    fieldIntensitySlider_->SetValue(std::clamp(static_cast<int>(config.fieldView.intensity * 10.0), 0, 100));
    if (fieldIntensityValue_) {
        fieldIntensityValue_->SetLabel(wxString::Format(wxT("%.1f"), config.fieldView.intensity));
    }
    if (fieldCenterXCtrl_) {
        fieldCenterXCtrl_->SetValue(static_cast<int>(config.fieldView.centerX));
    }
    if (fieldCenterYCtrl_) {
        fieldCenterYCtrl_->SetValue(static_cast<int>(config.fieldView.centerY));
    }
}

void RightPanel::showBoatProperties()
{
    if (notebook_) {
        notebook_->SetSelection(2); // Pestaña Bote
    }
}

void RightPanel::showMotorProperties()
{
    if (notebook_) {
        notebook_->SetSelection(0); // Pestaña Motor
    }
}

void RightPanel::showFieldProperties()
{
    if (notebook_) {
        notebook_->SetSelection(1); // Pestaña Campo
    }
}

void RightPanel::showScenarioProperties()
{
    if (notebook_) {
        notebook_->SetSelection(4); // Pestaña Escenario
    }
}

// Getters para actualización en tiempo real durante simulación
double RightPanel::getThrustValue() const
{
    if (thrustSlider_) {
        return thrustSlider_->GetValue() / 10.0;
    }
    return 5.0;
}

double RightPanel::getDirectionValue() const
{
    if (directionSlider_) {
        return directionSlider_->GetValue() * 3.14159 / 180.0;
    }
    return 0.0;
}

int RightPanel::getFieldType() const
{
    if (fieldTypeChoice_) {
        return fieldTypeChoice_->GetSelection();
    }
    return 0;
}

double RightPanel::getFieldIntensity() const
{
    if (fieldIntensitySlider_) {
        return fieldIntensitySlider_->GetValue() / 10.0;
    }
    return 5.0;
}

int RightPanel::getMethod() const
{
    if (methodChoice_) {
        return methodChoice_->GetSelection();
    }
    return 0;
}

double RightPanel::getTimeStep() const
{
    if (dtSlider_) {
        return dtSlider_->GetValue() / 1000.0;
    }
    return 0.033;
}

void RightPanel::onBoatParamChange(wxSpinEvent& event)
{
    (void)event;
    updateBoatFromUI();
}

void RightPanel::onMotorParamChange(wxCommandEvent& event)
{
    (void)event;
    updateMotorFromUI();
}

void RightPanel::onFieldTypeChange(wxCommandEvent& event)
{
    (void)event;
    updateFieldFromUI();
}

void RightPanel::onFieldParamChange(wxCommandEvent& event)
{
    (void)event;
    updateFieldFromUI();
}

void RightPanel::onNumericalParamChange(wxCommandEvent& event)
{
    (void)event;
    updateNumericalFromUI();
}

void RightPanel::onDurationChange(wxSpinEvent& event)
{
    (void)event;
    updateNumericalFromUI();
}

void RightPanel::updateBoatFromUI()
{
    if (!mainWindow_) return;
    
    auto& config = mainWindow_->getExperimentService().getConfig();
    
    // Actualizar masa
    if (massCtrl_) {
        config.boat.setMass(massCtrl_->GetValue());
    }
    
    // Actualizar arrastre
    if (dragCtrl_) {
        config.boat.setDragCoefficient(dragCtrl_->GetValue() / 10.0);
    }
    
    // Actualizar tamaño
    if (sizeCtrl_) {
        config.boat.setSize(sizeCtrl_->GetValue());
    }
    
    // Actualizar posición inicial
    double x = initialXCtrl_ ? initialXCtrl_->GetValue() : 10.0;
    double y = initialYCtrl_ ? initialYCtrl_->GetValue() : 25.0;
    config.boat.setPosition(tp::shared::Vec2d(x, y));
    
    // Actualizar ángulo inicial
    if (initialAngleCtrl_) {
        double angle = initialAngleCtrl_->GetValue() * 3.14159 / 180.0;
        config.boat.setOrientation(angle);
    }
    
    refreshScenePreview(true);
}

void RightPanel::updateMotorFromUI()
{
    if (!mainWindow_) return;
    
    auto& config = mainWindow_->getExperimentService().getConfig();
    
    double thrust = thrustSlider_ ? thrustSlider_->GetValue() / 10.0 : 5.0;
    double direction = directionSlider_ ? directionSlider_->GetValue() * 3.14159 / 180.0 : 0.0;

    if (thrustValueText_) {
        thrustValueText_->SetLabel(wxString::Format(wxT("%.1f N"), thrust));
    }
    if (directionValueText_) {
        directionValueText_->SetLabel(wxString::Format(wxT("%d°"), static_cast<int>(direction * 180.0 / 3.14159)));
    }
    
    config.motor = tp::domain::Motor::constant(thrust, direction);
    config.boat.setOrientation(direction);

    refreshScenePreview(true);
}

void RightPanel::updateFieldFromUI()
{
    if (!mainWindow_) return;
    
    auto& config = mainWindow_->getExperimentService().getConfig();
    
    int fieldType = fieldTypeChoice_ ? fieldTypeChoice_->GetSelection() : 0;
    double intensity = fieldIntensitySlider_ ? fieldIntensitySlider_->GetValue() / 10.0 : 5.0;
    double centerX = fieldCenterXCtrl_ ? fieldCenterXCtrl_->GetValue() : 25.0;
    double centerY = fieldCenterYCtrl_ ? fieldCenterYCtrl_->GetValue() : 25.0;

    config.fieldView.type = static_cast<tp::application::FieldPresetType>(fieldType);
    config.fieldView.intensity = intensity;
    config.fieldView.centerX = centerX;
    config.fieldView.centerY = centerY;
    
        switch (fieldType) {
            case 0: // Uniforme
                config.field = tp::domain::VectorField::uniform(intensity, 0.0);
                break;
        case 1: // Lineal
            config.field = tp::domain::VectorField::linear(intensity / 25.0, 0.0, 0.0, 0.0);
            break;
        case 2: // Radial
            config.field = tp::domain::VectorField::radial(centerX, centerY, intensity);
            break;
            case 3: // Rotacional
                config.field = tp::domain::VectorField::rotational(centerX, centerY, intensity);
                break;
            case 4: // Personalizado
                break;
            default:
                break;
        }
    
    refreshScenePreview(true);
}

void RightPanel::updateNumericalFromUI()
{
    if (!mainWindow_) return;
    
    auto& config = mainWindow_->getExperimentService().getConfig();
    
    // Actualizar método
    if (methodChoice_) {
        int method = methodChoice_->GetSelection();
        switch (method) {
            case 0: config.method = tp::shared::IntegrationMethod::Euler; break;
            case 1: config.method = tp::shared::IntegrationMethod::ImprovedEuler; break;
            case 2: config.method = tp::shared::IntegrationMethod::RK4; break;
        }
    }
    
    // Actualizar paso temporal (convertir ms a segundos)
    if (dtSlider_) {
        config.timeStep = dtSlider_->GetValue() / 1000.0;
    }
    
    // Actualizar duración máxima
    if (durationCtrl_) {
        config.maxTime = durationCtrl_->GetValue();
    }
    
    refreshScenePreview(false);
}

void RightPanel::refreshScenePreview(bool syncProperties)
{
    if (!mainWindow_) return;

    mainWindow_->getExperimentService().notifyConfigChange();
    mainWindow_->updateCanvas();

    if (syncProperties) {
        updateProperties();
    }
}

//=============================================================================
// BOTTOM PANEL - Pestañas de resultados
//=============================================================================

wxBEGIN_EVENT_TABLE(BottomPanel, wxPanel)
    // Eventos
wxEND_EVENT_TABLE()

BottomPanel::BottomPanel(wxWindow* parent, MainWindow* mainWindow)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME)
    , mainWindow_(mainWindow)
    , notebook_(nullptr)
    , timeText_(nullptr)
    , speedText_(nullptr)
    , energyText_(nullptr)
    , distanceText_(nullptr)
    , collisionsText_(nullptr)
    , displacementText_(nullptr)
    , efficiencyText_(nullptr)
    , eventsList_(nullptr)
    , theoryTitle_(nullptr)
    , theoryContent_(nullptr)
    , theoryFormula_(nullptr)
    , comparisonText_(nullptr)
{
    SetBackgroundColour(ModernColors::PANEL_BG);
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Notebook para pestañas
    notebook_ = new wxNotebook(this, wxID_ANY);
    
    // Pestaña: Resultados
    wxPanel* resultsPanel = new wxPanel(notebook_);
    wxBoxSizer* resultsSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* resultsTitle = new wxStaticText(resultsPanel, wxID_ANY, wxT("Resumen de la simulación"));
    resultsTitle->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    resultsSizer->Add(resultsTitle, 0, wxLEFT | wxRIGHT | wxTOP, 12);

    wxStaticText* resultsIntro = new wxStaticText(
        resultsPanel,
        wxID_ANY,
        wxT("Estos indicadores se actualizan durante la simulación y resumen el desempeño del bote respecto al escenario y al método numérico seleccionado."));
    resultsIntro->Wrap(900);
    resultsSizer->Add(resultsIntro, 0, wxLEFT | wxRIGHT | wxBOTTOM, 12);

    wxFlexGridSizer* statsGrid = new wxFlexGridSizer(0, 3, 12, 12);
    statsGrid->AddGrowableCol(0, 1);
    statsGrid->AddGrowableCol(1, 1);
    statsGrid->AddGrowableCol(2, 1);

    auto createStatCard = [&](const wxString& title, const wxString& initialValue, wxColour accent, wxStaticText** valueOut) {
        wxPanel* card = new wxPanel(resultsPanel);
        card->SetBackgroundColour(wxColour(248, 250, 253));
        card->SetMinSize(wxSize(210, 92));

        wxBoxSizer* cardSizer = new wxBoxSizer(wxVERTICAL);

        wxPanel* accentBar = new wxPanel(card);
        accentBar->SetBackgroundColour(accent);
        accentBar->SetMinSize(wxSize(-1, 4));
        cardSizer->Add(accentBar, 0, wxEXPAND);

        wxStaticText* label = new wxStaticText(card, wxID_ANY, title);
        label->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        label->SetForegroundColour(wxColour(90, 100, 115));
        cardSizer->Add(label, 0, wxLEFT | wxRIGHT | wxTOP, 10);

        wxStaticText* value = new wxStaticText(card, wxID_ANY, initialValue);
        value->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        value->SetForegroundColour(wxColour(25, 35, 50));
        cardSizer->Add(value, 1, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 10);

        card->SetSizer(cardSizer);
        *valueOut = value;
        return card;
    };

    statsGrid->Add(createStatCard(wxT("Tiempo total"), wxT("0.00 s"), wxColour(66, 133, 244), &timeText_), 1, wxEXPAND);
    statsGrid->Add(createStatCard(wxT("Velocidad actual"), wxT("0.00 m/s"), wxColour(15, 157, 88), &speedText_), 1, wxEXPAND);
    statsGrid->Add(createStatCard(wxT("Energía cinética"), wxT("0.00 J"), wxColour(244, 160, 0), &energyText_), 1, wxEXPAND);
    statsGrid->Add(createStatCard(wxT("Distancia recorrida"), wxT("0.00 m"), wxColour(171, 71, 188), &distanceText_), 1, wxEXPAND);
    statsGrid->Add(createStatCard(wxT("Desplazamiento neto"), wxT("0.00 m"), wxColour(0, 172, 193), &displacementText_), 1, wxEXPAND);
    statsGrid->Add(createStatCard(wxT("Eficiencia de ruta"), wxT("100.0 %"), wxColour(239, 83, 80), &efficiencyText_), 1, wxEXPAND);

    wxPanel* extraRow = new wxPanel(resultsPanel);
    extraRow->SetBackgroundColour(wxColour(248, 250, 253));
    wxBoxSizer* extraSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* collisionLabel = new wxStaticText(extraRow, wxID_ANY, wxT("Colisiones detectadas"));
    collisionLabel->SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    collisionLabel->SetForegroundColour(wxColour(90, 100, 115));
    collisionsText_ = new wxStaticText(extraRow, wxID_ANY, wxT("0"));
    collisionsText_->SetFont(wxFont(13, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    collisionsText_->SetForegroundColour(wxColour(25, 35, 50));
    extraSizer->Add(collisionLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 10);
    extraSizer->AddStretchSpacer();
    extraSizer->Add(collisionsText_, 0, wxALL | wxALIGN_CENTER_VERTICAL, 10);
    extraRow->SetSizer(extraSizer);

    resultsSizer->Add(statsGrid, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);
    resultsSizer->Add(extraRow, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 12);
    resultsSizer->AddStretchSpacer();
    resultsPanel->SetSizer(resultsSizer);
    notebook_->AddPage(resultsPanel, wxT("Resultados"));
    
    // Pestaña: Gráficas
    wxPanel* graphsPanel = new wxPanel(notebook_);
    wxBoxSizer* graphsSizer = new wxBoxSizer(wxHORIZONTAL);
    
    speedGraph_ = new SimpleGraph(graphsPanel, wxT("Velocidad vs Tiempo"));
    speedGraph_->setColor(wxColour(0, 100, 200));
    graphsSizer->Add(speedGraph_, 1, wxEXPAND | wxALL, 5);
    
    energyGraph_ = new SimpleGraph(graphsPanel, wxT("Energía vs Tiempo"));
    energyGraph_->setColor(wxColour(200, 100, 0));
    graphsSizer->Add(energyGraph_, 1, wxEXPAND | wxALL, 5);
    
    positionGraph_ = new SimpleGraph(graphsPanel, wxT("Posición X vs Tiempo"));
    positionGraph_->setColor(wxColour(0, 150, 0));
    graphsSizer->Add(positionGraph_, 1, wxEXPAND | wxALL, 5);
    
    graphsPanel->SetSizer(graphsSizer);
    notebook_->AddPage(graphsPanel, wxT("Gráficas"));
    
    // Pestaña: Eventos
    wxPanel* eventsPanel = new wxPanel(notebook_);
    wxBoxSizer* eventsSizer = new wxBoxSizer(wxVERTICAL);
    eventsList_ = new wxListBox(eventsPanel, wxID_ANY);
    eventsSizer->Add(eventsList_, 1, wxEXPAND | wxALL, 5);
    wxButton* clearEventsBtn = new wxButton(eventsPanel, wxID_ANY, wxT("Limpiar"));
    eventsSizer->Add(clearEventsBtn, 0, wxALIGN_RIGHT | wxALL, 5);
    eventsPanel->SetSizer(eventsSizer);
    notebook_->AddPage(eventsPanel, wxT("Eventos"));
    
    mainSizer->Add(notebook_, 1, wxEXPAND | wxALL, 5);
    SetSizer(mainSizer);
}

void BottomPanel::updateResults()
{
    if (!mainWindow_) return;
    
    // Obtener datos reales de mainWindow
    double time = mainWindow_->getCurrentTime();
    double speed = mainWindow_->getCurrentSpeed();
    double energy = mainWindow_->getCurrentEnergy();
    double distance = mainWindow_->getTotalDistance();
    double displacement = mainWindow_->getNetDisplacement();
    double efficiency = mainWindow_->getRouteEfficiency();
    
    // Actualizar texto de resultados
    if (timeText_) {
        wxString txt;
        txt.Printf(wxT("%.2f s"), time);
        timeText_->SetLabel(txt);
    }
    if (speedText_) {
        wxString txt;
        txt.Printf(wxT("%.2f m/s"), speed);
        speedText_->SetLabel(txt);
    }
    if (energyText_) {
        wxString txt;
        txt.Printf(wxT("%.2f J"), energy);
        energyText_->SetLabel(txt);
    }
    if (distanceText_) {
        wxString txt;
        txt.Printf(wxT("%.2f m"), distance);
        distanceText_->SetLabel(txt);
    }
    if (collisionsText_) {
        collisionsText_->SetLabel(wxString::Format(wxT("%d"), 0));
    }
    if (displacementText_) {
        wxString txt;
        txt.Printf(wxT("%.2f m"), displacement);
        displacementText_->SetLabel(txt);
    }
    if (efficiencyText_) {
        wxString txt;
        txt.Printf(wxT("%.1f %%"), efficiency * 100);
        efficiencyText_->SetLabel(txt);
    }
    
    // Actualizar gráficas si hay datos
    const auto& trajectory = mainWindow_->getTrajectoryPoints();
    if (!trajectory.empty()) {
        const auto& lastPoint = trajectory.back();
        
        if (speedGraph_) {
            speedGraph_->addDataPoint(time, speed);
        }
        if (energyGraph_) {
            energyGraph_->addDataPoint(time, energy);
        }
        if (positionGraph_) {
            positionGraph_->addDataPoint(time, lastPoint.x);
        }
    }
}

void BottomPanel::addEvent(const wxString& message)
{
    if (eventsList_) {
        wxDateTime now = wxDateTime::Now();
        wxString timestamp = now.Format(wxT("%H:%M:%S"));
        eventsList_->Append(timestamp + wxT(" - ") + message);
        // Auto-scroll al último evento
        eventsList_->SetSelection(eventsList_->GetCount() - 1);
    }
}

void BottomPanel::clearGraphs()
{
    if (speedGraph_) {
        speedGraph_->clearData();
    }
    if (energyGraph_) {
        energyGraph_->clearData();
    }
    if (positionGraph_) {
        positionGraph_->clearData();
    }
}

void BottomPanel::clearEvents()
{
    if (eventsList_) {
        eventsList_->Clear();
    }
}

void BottomPanel::setActiveTab(int tabIndex)
{
    if (notebook_ && tabIndex >= 0 && tabIndex < (int)notebook_->GetPageCount()) {
        notebook_->SetSelection(tabIndex);
    }
}

//=============================================================================
// MAIN WINDOW - Métodos implementados después de RightPanel
//=============================================================================

double MainWindow::getTimeStep() const
{
    /**
     * @brief Obtiene el paso temporal (dt) configurado por el usuario
     * @return Paso temporal en segundos (default: 0.033s = 33ms)
     *
     * Este valor proviene del slider de configuración numérica en el panel derecho.
     * Permite ajustar la velocidad de la simulación en tiempo real.
     */
    if (rightPanel_) {
        return rightPanel_->getTimeStep();
    }
    return 0.033; // Valor por defecto si no hay panel derecho
}

} // namespace tp::presentation
