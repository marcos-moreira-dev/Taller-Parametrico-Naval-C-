#pragma once

/**
 * @file MainWindow.hpp
 * @brief Ventana principal de la aplicación Taller Paramétrico
 * @author Proyecto Taller Paramétrico
 * @version 1.1.0
 * 
 * Punto central de la aplicación wxWidgets que:
 * - Gestiona el layout AUI (paneles flotantes)
 * - Coordina simulación y editor
 * - Maneja eventos de menú y toolbar
 * - Integración con todos los paneles (Left, Right, Bottom, Canvas)
 * 
 * @note Hereda de wxFrame para marco de ventana estándar
 * @see SimulationCanvas para visualización
 * @see ExperimentService para lógica de negocio
 */

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/notebook.h>
#include <wx/listbook.h>
#include <wx/spinctrl.h>
#include <wx/aui/framemanager.h>
#include <vector>
#include "ExperimentService.hpp"
#include "ScenarioDocument.hpp"
#include "widgets/SimulationCanvas.hpp"
#include "widgets/Water3DCanvas.hpp"

namespace tp::presentation {

class SimulationCanvas;
class Water3DCanvas;
class SimpleGraph;
class LeftPanel;
class RightPanel;
class BottomPanel;
class TheoryPanel;
class EditorPanel;
class FieldEditorPanel;

// Estados de la aplicación
enum class AppMode {
    Theory,      // Modo teoría educativa
    Simulation,  // Modo simulación
    Editor,      // Modo editor de escenarios
    FieldEditor  // Modo editor de campos
};

class MainWindow : public wxFrame {
public:
    MainWindow(const wxString& title);
    ~MainWindow();
    
    // Cambiar entre modos
    void setMode(AppMode mode);
    AppMode getCurrentMode() const { return currentMode_; }
    
    // Actualizar UI
    void updateResults();
    void updateCanvas();
    
    // Accesores para los paneles
    LeftPanel* getLeftPanel() const { return leftPanel_; }
    RightPanel* getRightPanel() const { return rightPanel_; }
    BottomPanel* getBottomPanel() const { return bottomPanel_; }
    SimulationCanvas* getCanvas() const { return canvas_; }
    Water3DCanvas* getCanvas3D() const { return canvas3D_; }
    void openCustomFieldDialog();
    void requestSimulationReset();
    bool requestSaveScenario();
    void requestLoadScenario();
    bool requestReloadCurrentScenarioFile();
    void requestLoadScenarioPreset(size_t index);
    
    // Acceso al servicio de experimentos
    tp::application::ExperimentService& getExperimentService() { return experimentService_; }
    tp::application::ScenarioDocument& getScenarioDocument() { return scenarioDocument_; }
    
    // Setters para el escenario actual
    void setScenarioName(const wxString& name) { currentScenarioName_ = name; }
    void setModified(bool modified) { markAsModified(modified); }
    
    // Getters para datos de simulación (para BottomPanel)
    double getCurrentTime() const { return currentTime_; }
    double getCurrentSpeed() const { return currentSpeed_; }
    double getCurrentEnergy() const { return currentEnergy_; }
    double getTotalDistance() const { return totalDistance_; }
    double getNetDisplacement() const { return netDisplacement_; }
    double getRouteEfficiency() const { return routeEfficiency_; }
    const std::vector<TrajectoryPoint>& getTrajectoryPoints() const { return trajectoryPoints_; }
    
    // Obtener paso temporal
    double getTimeStep() const;
    
    // Funciones del editor
    void updateEditorBrushSize(int size);
    void updateEditorSnap(bool snap);
    void setEditorMode(bool editor);
    void beginEditorStroke();
    void applyEditorToolAtCell(int x, int y);
    void endEditorStroke();
    
private:
    // Funciones auxiliares refactorizadas
    bool confirmSaveChanges(const wxString& action);
    void stripExtension(wxString& filename, const wxString& ext);
    void setupMenu();
    void setupToolbar();
    void setupAuiLayout();
    void setupStatusBar();
    void refreshStatusBar();
    void syncViewMenuChecks();
    bool saveScenarioWithDialog();
    
    
    // Eventos de menú y toolbar
    void onNew(wxCommandEvent& event);
    void onOpen(wxCommandEvent& event);
    void onSave(wxCommandEvent& event);
    void onExportImage(wxCommandEvent& event);
    void onExportData(wxCommandEvent& event);
    void onExit(wxCommandEvent& event);
    
    void onViewLeftPanel(wxCommandEvent& event);
    void onViewRightPanel(wxCommandEvent& event);
    void onViewBottomPanel(wxCommandEvent& event);
    void onViewGrid(wxCommandEvent& event);
    void onViewField(wxCommandEvent& event);
    void onViewTrajectory(wxCommandEvent& event);
    
    void onModeTheory(wxCommandEvent& event);
    void onModeSimulation(wxCommandEvent& event);
    void onModeEditor(wxCommandEvent& event);
    void onModeFieldEditor(wxCommandEvent& event);
    
    // Callback desde TheoryPanel para cargar escenario
    void onTheoryScenarioRequest(const wxString& scenarioName);
    
    void onStartSimulation(wxCommandEvent& event);
    void onPauseSimulation(wxCommandEvent& event);
    void onResetSimulation(wxCommandEvent& event);
    void onStepSimulation(wxCommandEvent& event);
    void onMethodChange(wxCommandEvent& event);
    void onDtChange(wxCommandEvent& event);
    
    void onAbout(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);
    
    // Herramientas
    void onResetView(wxCommandEvent& event);
    void onClearTrajectory(wxCommandEvent& event);
    void onCaptureScreen(wxCommandEvent& event);
    void onCustomField(wxCommandEvent& event);
    void onToggle3D(wxCommandEvent& event);
    void onAuiPaneClose(wxAuiManagerEvent& event);
    void OnClose(wxCloseEvent& event);
    
    // Actualizar UI
    void updateWindowTitle();
    void markAsModified(bool modified = true);
    
    // Timer para animación
    void onTimer(wxTimerEvent& event);
    void runSimulationStep();
    void resetSimulation();
    
    // Layout AUI
    wxAuiManager auiManager_;
    
    // Paneles principales
    LeftPanel* leftPanel_;
    RightPanel* rightPanel_;
    BottomPanel* bottomPanel_;
    SimulationCanvas* canvas_;
    Water3DCanvas* canvas3D_;
    bool is3DMode_;
    
    // Panel central (cambia según el modo)
    wxPanel* centerPanel_;
    TheoryPanel* theoryPanel_;
    wxPanel* simulationPanel_;
    EditorPanel* editorPanel_;
    FieldEditorPanel* fieldEditorPanel_;
    
    application::ExperimentService experimentService_;
    application::ScenarioDocument scenarioDocument_;
    
    // Estado
    AppMode currentMode_;
    
    // Timer para animación
    wxTimer* timer_;
    bool isRunning_;
    bool isPaused_;
    
    // Datos de simulación en curso
    double currentTime_;
    double currentSpeed_;
    double currentEnergy_;
    double totalDistance_;
    double netDisplacement_;
    double routeEfficiency_;
    tp::shared::Vec2d initialPosition_;
    tp::shared::Vec2d simulationStartPosition_;
    tp::shared::Vec2d simulationStartVelocity_;
    double simulationStartOrientation_;
    
    // Trayectoria acumulada
    std::vector<TrajectoryPoint> trajectoryPoints_;
    
    // Estado del documento
    bool isModified_;
    wxString currentScenarioName_;
    wxString currentScenarioPath_;
    wxString simulationStatusText_;
    
    // IDs para paneles AUI
    static const int ID_LEFT_PANEL = 1000;
    static const int ID_RIGHT_PANEL = 1001;
    static const int ID_BOTTOM_PANEL = 1002;
    static const int ID_TOGGLE_3D = 1003;
    
    wxDECLARE_EVENT_TABLE();
};

} // namespace tp::presentation
