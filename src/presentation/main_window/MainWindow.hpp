#pragma once

/**
 * @file MainWindow.hpp
 * @brief Ventana principal de la aplicación Taller Paramétrico
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
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
#include <vector>
#include "ExperimentService.hpp"
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

// Estados de la aplicación
enum class AppMode {
    Theory,      // Modo teoría educativa
    Simulation,  // Modo simulación
    Editor       // Modo editor de escenarios
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
    
    // Acceso al servicio de experimentos
    tp::application::ExperimentService& getExperimentService() { return experimentService_; }
    
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
    
private:
    // Funciones auxiliares refactorizadas
    bool confirmSaveChanges(const wxString& action);
    void stripExtension(wxString& filename, const wxString& ext);
    void setupMenu();
    void setupToolbar();
    void setupAuiLayout();
    void setupStatusBar();
    
    // Crear paneles
    wxPanel* createWelcomePanel();
    void createTheoryPanel();
    void createSimulationPanel();
    void createEditorPanel();
    
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
    
    application::ExperimentService experimentService_;
    
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
    
    // Trayectoria acumulada
    std::vector<TrajectoryPoint> trajectoryPoints_;
    
    // Estado del documento
    bool isModified_;
    wxString currentScenarioName_;
    
    // IDs para paneles AUI
    static const int ID_LEFT_PANEL = 1000;
    static const int ID_RIGHT_PANEL = 1001;
    static const int ID_BOTTOM_PANEL = 1002;
    static const int ID_TOGGLE_3D = 1003;
    
    wxDECLARE_EVENT_TABLE();
};

// Panel izquierdo: Navegación y escenarios
class LeftPanel : public wxPanel {
public:
    LeftPanel(wxWindow* parent, MainWindow* mainWindow);
    
    void updateScenarioList();
    int getSelectedScenarioIndex() const;
    void applyLayerVisibility();
    
private:
    void createNavigationSection();
    void createScenariosSection();
    void createLayersSection();
    
    void onModeSelect(wxCommandEvent& event);
    void onScenarioSelect(wxCommandEvent& event);
    void onLayerToggle(wxCommandEvent& event);
    
    MainWindow* mainWindow_;
    
    wxNotebook* notebook_;
    wxListBox* scenarioList_;
    wxCheckListBox* layerList_;
    wxRadioBox* modeSelector_;
    
    wxDECLARE_EVENT_TABLE();
};

// Panel derecho: Inspector de propiedades
class RightPanel : public wxPanel {
public:
    RightPanel(wxWindow* parent, MainWindow* mainWindow);
    
    void updateProperties();
    void showBoatProperties();
    void showMotorProperties();
    void showFieldProperties();
    void showScenarioProperties();
    
    // Obtener valores actuales para actualización en tiempo real
    double getThrustValue() const;
    double getDirectionValue() const;
    int getFieldType() const;
    double getFieldIntensity() const;
    int getMethod() const;
    double getTimeStep() const;
    bool isAutoThrustEnabled() const { return autoThrustCheck_ ? autoThrustCheck_->GetValue() : false; }
    
private:
    void createBoatSection();
    void createMotorSection();
    void createFieldSection();
    void createNumericalSection();
    
    void onBoatParamChange(wxSpinEvent& event);
    void onMotorParamChange(wxCommandEvent& event);
    void onFieldTypeChange(wxCommandEvent& event);
    void onFieldParamChange(wxCommandEvent& event);
    void onNumericalParamChange(wxCommandEvent& event);
    void onDurationChange(wxSpinEvent& event);
    
    // Actualizar configuración desde UI
    void updateBoatFromUI();
    void updateMotorFromUI();
    void updateFieldFromUI();
    void updateNumericalFromUI();
    void syncFieldControlsFromConfig();
    void refreshScenePreview(bool syncProperties = false);
    
    MainWindow* mainWindow_;
    wxNotebook* notebook_;
    
    // Controles del bote
    wxSpinCtrl* massCtrl_;
    wxSpinCtrl* dragCtrl_;
    wxSpinCtrl* sizeCtrl_;
    wxSpinCtrl* initialXCtrl_;
    wxSpinCtrl* initialYCtrl_;
    wxSpinCtrl* initialAngleCtrl_;
    
    // Controles del motor
    wxSlider* thrustSlider_;
    wxSlider* directionSlider_;
    wxStaticText* thrustValueText_;
    wxStaticText* directionValueText_;
    wxCheckBox* autoThrustCheck_;
    
    // Controles del campo
    wxChoice* fieldTypeChoice_;
    wxSlider* fieldIntensitySlider_;
    wxStaticText* fieldIntensityValue_;
    wxSpinCtrl* fieldCenterXCtrl_;
    wxSpinCtrl* fieldCenterYCtrl_;
    
    // Controles numéricos
    wxChoice* methodChoice_;
    wxSlider* dtSlider_;
    wxSpinCtrl* durationCtrl_;
    
    wxDECLARE_EVENT_TABLE();
};

// Panel inferior: Pestañas de resultados
class BottomPanel : public wxPanel {
public:
    BottomPanel(wxWindow* parent, MainWindow* mainWindow);
    
    void updateResults();
    void addEvent(const wxString& message);
    void clearEvents();
    void clearGraphs();
    void setActiveTab(int tabIndex);
    
private:
    void createResultsTab();
    void createGraphsTab();
    void createEventsTab();
    void createTheoryTab();
    void createComparisonTab();
    
    MainWindow* mainWindow_;
    
    wxNotebook* notebook_;
    
    // Tab Resultados
    wxStaticText* timeText_;
    wxStaticText* speedText_;
    wxStaticText* energyText_;
    wxStaticText* distanceText_;
    wxStaticText* collisionsText_;
    wxStaticText* displacementText_;
    wxStaticText* efficiencyText_;
    
    // Tab Gráficas
    SimpleGraph* speedGraph_;
    SimpleGraph* energyGraph_;
    SimpleGraph* positionGraph_;
    
    // Tab Eventos
    wxListBox* eventsList_;
    
    // Tab Teoría
    wxStaticText* theoryTitle_;
    wxStaticText* theoryContent_;
    wxStaticText* theoryFormula_;
    
    // Tab Comparación
    wxStaticText* comparisonText_;
    
    wxDECLARE_EVENT_TABLE();
};

}
