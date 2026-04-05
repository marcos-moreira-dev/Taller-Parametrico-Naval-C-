#pragma once

/**
 * @file SimulationCanvas.hpp
 * @brief Canvas principal de visualización y edición
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Widget wxWidgets que actúa como canvas principal de la aplicación.
 * Proporciona:
 * - Renderizado del escenario (células de agua/tierra/obstáculos)
 * - Visualización del campo vectorial (flechas)
 * - Animación del bateau y su trayectoria
 * - Sistema de partículas (estela, espuma, salpicaduras)
 * - Mode de edición de celdas
 * 
 * @note Hereda de wxPanel para manejo de eventos y renderizado
 * @see MainWindow para integración
 * @see WaterParticleSystem para efectos de agua
 */

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include <wx/wx.h>
#include <wx/dcclient.h>
#include <vector>
#include "Scenario.hpp"
#include "VectorField.hpp"
#include "Boat.hpp"
#include "WaterParticleSystem.hpp"
#include "WaterAnimationSystem.hpp"

namespace tp::presentation {

// Forward declaration
enum class AppMode;

/**
 * @brief Punto de la trayectoria del bateau
 * 
 * Almacena la posición y tiempo de cada punto de la trayectoria
 * para su posterior visualización.
 */
struct TrajectoryPoint {
    double x;        ///< Coordenada X [m]
    double y;        ///< Coordenada Y [m]
    double time;     ///< Tiempo [s]
};

/**
 * @brief Canvas principal de visualización y edición
 * 
 * Maneja el renderizado de todos los elementos de la simulación:
 * - Escenario (células)
 * - Campo vectorial (flechas)
 * - Bateau (triángulo)
 * - Trayectoria (línea)
 * - Partículas (agua/espuma)
 * 
 * Soporta dos modos de operación:
 * - Modo Simulación: visualización pasiva
 * - Modo Editor: edición interactiva de celdas
 */
class SimulationCanvas : public wxPanel {
public:
    /**
     * @brief Constructor
     * @param parent Ventana padre (MainWindow)
     * 
     * Inicializa el canvas con valores por defecto:
     * - Escala: 8.0 px/m
     * - Modo: Simulación
     * - Visible: todo activo
     * - Partículas: sistema con 800 partículas máx
     */
    SimulationCanvas(wxWindow* parent);
    
    // ============================================================
    // CONFIGURACIÓN
    // ============================================================
    
    /**
     * @brief Asigna el escenario a renderizar
     * @param scenario Puntero al escenario (no toma propiedad)
     * 
     * El escenario se renderiza como grille de celdas de colores:
     * - Agua: azul claro
     * - Tierra: verde
     * - Obstáculo: gris/negro
     */
    void setScenario(tp::domain::Scenario* scenario);
    
    void setField(tp::domain::VectorField* field);
    
    tp::domain::VectorField* getField() const { return field_; }
    
    void setBoat(tp::domain::Boat* boat);
    
    /**
     * @brief Establece la escala de renderizado
     * @param scale Pixels por metro (por defecto 8.0)
     */
    void setScale(double scale);
    
    /**
     * @brief Cambia el modo de operación
     * @param mode AppMode::Simulation o AppMode::Editor
     */
    void setMode(AppMode mode);
    
    // ============================================================
    // VISUALIZACIÓN
    // ============================================================
    
    /**
     * @brief Muestra/oculta el campo vectorial
     * @param show true para mostrar
     * 
     * Alias para compatibilidad con MainWindow.
     */
    void showField(bool show) { showField_ = show; Refresh(); }
    
    /**
     * @brief Muestra/oculta la trayectoria
     * @param show true para mostrar
     */
    void showTrajectory(bool show) { showTrajectory_ = show; Refresh(); }
    
    /**
     * @brief Muestra/oculta la grille
     * @param show true para mostrar
     */
    void showGrid(bool show) { showGrid_ = show; Refresh(); }
    
    /**
     * @brief Muestra/oculta el bateau
     * @param show true para mostrar
     */
    void showBoat(bool show) { showBoat_ = show; Refresh(); }
    void showObstacles(bool show) { showObstacles_ = show; Refresh(); }
    
    // Alias para MainWindow
    void setShowField(bool show) { showField(show); }
    void setShowTrajectory(bool show) { showTrajectory(show); }
    void setShowGrid(bool show) { showGrid(show); }
    void setShowBoat(bool show) { showBoat(show); }
    void setShowObstacles(bool show) { showObstacles(show); }
    
    // ============================================================
    // DATOS DE SIMULACIÓN
    // ============================================================
    
    /**
     * @brief Establece la trayectoria a renderizar
     * @param trajectory Vector de puntos TrajectoryPoint
     */
    void setTrajectory(const std::vector<TrajectoryPoint>& trajectory);
    
    /**
     * @brief Limpia la trayectoria actual
     */
    void clearTrajectory();
    
    /**
     * @brief Actualiza la posición del bateau
     * @param x Nueva posición X
     * @param y Nueva posición Y
     * 
     * Se llama desde runSimulationStep() para animación.
     */
    void updateBoatPosition(double x, double y);
    
    // ============================================================
    // SISTEMA DE PARTÍCULAS
    // ============================================================
    
    /**
     * @brief Actualiza el sistema de partículas
     * @param dt Paso temporal [s]
     * 
     * Debe llamarse en cada paso de simulación para:
     * - Emitir estela según velocidad del bateau
     * - Actualizar posición de todas las partículas
     * - Eliminar partículas muertas
     */
    void updateWaterParticles(double dt);
    
    /**
     * @brief Muestra/oculta efectos de agua
     * @param show true para mostrar
     */
    void showWaterEffects(bool show) { showWaterEffects_ = show; Refresh(); }
    
    /**
     * @brief Obtiene el sistema de partículas
     * @return Puntero al WaterParticleSystem
     * 
     * Permite a MainWindow emitir partículas.
     */
    WaterParticleSystem* getParticleSystem() { return particleSystem_; }
    
    // ============================================================
    // MODO EDITOR
    // ============================================================
    
    /**
     * @brief Establece el tamaño del brush
     * @param size Radio del brush [células]
     */
    void setBrushSize(int size) { brushSize_ = size; updateCursor(); }
    
    /**
     * @brief Obtiene el tamaño del brush
     * @return Radio actual
     */
    int getBrushSize() const { return brushSize_; }
    
    /**
     * @brief Activa/desactiva ajuste a grille
     * @param snap true para ajustar
     */
    void setSnapToGrid(bool snap) { snapToGrid_ = snap; }
    
    /**
     * @brief Verifica si snap está activo
     * @return true si está activo
     */
    bool isSnapToGrid() const { return snapToGrid_; }
    
    /**
     * @brief Actualiza el cursor según la herramienta
     * 
     * Cambia el cursor del mouse según:
     * - Modo simulación: cursor por defecto
     * - Modo editor: cruz o círculo según herramienta
     */
    void updateCursor();
    
    /**
     * @brief Activa/desactiva modo editor
     * @param editor true para modo editor
     */
    void setEditorMode(bool editor) { editorMode_ = editor; }
    
    /**
     * @brief Verifica si está en modo editor
     * @return true si está en modo editor
     */
    bool isEditorMode() const { return editorMode_; }
    
    /**
     * @brief Obtiene el escenario para edición
     * @return Puntero al escenario (puede ser nullptr)
     */
    const tp::domain::Scenario* getScenario() const;
    
protected:
    // Event handlers de wxWidgets
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnMotion(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    
    /**
     * @brief Maneja eventos de mouse en modo editor
     * @param event Evento del mouse
     * @param isDown true si es evento de click
     */
    void handleEditorMouseEvent(wxMouseEvent& event, bool isDown);
    
private:
    // Funciones de renderizado
    void drawGrid(wxDC& dc);
    void drawScenario(wxDC& dc);
    void drawVectorField(wxDC& dc);
    void drawWaterFlow(wxDC& dc);
    void drawTrajectory(wxDC& dc);
    void drawBoat(wxDC& dc);
    
    /**
     * @brief Convierte coordenadas del mundo a pantalla
     * @param x Coordenada X mundo
     * @param y Coordenada Y mundo
     * @return Punto en coordenadas de pantalla
     */
    wxPoint worldToScreen(double x, double y);
    
    // ============================================================
    // DATOS A RENDERIZAR - Patrón Observer (Non-Owning Pointers)
    // ============================================================
    
    /**
     * @defgroup observer_pointers Punteros Observadores (Non-Owning)
     * @brief Estos punteros implementan el patrón Observer sin ownership
     * 
     * @details USO DE PUNTEROS CRUDOS EN C++ MODERNO:
     * 
     * En C++ moderno (C++11/14/17/20) generalmente preferimos smart pointers:
     * - std::unique_ptr: Ownership exclusivo (1 dueño)
     * - std::shared_ptr: Ownership compartido (contador de referencias)
     * 
     * Sin embargo, hay un caso legítimo para punteros crudos (raw pointers):
     * cuando implementamos el patrón OBSERVER (Observador).
     * 
     * PATRÓN OBSERVER EN ESTE CONTEXTO:
     * - SimulationCanvas OBSERVA objetos que viven en otro lugar
     * - NO es dueño de estos objetos (no los crea ni destruye)
     * - Solo los usa temporalmente para renderizado
     * - El dueño real es MainWindow/ExperimentService (capa Application)
     * 
     * ¿Por qué no std::shared_ptr?
     * 1. Overhead: Contador de referencias innecesario para observación
     * 2. Ciclos: Podríamos crear referencias circulares accidentalmente
     * 3. Semántica: shared_ptr implica "compartir ownership", no "observar"
     * 
     * ¿Por qué no std::observer_ptr (C++20)?
     * - Es experimental en muchas implementaciones
     * - No añade safety en runtime, solo claridad semántica
     * - El proyecto usa punteros crudos documentados desde el inicio
     * 
     * ALTERNATIVAS CONSIDERADAS:
     * - std::weak_ptr: Para evitar ciclos con shared_ptr (overhead innecesario)
     * - Referencias (&): No permiten null (íbamos a necesitar optional<T&>)
     * - std::optional<std::reference_wrapper<T>>: Verbose, poco idiomático
     * 
     * DECISIÓN: Mantener punteros crudos con documentación explícita
     * y null-checks defensivos en todos los métodos de acceso.
     * 
     * @see https://en.cppreference.com/w/cpp/memory (smart pointers)
     * @see https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-owning
     * 
     * @pre Los objetos apuntados deben permanecer válidos durante todo el
     *      tiempo que SimulationCanvas los use (lifetime requirement)
     * @warning El llamador es responsable de notificar al canvas si los
     *          objetos se destruyen (llamando a setXxx(nullptr))
     * @invariant scenario_, field_, boat_, particleSystem_ pueden ser nullptr
     *            y el código debe manejar este caso gracefulmente
     */
    
    /// @{
    /// @memberof observer_pointers
    
    /**
     * @brief Escenario a renderizar (observador, no propietario)
     * @invariant Puede ser nullptr (canvas sin escenario asignado)
     * @note Seteado por setScenario(), típicamente desde MainWindow
     */
    tp::domain::Scenario* scenario_;
    
    /**
     * @brief Campo vectorial a visualizar (observador, no propietario)
     * @invariant Puede ser nullptr (campo no visible o no asignado)
     * @note El campo puede cambiar dinámicamente durante la simulación
     */
    tp::domain::VectorField* field_;
    
    /**
     * @brief Bote a renderizar (observador, no propietario)
     * @invariant Puede ser nullptr (sin bote en modo editor puro)
     * @note La posición del bote se actualiza cada frame desde el runner
     */
    tp::domain::Boat* boat_;
    /// @}
    
    // Transformación
    double scale_;          ///< Pixels por metro
    double offsetX_;        ///< Desplazamiento X
    double offsetY_;        ///< Desplazamiento Y
    
    // Flags de visualización
    bool showField_;
    bool showTrajectory_;
    bool showGrid_;
    bool showBoat_;
    bool showObstacles_;
    
    // Estado de simulación
    std::vector<TrajectoryPoint> trajectory_;
    double boatX_;
    double boatY_;
    
    /// @{
    /// @memberof observer_pointers
    
    /**
     * @brief Sistema de partículas de agua/espuma (observador, no propietario)
     * @invariant Puede ser nullptr (efectos desactivados o no inicializados)
     * @note El sistema de partículas es creado y destruido por MainWindow
     *       para permitir compartir el mismo sistema entre canvas 2D y 3D
     */
    WaterParticleSystem* particleSystem_;
    
    /// @}
    
    bool showWaterEffects_;     ///< Estado de visibilidad de efectos de agua
    double lastBoatSpeed_;      ///< Velocidad anterior para detección de cambios
    
    /**
     * @brief Sistema de animación de flujo de agua (propiedad exclusiva)
     * @details A diferencia de los punteros anteriores, este SÍ es owned.
     *          Usamos std::unique_ptr porque:
     *          1. El sistema de animación es específico del canvas 2D
     *          2. No necesita ser compartido con otros componentes
     *          3. Su lifetime está acoplado al del canvas
     * 
     * @note El constructor único de unique_ptr asegura que solo el canvas
     *       pueda destruir este objeto, previniendo double-free bugs.
     * 
     * @see std::unique_ptr - https://en.cppreference.com/w/cpp/memory/unique_ptr
     * @see RAII (Resource Acquisition Is Initialization) - técnica fundamental
     *      de C++ para gestión automática de recursos
     */
    std::unique_ptr<WaterAnimationSystem> waterAnimation_;
    bool showWaterFlow_;
    
    // Editor
    int brushSize_;
    bool snapToGrid_;
    bool editorMode_;
    
    wxDECLARE_EVENT_TABLE();
};

} // namespace tp::presentation
