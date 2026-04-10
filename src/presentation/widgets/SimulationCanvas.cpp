// Definir ssize_t si no está definido
#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include "SimulationCanvas.hpp"
#include "FieldArrowSampler.hpp"
#include "main_window/MainWindow.hpp"
#include <cmath>

namespace tp::presentation {

wxBEGIN_EVENT_TABLE(SimulationCanvas, wxPanel)
    EVT_LEFT_DOWN(SimulationCanvas::OnLeftDown)
    EVT_MOTION(SimulationCanvas::OnMotion)
    EVT_LEFT_UP(SimulationCanvas::OnLeftUp)
    EVT_PAINT(SimulationCanvas::OnPaint)
    EVT_SIZE(SimulationCanvas::OnSize)
wxEND_EVENT_TABLE()

SimulationCanvas::SimulationCanvas(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
              wxBORDER_SUNKEN | wxFULL_REPAINT_ON_RESIZE)
    , scenario_(nullptr)
    , field_(nullptr)
    , boat_(nullptr)
    , scale_(10.0)
    , offsetX_(0)
    , offsetY_(0)
    , showField_(true)
    , showTrajectory_(true)
    , showGrid_(true)
    , showBoat_(true)
    , showObstacles_(true)
    , boatX_(25.0)
    , boatY_(25.0)
    , particleSystem_(new WaterParticleSystem(800))
    , showWaterEffects_(true)
    , lastBoatSpeed_(0.0)
    , waterAnimation_(std::make_unique<WaterAnimationSystem>())
    , showWaterFlow_(true)
    , brushSize_(1)
    , snapToGrid_(true)
    , editorMode_(false) {
    
    // Color de fondo celeste claro
    SetBackgroundColour(wxColour(200, 230, 255));
    
    // Habilitar doble buffer para evitar parpadeo
    SetDoubleBuffered(true);
    
    // Configurar estilo de fondo para que wx maneje el borrado
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void SimulationCanvas::setScenario(tp::domain::Scenario* scenario) {
    /**
     * @internal IMPLEMENTACIÓN DEL PATRÓN OBSERVER CON NULL SAFETY
     * 
     * Este método demuestra varios principios importantes:
     * 
     * 1. NULL SAFETY (SEGURIDAD ANTE NULOS):
     *    - Aceptamos nullptr como valor válido (canvas sin escenario)
     *    - Verificamos scenario_ != nullptr antes de dereferenciar
     *    - Esto previene crashes si se llama antes de inicializar
     * 
     * 2. PRINCIPIO DE NO ROBERY (NO ROBAR RESPONSABILIDADES):
     *    - No destruimos el escenario anterior (no somos dueños)
     *    - No creamos copia (sería ineficiente para escenarios grandes)
     *    - Solo guardamos referencia para observación/renderizado
     * 
     * 3. AUTO-CONFIGURACIÓN DEFENSIVA:
     *    - Si el escenario es válido, calculamos automáticamente la escala
     *    - Centramos el escenario en el canvas
     *    - Esto evita que el usuario tenga que configurar manualmente
     * 
     * @note El nullptr es un estado válido: representa "sin escenario"
     *       y es útil durante la inicialización o al cerrar experimentos.
     */
    scenario_ = scenario;
    if (scenario_) {
        // Calcular escala para que quepa todo el escenario
        wxSize size = GetClientSize();
        double scaleX = (size.GetWidth() - 40) / static_cast<double>(scenario_->getWidth());
        double scaleY = (size.GetHeight() - 40) / static_cast<double>(scenario_->getHeight());
        scale_ = std::min(scaleX, scaleY);
        
        offsetX_ = (size.GetWidth() - scenario_->getWidth() * scale_) / 2;
        offsetY_ = (size.GetHeight() - scenario_->getHeight() * scale_) / 2;
        
        // Iniciar animación de flujo de agua
        if (waterAnimation_ && showWaterFlow_) {
            waterAnimation_->start(scenario_, field_);
        }
    }
    // Incluso si scenario_ es nullptr, llamamos a Refresh() para limpiar
    // el canvas y mostrar el fondo vacío (celeste claro)
    Refresh();
}

void SimulationCanvas::setField(tp::domain::VectorField* field) {
    /**
     * @internal CAMBIO DINÁMICO DE CAMPO VECTORIAL
     * 
     * A diferencia de setScenario(), este método:
     * - No recalcula escala (el campo no afecta dimensiones)
     * - Notifica al sistema de animación si existe
     * - Permite cambiar el campo durante la simulación en tiempo real
     * 
     * CASOS DE USO:
     * 1. Usuario cambia de tipo de campo (uniforme → radial → etc.)
     * 2. Carga de escenario predefinido con campo específico
     * 3. Campo personalizado desde expresiones matemáticas
     * 
     * NULL SAFETY:
     * - nullptr es válido: oculta el campo vectorial visualmente
     * - waterAnimation_->updateField(nullptr) es seguro si verificamos existencia
     */
    field_ = field;
    if (waterAnimation_) {
        waterAnimation_->updateField(field);
    }
    Refresh();
}

void SimulationCanvas::setBoat(tp::domain::Boat* boat) {
    /**
     * @internal ASIGNACIÓN DE BOTE CON POSICIÓN CACHEADA
     * 
     * PATRÓN: Doble buffering de posición
     * - boatX_, boatY_ son cachés de la última posición conocida
     * - Esto permite renderizar el bote incluso si boat_ es null temporalmente
     * - Útil durante transiciones o cuando el bote se destruye/recrea
     * 
     * CASOS:
     * 1. boat != nullptr: Actualizamos caché desde el objeto Boat
     * 2. boat == nullptr: Mantenemos última posición conocida (boatX_, boatY_)
     *    para evitar que el bote desaparezca repentinamente de la pantalla
     * 
     * @note En modo Editor puro, boat_ puede ser nullptr permanentemente
     *       ya que no hay simulación activa.
     */
    boat_ = boat;
    if (boat_) {
        auto pos = boat_->getPosition();
        boatX_ = pos.x;
        boatY_ = pos.y;
    }
    // Si boat_ es nullptr, mantenemos boatX_/boatY_ para renderizado
    // de "última posición conocida" (ghost boat)
    Refresh();
}

void SimulationCanvas::setScale(double scale) {
    scale_ = scale;
    Refresh();
}

void SimulationCanvas::setMode(AppMode mode) {
    editorMode_ = (mode == AppMode::Editor);
    updateCursor();
    Refresh();
}

void SimulationCanvas::setTrajectory(const std::vector<TrajectoryPoint>& trajectory) {
    trajectory_ = trajectory;
    Refresh();
}

void SimulationCanvas::clearTrajectory() {
    trajectory_.clear();
    Refresh();
}

void SimulationCanvas::updateBoatPosition(double x, double y) {
    boatX_ = x;
    boatY_ = y;
    Refresh();
}

wxPoint SimulationCanvas::worldToScreen(double x, double y) {
    return wxPoint(static_cast<int>(offsetX_ + x * scale_),
                   static_cast<int>(offsetY_ + y * scale_));
}

void SimulationCanvas::OnPaint(wxPaintEvent& event) {
    (void)event;
    wxPaintDC dc(this);
    
    // Fondo
    dc.SetBackground(wxBrush(wxColour(230, 245, 255)));
    dc.Clear();
    
    if (!scenario_) {
        // Mostrar mensaje cuando no hay escenario
        dc.SetTextForeground(wxColour(150, 150, 150));
        dc.SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        wxString msg = wxT("No hay escenario cargado\n\nCree uno nuevo o abra un escenario existente");
        dc.DrawLabel(msg, GetClientRect(), wxALIGN_CENTER);
        return;
    }
    
    if (showGrid_) {
        drawGrid(dc);
    }
    
    drawScenario(dc);
    
    // Dibujar flujo de agua animado
    if (showWaterFlow_ && waterAnimation_ && waterAnimation_->isRunning()) {
        drawWaterFlow(dc);
    }
    
    if (showField_) {
        drawVectorField(dc);
    }
    
    if (showTrajectory_ && !trajectory_.empty()) {
        drawTrajectory(dc);
    }
    
    if (showBoat_) {
        drawBoat(dc);
    }
    
    // Dibujar borde del escenario
    dc.SetPen(wxPen(wxColour(100, 100, 100), 2));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    wxPoint topLeft = worldToScreen(0, 0);
    wxPoint bottomRight = worldToScreen(scenario_->getWidth(), scenario_->getHeight());
    dc.DrawRectangle(topLeft.x, topLeft.y, 
                     bottomRight.x - topLeft.x, 
                     bottomRight.y - topLeft.y);
}

void SimulationCanvas::drawGrid(wxDC& dc) {
    dc.SetPen(wxPen(wxColour(200, 200, 200), 1));
    
    int width = scenario_->getWidth();
    int height = scenario_->getHeight();
    
    // Líneas verticales cada 5 unidades
    for (int x = 0; x <= width; x += 5) {
        wxPoint p1 = worldToScreen(x, 0);
        wxPoint p2 = worldToScreen(x, height);
        dc.DrawLine(p1.x, p1.y, p2.x, p2.y);
    }
    
    // Líneas horizontales cada 5 unidades
    for (int y = 0; y <= height; y += 5) {
        wxPoint p1 = worldToScreen(0, y);
        wxPoint p2 = worldToScreen(width, y);
        dc.DrawLine(p1.x, p1.y, p2.x, p2.y);
    }
}

void SimulationCanvas::drawScenario(wxDC& dc) {
    if (!scenario_) return;
    
    int width = scenario_->getWidth();
    int height = scenario_->getHeight();
    
    // Efecto 3D: dibujar con gradientes y sombras
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            auto cell = scenario_->getCell(x, y);
            
            wxPoint pos = worldToScreen(x, y);
            wxPoint nextPos = worldToScreen(x + 1, y + 1);
            int cellWidth = nextPos.x - pos.x;
            int cellHeight = nextPos.y - pos.y;
            
            // Evitar celdas demasiado pequeñas
            if (cellWidth < 2 || cellHeight < 2) continue;
            
            switch (cell) {
                case tp::shared::CellType::Water: {
                    // Agua azul sólida sin efecto de grid
                    dc.SetBrush(wxBrush(wxColour(74, 156, 217)));
                    dc.SetPen(*wxTRANSPARENT_PEN);
                    dc.DrawRectangle(pos.x, pos.y, cellWidth, cellHeight);
                    break;
                }
                case tp::shared::CellType::Land: {
                    if (!showObstacles_) break;
                    // Tierra color arena
                    dc.SetBrush(wxBrush(wxColour(210, 191, 138)));
                    dc.SetPen(wxPen(wxColour(189, 165, 109), 1));
                    dc.DrawRectangle(pos.x, pos.y, cellWidth, cellHeight);

                    dc.SetPen(wxPen(wxColour(235, 220, 170), 1));
                    dc.DrawLine(pos.x, pos.y, pos.x + cellWidth, pos.y);
                    dc.DrawLine(pos.x, pos.y, pos.x, pos.y + cellHeight);
                    break;
                }
                case tp::shared::CellType::Obstacle: {
                    if (!showObstacles_) break;
                    // Obstáculo gris oscuro
                    dc.SetBrush(wxBrush(wxColour(101, 103, 112)));
                    dc.SetPen(wxPen(wxColour(73, 76, 84), 1));
                    dc.DrawRectangle(pos.x, pos.y, cellWidth, cellHeight);

                    dc.SetPen(wxPen(wxColour(145, 150, 160), 1));
                    dc.DrawLine(pos.x, pos.y, pos.x + cellWidth, pos.y + cellHeight);
                    dc.DrawLine(pos.x + cellWidth, pos.y, pos.x, pos.y + cellHeight);
                    break;
                }
            }
        }
    }
}

void SimulationCanvas::drawVectorField(wxDC& dc) {
    if (!field_ || !scenario_) return;

    const auto arrows = FieldArrowSampler::sample(scenario_, field_, 3, 0.0001, 0.5);
    if (arrows.empty()) {
        return;
    }

    double maxMagnitude = 0.0;
    for (const auto& arrow : arrows) {
        maxMagnitude = std::max(maxMagnitude, arrow.magnitude);
    }
    if (maxMagnitude < 1e-9) {
        return;
    }

    for (const auto& arrow : arrows) {
        wxPoint center = worldToScreen(arrow.x, arrow.y);
        const double referenceMagnitude = 10.0;
        const double normalized = std::clamp(arrow.magnitude / referenceMagnitude, 0.0, 1.0);
        const int penWidth = normalized < 0.30 ? 1 : (normalized < 0.70 ? 2 : 3);

        // Débil: celeste frío. Fuerte: blanco brillante.
        const int red = static_cast<int>(70 + normalized * 185.0);
        const int green = static_cast<int>(150 + normalized * 105.0);
        const int blue = 255;
        const wxColour core(red, green, blue);
        const wxColour glow(
            static_cast<unsigned char>(180 + normalized * 75.0),
            static_cast<unsigned char>(220 + normalized * 35.0),
            255);

        const double arrowLength = std::clamp(7.0 + normalized * 12.0, 7.0, 19.0);
        const double angle = std::atan2(arrow.direction.y, arrow.direction.x);

        wxPoint end(
            center.x + static_cast<int>(arrowLength * std::cos(angle)),
            center.y + static_cast<int>(arrowLength * std::sin(angle))
        );

        dc.SetPen(wxPen(glow, penWidth + 2));
        dc.DrawLine(center.x, center.y, end.x, end.y);
        dc.SetPen(wxPen(core, penWidth));
        dc.DrawLine(center.x, center.y, end.x, end.y);

        const double arrowAngle = 0.5;
        const int arrowSize = static_cast<int>(5 + normalized * 5);
        wxPoint arrow1(
            end.x - static_cast<int>(arrowSize * std::cos(angle - arrowAngle)),
            end.y - static_cast<int>(arrowSize * std::sin(angle - arrowAngle))
        );
        wxPoint arrow2(
            end.x - static_cast<int>(arrowSize * std::cos(angle + arrowAngle)),
            end.y - static_cast<int>(arrowSize * std::sin(angle + arrowAngle))
        );

        dc.SetPen(wxPen(glow, penWidth + 2));
        dc.DrawLine(end.x, end.y, arrow1.x, arrow1.y);
        dc.DrawLine(end.x, end.y, arrow2.x, arrow2.y);
        dc.SetPen(wxPen(core, penWidth));
        dc.DrawLine(end.x, end.y, arrow1.x, arrow1.y);
        dc.DrawLine(end.x, end.y, arrow2.x, arrow2.y);
    }
}

void SimulationCanvas::drawWaterFlow(wxDC& dc) {
    if (!waterAnimation_) return;
    
    const auto& particles = waterAnimation_->getParticles();
    
    for (const auto& p : particles) {
        wxPoint pos = worldToScreen(p.position.x, p.position.y);
        
        // Solo dibujar como puntos pequeños simples
        int alpha = static_cast<int>(std::min(1.0, p.alpha) * 100);
        wxColour color(150, 200, 255, alpha);
        
        dc.SetBrush(wxBrush(color));
        dc.SetPen(*wxTRANSPARENT_PEN);
        
        int size = static_cast<int>(p.size * 0.3);
        if (size < 1) size = 1;
        dc.DrawCircle(pos.x, pos.y, size);
    }
}

void SimulationCanvas::drawTrajectory(wxDC& dc) {
    if (trajectory_.size() < 2) return;
    
    // Dibujar línea de trayectoria
    dc.SetPen(wxPen(wxColour(180, 255, 160), 3));
    
    for (size_t i = 1; i < trajectory_.size(); ++i) {
        wxPoint p1 = worldToScreen(trajectory_[i-1].x, trajectory_[i-1].y);
        wxPoint p2 = worldToScreen(trajectory_[i].x, trajectory_[i].y);
        dc.DrawLine(p1.x, p1.y, p2.x, p2.y);
    }
    
    // Dibujar puntos de la trayectoria
    dc.SetPen(wxPen(wxColour(220, 255, 200), 1));
    dc.SetBrush(wxBrush(wxColour(220, 255, 200)));
    
    for (const auto& point : trajectory_) {
        wxPoint p = worldToScreen(point.x, point.y);
        dc.DrawCircle(p.x, p.y, 2);
    }
}

void SimulationCanvas::drawBoat(wxDC& dc) {
    wxPoint pos = worldToScreen(boatX_, boatY_);
    
    // Dibujar bote como un triángulo
    int size = static_cast<int>(5 * scale_);
    if (size < 8) size = 8;
    if (size > 20) size = 20;
    
    double angle = 0.0;
    if (boat_) {
        angle = boat_->getOrientation();
    }
    
    // Puntos del triángulo (proa, popa izquierda, popa derecha)
    wxPoint p1(
        pos.x + static_cast<int>(size * std::cos(angle)),
        pos.y + static_cast<int>(size * std::sin(angle))
    );
    wxPoint p2(
        pos.x + static_cast<int>(size * 0.5 * std::cos(angle + 2.5)),
        pos.y + static_cast<int>(size * 0.5 * std::sin(angle + 2.5))
    );
    wxPoint p3(
        pos.x + static_cast<int>(size * 0.5 * std::cos(angle - 2.5)),
        pos.y + static_cast<int>(size * 0.5 * std::sin(angle - 2.5))
    );
    
    // Bote color crema/blanco para mejor contraste con el agua azul
    dc.SetPen(wxPen(wxColour(180, 160, 100), 2));  // Borde dorado
    dc.SetBrush(wxBrush(wxColour(255, 250, 220)));   // Relleno crema
    
    wxPoint points[3] = {p1, p2, p3};
    dc.DrawPolygon(3, points);
}

void SimulationCanvas::OnSize(wxSizeEvent& event) {
    (void)event;
    if (scenario_) {
        // Recalcular escala
        wxSize size = GetClientSize();
        double scaleX = (size.GetWidth() - 40) / static_cast<double>(scenario_->getWidth());
        double scaleY = (size.GetHeight() - 40) / static_cast<double>(scenario_->getHeight());
        scale_ = std::min(scaleX, scaleY);
        
        offsetX_ = (size.GetWidth() - scenario_->getWidth() * scale_) / 2;
        offsetY_ = (size.GetHeight() - scenario_->getHeight() * scale_) / 2;
    }
    Refresh();
}

const tp::domain::Scenario* SimulationCanvas::getScenario() const {
    return scenario_;
}

void SimulationCanvas::OnLeftDown(wxMouseEvent& event) {
    if (auto* mainWindow = wxDynamicCast(GetParent(), MainWindow)) {
        mainWindow->beginEditorStroke();
    }
    handleEditorMouseEvent(event, true);
}

void SimulationCanvas::OnMotion(wxMouseEvent& event) {
    if (event.Dragging()) {
        handleEditorMouseEvent(event, true);
    }
}

void SimulationCanvas::OnLeftUp(wxMouseEvent& event) {
    handleEditorMouseEvent(event, false);
    if (auto* mainWindow = wxDynamicCast(GetParent(), MainWindow)) {
        mainWindow->endEditorStroke();
    }
}

void SimulationCanvas::handleEditorMouseEvent(wxMouseEvent& event, bool isDown) {
    // Solo procesar en modo editor
    if (!scenario_ || !editorMode_) return;
    
    wxPoint pos = event.GetPosition();
    
    // Convertir coordenadas de pantalla a coordenadas del mundo
    double worldX = (pos.x - offsetX_) / scale_;
    double worldY = (pos.y - offsetY_) / scale_;
    
    // Aplicar snap a la grilla si está habilitado
    if (snapToGrid_) {
        worldX = std::round(worldX);
        worldY = std::round(worldY);
    }
    
    // Convertir a coordenadas de celda
    int cellX = static_cast<int>(worldX);
    int cellY = static_cast<int>(worldY);
    
    // Validar que esté dentro de los límites del escenario
    if (cellX < 0 || cellX >= scenario_->getWidth() || 
        cellY < 0 || cellY >= scenario_->getHeight()) {
        return;
    }
    
    // Notificar a MainWindow sobre la acción de edición
    if (isDown) {
        if (auto* mainWindow = wxDynamicCast(GetParent(), MainWindow)) {
            mainWindow->applyEditorToolAtCell(cellX, cellY);
        }
    }
}

void SimulationCanvas::updateCursor() {
    // Actualizar el cursor según el tamaño del pincel
    // En wxWidgets, podemos usar cursores personalizados
    int cursorSize = brushSize_ * static_cast<int>(scale_);
    
    // Establecer cursor de cruz para edición
    SetCursor(wxCursor(wxCURSOR_CROSS));
}

}
