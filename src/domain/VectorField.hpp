#pragma once

/**
 * @file VectorField.hpp
 * @brief Campo vectorial 2D para representar velocidad de fluidos
 * @author Proyecto Taller Paramétrico
 * @version 0.2.0
 * 
 * Representa un campo vectorial F: R² → R² que asigna un vector de velocidad
 * a cada punto (x,y) del espacio. Usado para simular corrientes de agua.
 * 
 * Soporta composición mediante lambdas, permitiendo campos complejos:
 * - Campos uniformes (flujo constante)
 * - Campos radiales (fuentes/sumideros)
 * - Campos rotacionales (vórtices)
 * - Campos lineales (gradientes)
 * 
 * @see Boat::applyHydrodynamicForces para uso en simulación
 * @see SimulationCanvas::drawVectorField para visualización
 */

#include <functional>
#include "Vec2.hpp"

namespace tp::domain {

using namespace tp::shared;

/**
 * @brief Campo vectorial 2D para simulación de fluidos
 * 
 * Implementa el concepto matemático de campo vectorial donde cada punto
 * del espacio 2D tiene asociado un vector velocidad.
 * 
 * Ejemplo de uso:
 * @code
 * // Campo uniforme hacia la derecha
 * auto field = VectorField::uniform(2.0, 0.0);
 * 
 * // Consultar velocidad en punto (10, 20)
 * Vec2d vel = field.getValue(10.0, 20.0);
 * @endcode
 */
class VectorField {
public:
    /**
     * @brief Tipo de función para definir campos personalizados
     * 
     * Función que recibe coordenadas (x,y) y retorna vector velocidad.
     * Permite definir campos arbitrarios mediante lambdas.
     */
    using FieldFunction = std::function<Vec2d(double x, double y)>;
    
    /**
     * @brief Constructor por defecto - campo nulo
     */
    VectorField() = default;
    
    /**
     * @brief Constructor con función personalizada
     * @param func Función que define el campo F(x,y)
     */
    explicit VectorField(FieldFunction func);
    
    /**
     * @brief Obtiene valor del campo en coordenadas (x,y)
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return Vector velocidad en ese punto
     */
    Vec2d getValue(double x, double y) const;
    
    /**
     * @brief Sobrecarga para recibir vector de posición
     * @param pos Posición como Vec2d
     * @return Vector velocidad en esa posición
     */
    Vec2d getValue(const Vec2d& pos) const;
    
    /**
     * @brief Establece una nueva función para el campo
     * @param func Nueva función que define el campo F(x,y)
     * 
     * Permite modificar dinámicamente el comportamiento del campo
     * durante la ejecución, útil para editor de escenarios.
     */
    void setFunction(FieldFunction func);
    
    /**
     * @brief Verifica si el campo tiene una función válida
     * @return true si el campo está inicializado
     * 
     * Un campo sin función válida retorna (0,0) en todas las posiciones.
     */
    bool isValid() const;
    
    // ============================================================
    // FÁBRICAS DE CAMPOS PREDEFINIDOS
    // ============================================================
    
    /**
     * @brief Crea campo uniforme (flujo constante)
     * @param vx Componente X de velocidad
     * @param vy Componente Y de velocidad
     * @return Campo uniforme F(x,y) = (vx, vy)
     * 
     * Representa un flujo constante en toda la región, como un río
     * con corriente uniforme.
     */
    static VectorField uniform(double vx, double vy);
    static VectorField linear(double a, double b, double c, double d);
    static VectorField radial(double cx, double cy, double k);
    static VectorField rotational(double cx, double cy, double strength);
    
    /**
     * @brief Campo de vórtice (rotacional con decaimiento)
     * @param cx Centro X del vórtice
     * @param cy Centro Y del vórtice
     * @param strength Intensidad de rotación
     * @param decay Factor de decaimiento con la distancia
     */
    static VectorField vortex(double cx, double cy, double strength, double decay);
    
    /**
     * @brief Campo de ondas sinusoidales
     * @param amplitude Amplitud de la onda
     * @param frequency Frecuencia espacial
     * @param direction Dirección de propagación (0=horizontal, 1=vertical)
     */
    static VectorField wave(double amplitude, double frequency, int direction);
    
    /**
     * @brief Campo de turbulencia (aleatorio suavizado)
     * @param seed Semilla para el generador aleatorio
     * @param scale Escala de la turbulencia
     * @param intensity Intensidad máxima
     */
    static VectorField turbulence(int seed, double scale, double intensity);
    
    /**
     * @brief Campo de doble vórtice (dos vórtices interactivos)
     * @param cx1 Centro X del primer vórtice
     * @param cy1 Centro Y del primer vórtice
     * @param cx2 Centro X del segundo vórtice
     * @param cy2 Centro Y del segundo vórtice
     * @param strength Intensidad de ambos vórtices
     */
    static VectorField doubleVortex(double cx1, double cy1, double cx2, double cy2, double strength);
    
    /**
     * @brief Campo de canal con obstáculo
     * @param centerY Centro del canal en Y
     * @param width Ancho del canal
     * @param obstacleX Posición X del obstáculo
     * @param flowSpeed Velocidad base del flujo
     */
    static VectorField channelWithObstacle(double centerY, double width, double obstacleX, double flowSpeed);
    
private:
    /**
     * @brief Función interna que define el comportamiento del campo
     * 
     * Almacena la lambda o función que calcula el vector velocidad
     * para cada punto (x,y). Si es nula, el campo retorna (0,0).
     */
    FieldFunction function_;
};

} // namespace tp::domain
