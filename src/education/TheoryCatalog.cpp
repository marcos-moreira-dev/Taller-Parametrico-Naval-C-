#include "TheoryCatalog.hpp"
#include <algorithm>

namespace tp::education {

TheoryCatalog& TheoryCatalog::instance() {
    static TheoryCatalog instance;
    return instance;
}

void TheoryCatalog::registerCard(const TheoryCard& card) {
    // Verificar si ya existe un card con ese id
    auto it = std::find_if(cards_.begin(), cards_.end(),
        [&card](const TheoryCard& existing) { return existing.id == card.id; });
    
    if (it != cards_.end()) {
        *it = card;  // Actualizar si existe
    } else {
        cards_.push_back(card);  // Agregar nuevo
    }
}

std::vector<TheoryCard> TheoryCatalog::getAllCards() const {
    return cards_;
}

std::optional<TheoryCard> TheoryCatalog::getCard(const std::string& id) const {
    auto it = std::find_if(cards_.begin(), cards_.end(),
        [&id](const TheoryCard& card) { return card.id == id; });
    
    if (it != cards_.end()) {
        return *it;
    }
    return std::nullopt;
}

std::vector<TheoryCard> TheoryCatalog::getCardsByCategory(const std::string& category) const {
    std::vector<TheoryCard> result;
    std::copy_if(cards_.begin(), cards_.end(), std::back_inserter(result),
        [&category](const TheoryCard& card) { return card.category == category; });
    return result;
}

void TheoryCatalog::initializeDefaultCards() {
    // Conceptos de Cálculo
    registerCard({
        "calculus.vector_fields",
        "Campos Vectoriales",
        "Calculo",
        "Un campo vectorial asigna un vector a cada punto del espacio. "
        "En este taller, representan la velocidad del agua en cada punto.",
        "F(x, y) = (Fx(x,y), Fy(x,y))",
        "La función F toma coordenadas (x,y) y devuelve un vector velocidad. "
        "Visualmente se representa con flechas donde la longitud indica magnitud "
        "y la dirección indica el sentido del flujo.",
        {"gradiente", "divergencia", "rotacional"}
    });
    
    registerCard({
        "calculus.ode",
        "Ecuaciones Diferenciales Ordinarias",
        "Calculo",
        "Las EDO describen cómo cambian las variables con respecto al tiempo. "
        "El movimiento del bote está gobernado por una EDO de segundo orden.",
        "m·d²x/dt² = F_total",
        "Esta es la segunda ley de Newton. La posición del bote cambia "
        "según las fuerzas que actúan sobre él (motor, arrastre, corriente).",
        {"newton", "integracion", "sistemas-dinamicos"}
    });
    
    // Conceptos de Física
    registerCard({
        "physics.drag",
        "Fuerza de Arrastre",
        "Fisica",
        "El arrastre es la resistencia que ofrece el agua al movimiento del bote. "
        "Depende de la velocidad relativa entre bote y agua.",
        "F_arrastre = -½·ρ·Cd·A·v²",
        "ρ = densidad del agua, Cd = coeficiente de arrastre, "
        "A = área transversal, v = velocidad relativa. "
        "El signo negativo indica que opone al movimiento.",
        {"reynolds", "viscosidad", "hidrodinamica"}
    });
    
    registerCard({
        "physics.lift",
        "Fuerza de Sustentación",
        "Fisica",
        "A altas velocidades, el agua genera sustentación dinámica que "
        "reduce la fricción permitiendo que el bote 'planee' sobre el agua.",
        "L = ½·ρ·v²·A·Cl",
        "Cl = coeficiente de sustentación. Esta fuerza reduce el área "
        "mojada del casco, disminuyendo la resistencia total al avance.",
        {"froude", "planeo", "hidrodinamica"}
    });
    
    // Conceptos de Métodos Numéricos
    registerCard({
        "numerics.euler",
        "Método de Euler",
        "Metodos Numericos",
        "El método más simple para integrar EDOs. Usa la derivada actual "
        "para predecir el siguiente estado.",
        "x(t+h) = x(t) + h·f(x(t), t)",
        "h = paso temporal, f = función derivada. Error local O(h²). "
        "Simple pero inestable para pasos grandes. Útil para demostraciones.",
        {"integracion", "errores", "estabilidad"}
    });
    
    registerCard({
        "numerics.rk4",
        "Runge-Kutta de Orden 4",
        "Metodos Numericos",
        "El estándar de oro para integración numérica. Usa 4 evaluaciones "
        "de la derivada para obtener alta precisión.",
        "x(t+h) = x(t) + h/6·(k1 + 2k2 + 2k3 + k4)",
        "ki son las pendientes evaluadas en diferentes puntos. "
        "Error local O(h⁵), mucho más preciso que Euler para el mismo h.",
        {"integracion", "precision", "estabilidad"}
    });
    
    registerCard({
        "numerics.error",
        "Análisis de Error",
        "Metodos Numericos",
        "Los métodos numéricos introducen errores de truncamiento. "
        "Es importante entender y controlar estos errores.",
        "Error ≈ C·h^p",
        "C = constante que depende del problema, p = orden del método. "
        "Reducir h a la mitad reduce el error en 2^p para métodos de orden p.",
        {"convergencia", "estabilidad", "paso-temporal"}
    });
    
    // Conceptos de Navegación
    registerCard({
        "navigation.heading",
        "Rumbo y Orientación",
        "Navegacion",
        "El bote tiene una orientación (dónde apunta) que puede diferir "
        "de su rumbo (dirección de movimiento) debido a la corriente.",
        "Rumbo = atan2(vy, vx)",
        "La diferencia entre orientación y rumbo se llama 'deriva'. "
        "Navegar eficientemente requiere compensar la deriva.",
        {"corriente", "velocidad", "trayectoria"}
    });
    
    registerCard({
        "navigation.scenarios",
        "Escenarios de Navegación",
        "Navegacion",
        "Diferentes entornos acuáticos presentan desafíos únicos: "
        "ríos (corriente uniforme), remolinos (vórtices), canales (paredes).",
        "Campo vectorial específico por escenario",
        "Cada escenario modifica el campo de velocidades del agua. "
        "El bote debe adaptar su estrategia de navegación según el escenario.",
        {"escenarios", "corrientes", "obstaculos"}
    });
}

} // namespace tp::education
