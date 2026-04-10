#include "main_window/MainWindow.hpp"

#include "main_window/MainWindowIds.hpp"
#include "main_window/MainWindowStyles.hpp"
#include "panels/BottomPanel.hpp"
#include "panels/LeftPanel.hpp"
#include "panels/RightPanel.hpp"
#include "panels/TheoryPanel.hpp"
#include "panels/EditorPanel.hpp"
#include "panels/FieldEditorPanel.hpp"
#include "widgets/SimulationCanvas.hpp"
#include "widgets/Water3DCanvas.hpp"
#include "widgets/SimpleGraph.hpp"

#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>

#include <fstream>
#include <iomanip>

#include "Logger.hpp"
#include "TraceLogger.hpp"
#include "ExpressionParser.hpp"
#include "Scenario.hpp"
#include "VectorField.hpp"
#include "MathExpression.hpp"
#include "Boat.hpp"
#include "Motor.hpp"
#include "integrators/Integrators.hpp"
#include "models/PhysicsModel.hpp"
#include "runner/SimulationRunner.hpp"

namespace tp::presentation {

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
    EVT_MENU(ID_MENU_MODE_FIELD_EDITOR, MainWindow::onModeFieldEditor)
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
    EVT_AUI_PANE_CLOSE(MainWindow::onAuiPaneClose)
    EVT_CLOSE(MainWindow::OnClose)
wxEND_EVENT_TABLE()

MainWindow::MainWindow(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1600, 1000),
              wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
    , scenarioDocument_(experimentService_)
    , leftPanel_(nullptr)
    , rightPanel_(nullptr)
    , bottomPanel_(nullptr)
    , canvas_(nullptr)
    , canvas3D_(nullptr)
    , centerPanel_(nullptr)
    , theoryPanel_(nullptr)
    , simulationPanel_(nullptr)
    , editorPanel_(nullptr)
    , fieldEditorPanel_(nullptr)
    , is3DMode_(false)
    , currentMode_(AppMode::Simulation)
    , isRunning_(false)
    , isPaused_(false)
    , timer_(nullptr)
    , currentTime_(0.0)
    , currentSpeed_(0.0)
    , currentEnergy_(0.0)
    , totalDistance_(0.0)
    , netDisplacement_(0.0)
    , routeEfficiency_(0.0)
    , initialPosition_(tp::shared::Vec2d(0.0, 0.0))
    , simulationStartPosition_(tp::shared::Vec2d(0.0, 0.0))
    , simulationStartVelocity_(tp::shared::Vec2d(0.0, 0.0))
    , simulationStartOrientation_(0.0)
    , isModified_(false)
    , currentScenarioName_(wxT("Nuevo Escenario"))
    , currentScenarioPath_(wxEmptyString)
    , simulationStatusText_(wxT("Listo para simular"))
{
    // IMPORTANTE: Primero configurar el frame básico
    SetBackgroundColour(ModernColors::PRIMARY_BG);

#ifdef __WXMSW__
    wxIcon windowIcon(wxT("IDI_ICON1"), wxBITMAP_TYPE_ICO_RESOURCE);
    if (windowIcon.IsOk()) {
        SetIcon(windowIcon);
    }
#endif

    LOG_INFO_MACRO("MainWindow inicializada.");
    
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
    currentScenarioName_ = wxString::FromUTF8(experimentService_.getConfig().scenario.getName().c_str());
    simulationStartPosition_ = experimentService_.getConfig().boat.getPosition();
    simulationStartVelocity_ = experimentService_.getConfig().boat.getVelocity();
    simulationStartOrientation_ = experimentService_.getConfig().boat.getOrientation();
    
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
    viewMenu->AppendSeparator();
    viewMenu->Append(ID_MENU_RESET_VIEW, wxT("Reiniciar &Vista"), wxT("Restaurar todos los paneles visibles"));
    menuBar->Append(viewMenu, wxT("&Ver"));
    
    // Menú Modo
    wxMenu* modeMenu = new wxMenu();
    modeMenu->AppendRadioItem(ID_MENU_MODE_THEORY, wxT("&Teoría\tF1"), wxT("Modo teoría educativa"));
    modeMenu->AppendRadioItem(ID_MENU_MODE_SIMULATION, wxT("&Simulación\tF2"), wxT("Modo simulación"));
    modeMenu->AppendRadioItem(ID_MENU_MODE_EDITOR, wxT("Editor de &escenario\tF3"), wxT("Modo editor de escenarios"));
    modeMenu->AppendRadioItem(ID_MENU_MODE_FIELD_EDITOR, wxT("Editor de &campo\tF7"), wxT("Modo editor de campos"));
    modeMenu->Check(ID_MENU_MODE_SIMULATION, true);
    menuBar->Append(modeMenu, wxT("&Modo"));
    
    // Menú Simulación
    wxMenu* simMenu = new wxMenu();
    simMenu->Append(ID_MENU_START, wxT("&Iniciar\tSpace"), wxT("Iniciar simulación"));
    simMenu->Append(ID_MENU_PAUSE, wxT("&Pausar\tP"), wxT("Pausar/reanudar simulación"));
    simMenu->Append(ID_MENU_RESET, wxT("&Reiniciar\tCtrl+R"), wxT("Reiniciar simulación"));
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
    toolbar->AddTool(ID_MENU_MODE_EDITOR, wxT("Mapa"),
                     wxArtProvider::GetBitmap(wxART_LIST_VIEW, wxART_TOOLBAR, wxSize(28, 28)),
                     wxT("Editor de escenarios (F3)"));
    toolbar->AddTool(ID_MENU_MODE_FIELD_EDITOR, wxT("Campo"),
                     wxArtProvider::GetBitmap(wxART_REPORT_VIEW, wxART_TOOLBAR, wxSize(28, 28)),
                     wxT("Editor de campos (F7)"));
    
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
                     wxT("Reiniciar simulación (Ctrl+R)"));
    
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
        if (editorPanel_) {
            editorPanel_->syncFromDocument();
        }
        if (fieldEditorPanel_) {
            fieldEditorPanel_->syncFromDocument();
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
              .Caption(wxT("Editor de escenario"))
              .Right()
              .Layer(1)
              .Position(1)
              .BestSize(430, 800)
              .MinSize(360, 500)
              .Hide();
    auiManager_.AddPane(editorPanel_, editorInfo);

    fieldEditorPanel_ = new FieldEditorPanel(this, this);
    wxAuiPaneInfo fieldEditorInfo;
    fieldEditorInfo.Name(wxT("fieldEditorPanel"))
                  .Caption(wxT("Editor de campo"))
                  .Right()
                  .Layer(1)
                  .Position(1)
                  .BestSize(430, 800)
                  .MinSize(360, 500)
                  .Hide();
    auiManager_.AddPane(fieldEditorPanel_, fieldEditorInfo);
    
    // Aplicar layout
    auiManager_.Update();
}

void MainWindow::setupStatusBar()
{
    CreateStatusBar(5);
    int widths[] = {250, 180, 180, 150, 120};
    SetStatusWidths(5, widths);

    refreshStatusBar();
}

void MainWindow::refreshStatusBar()
{
    wxString primary;
    switch (currentMode_) {
        case AppMode::Theory:
            primary = wxT("Teoría educativa");
            break;
        case AppMode::Editor:
            primary = wxT("Editor de escenario activo");
            break;
        case AppMode::FieldEditor:
            primary = wxT("Editor de campo activo");
            break;
        case AppMode::Simulation:
        default:
            primary = simulationStatusText_.empty() ? wxT("Listo para simular") : simulationStatusText_;
            break;
    }

    wxString modeText;
    switch (currentMode_) {
        case AppMode::Theory: modeText = wxT("Modo: Teoría"); break;
        case AppMode::Simulation: modeText = wxT("Modo: Simulación"); break;
        case AppMode::Editor: modeText = wxT("Modo: Editor de escenario"); break;
        case AppMode::FieldEditor: modeText = wxT("Modo: Editor de campo"); break;
    }

    SetStatusText(primary, 0);
    SetStatusText(modeText, 1);

    if (currentMode_ == AppMode::Theory) {
        SetStatusText(wxEmptyString, 2);
        SetStatusText(wxEmptyString, 3);
        SetStatusText(wxEmptyString, 4);
        return;
    }

    const auto& boat = experimentService_.getConfig().boat;
    const auto pos = boat.getPosition();
    SetStatusText(wxString::Format(wxT("Coords: (%.1f, %.1f)"), pos.x, pos.y), 2);

    if (currentMode_ == AppMode::Simulation) {
        SetStatusText(wxString::Format(wxT("t: %.2f s | v: %.2f m/s"), currentTime_, currentSpeed_), 3);
        wxString methodText = wxT("Método: Euler");
        switch (experimentService_.getConfig().method) {
            case tp::shared::IntegrationMethod::ImprovedEuler: methodText = wxT("Método: Heun"); break;
            case tp::shared::IntegrationMethod::RK4: methodText = wxT("Método: RK4"); break;
            case tp::shared::IntegrationMethod::Euler:
            default: break;
        }
        SetStatusText(methodText, 4);
    } else {
        SetStatusText(wxEmptyString, 3);
        SetStatusText(wxEmptyString, 4);
    }
}

void MainWindow::syncViewMenuChecks()
{
    wxMenuBar* menuBar = GetMenuBar();
    if (!menuBar) {
        return;
    }

    menuBar->Check(ID_MENU_VIEW_LEFT, auiManager_.GetPane(leftPanel_).IsOk() && auiManager_.GetPane(leftPanel_).IsShown());
    menuBar->Check(ID_MENU_VIEW_RIGHT, auiManager_.GetPane(rightPanel_).IsOk() && auiManager_.GetPane(rightPanel_).IsShown());
    menuBar->Check(ID_MENU_VIEW_BOTTOM, auiManager_.GetPane(bottomPanel_).IsOk() && auiManager_.GetPane(bottomPanel_).IsShown());
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
        menuBar->Check(ID_MENU_MODE_FIELD_EDITOR, mode == AppMode::FieldEditor);
    }
    
    // Cambiar panel central visible
    if (auiManager_.GetPane(canvas_).IsOk()) {
        auiManager_.GetPane(canvas_).Show(mode == AppMode::Simulation || mode == AppMode::Editor || mode == AppMode::FieldEditor);
    }
    if (auiManager_.GetPane(theoryPanel_).IsOk()) {
        auiManager_.GetPane(theoryPanel_).Show(mode == AppMode::Theory);
    }
    if (auiManager_.GetPane(editorPanel_).IsOk()) {
        auiManager_.GetPane(editorPanel_).Show(mode == AppMode::Editor);
    }
    if (auiManager_.GetPane(fieldEditorPanel_).IsOk()) {
        auiManager_.GetPane(fieldEditorPanel_).Show(mode == AppMode::FieldEditor);
    }
    if (auiManager_.GetPane(leftPanel_).IsOk()) {
        const bool showLeft = (mode != AppMode::Theory);
        auiManager_.GetPane(leftPanel_).Show(showLeft);
    }
    if (auiManager_.GetPane(rightPanel_).IsOk()) {
        const bool showRight = (mode == AppMode::Simulation);
        auiManager_.GetPane(rightPanel_).Show(showRight);
    }
    if (auiManager_.GetPane(bottomPanel_).IsOk()) {
        const bool showBottom = (mode == AppMode::Simulation);
        auiManager_.GetPane(bottomPanel_).Show(showBottom);
    }
    auiManager_.Update();
    syncViewMenuChecks();
    refreshStatusBar();
    
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
