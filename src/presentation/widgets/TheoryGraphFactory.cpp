#include "widgets/TheoryGraphFactory.hpp"

#include <cmath>

namespace tp::presentation {

void TheoryGraphFactory::configure(MathGraph& graph, const std::string& graphId) {
    graph.clearFunctions();
    graph.clearTrajectories();
    graph.clearVectorField();
    graph.showAxes(true);
    graph.showGrid(true);

    if (graphId == "uniform_field") {
        graph.setTitle(wxT("Campo uniforme y deriva de una lancha"));
        graph.setLabels(wxT("x"), wxT("y"));
        graph.setRange(0, 50, 0, 50);
        graph.addVectorField([](double, double) { return std::make_pair(2.0, 0.0); }, 6.0);
        graph.addTrajectory({{10, 25}, {20, 27}, {30, 29}}, wxColour(255, 120, 0));
    } else if (graphId == "ode_parabola") {
        graph.setTitle(wxT("Posición con aceleración constante"));
        graph.setLabels(wxT("t"), wxT("x(t)"));
        graph.setRange(0, 4, 0, 16);
        graph.addFunction(wxT("x=t²"), [](double t) { return t * t; }, wxColour(0, 102, 204));
    } else if (graphId == "euler_vs_exact") {
        graph.setTitle(wxT("Euler frente a solución exacta"));
        graph.setLabels(wxT("t"), wxT("v(t)"));
        graph.setRange(0, 1.2, 0.5, 3.4);
        graph.addFunction(wxT("Exacta"), [](double t) { return 1.0 + 2.0 * t; }, wxColour(0, 120, 220));
        graph.addTrajectory({{0.0, 1.0}, {0.5, 2.0}, {1.0, 3.0}}, wxColour(255, 140, 0));
    } else if (graphId == "heun_exp") {
        graph.setTitle(wxT("Heun para y' = y"));
        graph.setLabels(wxT("t"), wxT("y(t)"));
        graph.setRange(0, 0.6, 1.0, 1.9);
        graph.addFunction(wxT("Exacta e^t"), [](double t) { return std::exp(t); }, wxColour(0, 120, 220));
        graph.addTrajectory({{0.0, 1.0}, {0.1, 1.105}, {0.2, 1.221}}, wxColour(255, 140, 0));
    } else if (graphId == "rk4_decay") {
        graph.setTitle(wxT("RK4 para un decaimiento exponencial"));
        graph.setLabels(wxT("t"), wxT("y(t)"));
        graph.setRange(0, 1.0, 0.0, 1.1);
        graph.addFunction(wxT("Exacta e^{-2t}"), [](double t) { return std::exp(-2.0 * t); }, wxColour(0, 120, 220));
        graph.addTrajectory({{0.0, 1.0}, {0.2, 0.6704}, {0.4, 0.4493}, {0.6, 0.3012}}, wxColour(255, 140, 0));
    } else if (graphId == "vector_sum") {
        graph.setTitle(wxT("Suma de velocidad propia y corriente lateral"));
        graph.setLabels(wxT("x"), wxT("y"));
        graph.setRange(-1, 6, -1, 3);
        graph.addTrajectory({{0, 0}, {4, 0}}, wxColour(0, 120, 220));
        graph.addTrajectory({{0, 0}, {0, 1.5}}, wxColour(0, 180, 120));
        graph.addTrajectory({{0, 0}, {4, 1.5}}, wxColour(255, 140, 0));
    } else if (graphId == "radial_source") {
        graph.setTitle(wxT("Campo radial"));
        graph.setLabels(wxT("x"), wxT("y"));
        graph.setRange(0, 50, 0, 50);
        graph.addVectorField([](double x, double y) { return std::make_pair((x - 25.0) / 10.0, (y - 25.0) / 10.0); }, 6.0);
    } else if (graphId == "rotational_field") {
        graph.setTitle(wxT("Campo rotacional alrededor del centro"));
        graph.setLabels(wxT("x"), wxT("y"));
        graph.setRange(0, 50, 0, 50);
        graph.addVectorField([](double x, double y) { return std::make_pair(-(y - 25.0) / 10.0, (x - 25.0) / 10.0); }, 6.0);
        graph.addTrajectory({{35, 25}, {32, 32}, {25, 35}, {18, 32}, {15, 25}, {18, 18}, {25, 15}, {32, 18}, {35, 25}}, wxColour(255, 140, 0));
    } else if (graphId == "energy_curve") {
        graph.setTitle(wxT("Energía cinética en función de la velocidad"));
        graph.setLabels(wxT("v"), wxT("E_k"));
        graph.setRange(0, 6, 0, 190);
        graph.addFunction(wxT("E_k = 5v²"), [](double v) { return 5.0 * v * v; }, wxColour(0, 120, 220));
    } else if (graphId == "power_curve") {
        graph.setTitle(wxT("Potencia con fuerza constante"));
        graph.setLabels(wxT("v"), wxT("P"));
        graph.setRange(0, 6, 0, 200);
        graph.addFunction(wxT("P = 30v"), [](double v) { return 30.0 * v; }, wxColour(0, 120, 220));
    } else if (graphId == "parametric_curve") {
        graph.setTitle(wxT("Curva paramétrica de ejemplo"));
        graph.setLabels(wxT("x"), wxT("y"));
        graph.setRange(0, 8, 0, 16);
        graph.addTrajectory({{0, 0}, {1, 0.25}, {2, 1}, {3, 2.25}, {4, 4}, {5, 6.25}, {6, 9}, {7, 12.25}}, wxColour(0, 120, 220));
    } else if (graphId == "line_integral_paths") {
        graph.setTitle(wxT("Trayectoria en un campo uniforme"));
        graph.setLabels(wxT("x"), wxT("y"));
        graph.setRange(0, 10, -1, 6);
        graph.addVectorField([](double, double) { return std::make_pair(1.5, 0.0); }, 2.5);
        graph.addTrajectory({{0, 0}, {5, 0}}, wxColour(255, 140, 0));
        graph.addTrajectory({{0, 2}, {2, 2}, {5, 2}}, wxColour(0, 160, 120));
    } else if (graphId == "circulation_loop") {
        graph.setTitle(wxT("Contorno cerrado alrededor de un vórtice"));
        graph.setLabels(wxT("x"), wxT("y"));
        graph.setRange(0, 50, 0, 50);
        graph.addVectorField([](double x, double y) { return std::make_pair(-(y - 25.0) / 12.0, (x - 25.0) / 12.0); }, 6.0);
        graph.addTrajectory({{33,25},{31,31},{25,33},{19,31},{17,25},{19,19},{25,17},{31,19},{33,25}}, wxColour(255, 140, 0));
    } else if (graphId == "divergence_source") {
        graph.setTitle(wxT("Divergencia positiva: fuente"));
        graph.setLabels(wxT("x"), wxT("y"));
        graph.setRange(0, 50, 0, 50);
        graph.addVectorField([](double x, double y) { return std::make_pair((x - 25.0) / 12.0, (y - 25.0) / 12.0); }, 6.0);
    } else if (graphId == "curl_field") {
        graph.setTitle(wxT("Campo con rotor positivo"));
        graph.setLabels(wxT("x"), wxT("y"));
        graph.setRange(0, 50, 0, 50);
        graph.addVectorField([](double x, double y) { return std::make_pair(-(y - 25.0) / 10.0, (x - 25.0) / 10.0); }, 6.0);
    } else {
        graph.setTitle(wxT("Representación gráfica del concepto"));
        graph.setLabels(wxT("x"), wxT("y"));
        graph.setRange(0, 10, 0, 10);
        graph.addFunction(wxT("Referencia"), [](double x) { return x; }, wxColour(0, 120, 220));
    }
}

} // namespace tp::presentation
