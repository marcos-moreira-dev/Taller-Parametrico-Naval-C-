#pragma once

/**
 * @file EditorPanel.hpp
 * @brief Panel de edición de escenarios
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Proporciona la interfaz de usuario para:
 * - Seleccionar herramientas de edición
 * - Configurar tamaño de brush y snap
 * - Crear/redimensionar escenarios
 * - Definir el campo vectorial
 * - Deshacer/rehacer acciones (undo/redo)
 * 
 * @note Usa CommandHistory para implementar undo/redo
 * @see SimulationCanvas para el renderizado del editor
 * @see EditorCommand para el patrón Command
 */

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "panels/EditorCommand.hpp"
#include "main_window/MainWindow.hpp"

namespace tp::presentation {

/**
 * @brief Herramientas disponibles en el editor
 * 
 * Cada herramienta define el comportamiento al interactuar
 * con el canvas en modo editor.
 */
enum class EditorTool {
    Select,          ///< Seleccionar celdas
    PaintWater,       ///< Pintar agua
    PaintLand,        ///< Pintar tierra
    PlaceObstacle,    ///< Colocar obstáculo
    Erase,            ///< Borrar (poner agua)
    Fill,             ///< Llenar área
    PlaceBoat,       ///< Posicionar bateau
    DefineField      ///< Definir campo vectorial
};

/**
 * @brief Panel de control del editor de escenarios
 * 
 * Organiza los controles en cuatro secciones:
 * 1. Herramientas: selección de herramienta de edición
 * 2. Brush: tamaño del pincel y snap a grille
 * 3. Escenario: crear/redimensionar escenario
 * 4. Campo: tipo y parámetros del campo vectorial
 * 
 * Implementa undo/redo mediante CommandHistory.
 */
class EditorPanel : public wxPanel {
public:
    /**
     * @brief Constructor
     * @param parent Ventana padre (RightPanel de MainWindow)
     * @param mainWindow Referencia a MainWindow para acceder al escenario
     * 
     * Inicializa con herramienta por defecto: Select
     */
    EditorPanel(wxWindow* parent, MainWindow* mainWindow);
    
    /**
     * @brief Obtiene la herramienta seleccionada
     * @return EditorTool actual
     */
    EditorTool getCurrentTool() const { return currentTool_; }
    
    /**
     * @brief Selecciona una herramienta
     * @param tool Nueva herramienta
     * 
     * Actualiza el estado visual de los botones.
     */
    void setCurrentTool(EditorTool tool);
    
    /**
     * @brief Obtiene el tamaño del brush
     * @return Radio del brush [células]
     */
    int getBrushSize() const;
    
    /**
     * @brief Verifica si snap a grille está activo
     * @return true si está activo
     */
    bool isSnapToGrid() const;
    
private:
    /**
     * @brief Configura la interfaz de usuario
     * 
     * Crea el layout principal con pestañas para:
     * - Herramientas
     * - Escenario
     * - Campo
     */
    void setupUI();
    
    /**
     * @brief Crea la sección de herramientas
     * 
     * Botones para cada EditorTool con actualización visual.
     */
    void createToolsSection();
    
    /**
     * @brief Crea la sección de brush
     * 
     * Slider para tamaño y checkbox para snap.
     */
    void createBrushSection();
    
    /**
     * @brief Crea la sección de escenario
     * 
     * Controles para dimensiones y nombre del escenario.
     */
    void createScenarioSection();
    
    /**
     * @brief Crea la sección de campo vectorial
     * 
     * Choice para tipo de campo y sliders para parámetros.
     */
    void createFieldSection();
    
    // ============================================================
    // EVENT HANDLERS
    // ============================================================
    
    void onToolSelect(wxCommandEvent& event);
    void onBrushSizeChange(wxCommandEvent& event);
    void onSnapToggle(wxCommandEvent& event);
    void onNewScenario(wxCommandEvent& event);
    void onResizeScenario(wxCommandEvent& event);
    void onClearScenario(wxCommandEvent& event);
    void onFillWater(wxCommandEvent& event);
    void onFillLand(wxCommandEvent& event);
    void onUndo(wxCommandEvent& event);
    void onRedo(wxCommandEvent& event);
    void onFieldTypeChange(wxCommandEvent& event);
    void onFieldParamChange(wxCommandEvent& event);
    void onSpinCtrlChange(wxSpinEvent& event);
    
    MainWindow* mainWindow_;        ///< Referencia a MainWindow
    EditorTool currentTool_;         ///< Herramienta seleccionada
    
    // Controles de herramientas
    wxToolBar* toolsToolbar_;
    wxButton* btnSelect_;
    wxButton* btnWater_;
    wxButton* btnLand_;
    wxButton* btnObstacle_;
    wxButton* btnErase_;
    wxButton* btnBoat_;
    
    // Controles de brush
    wxSlider* brushSizeSlider_;
    wxCheckBox* snapCheck_;
    
    // Controles de escenario
    wxSpinCtrl* widthCtrl_;
    wxSpinCtrl* heightCtrl_;
    wxTextCtrl* nameCtrl_;
    
    // Controles de campo
    wxChoice* fieldTypeChoice_;
    wxSlider* fieldIntensitySlider_;
    wxSpinCtrl* fieldCenterXCtrl_;
    wxSpinCtrl* fieldCenterYCtrl_;
    
    /**
     * @brief Historial de comandos para undo/redo
     * 
     * Almacena hasta 50 comandos ejecutados.
     * Permite deshacer y rehacer operaciones de edición.
     */
    CommandHistory commandHistory_;
    
    wxDECLARE_EVENT_TABLE();
};

} // namespace tp::presentation
