#pragma once

/**
 * @file TheoryCatalog.hpp
 * @brief Catálogo de tarjetas de teoría física
 * @author TallerParamétrico
 * @version 1.0
 * @see TheoryPanel
 */

#include <string>
#include <vector>
#include <functional>
#include <optional>

namespace tp::education {

/**
 * @brief Tarjeta de teoría física con fórmula y descripción
 * @struct TheoryCard
 * @details Representa una tarjeta educativa que contiene información
 * sobre un concepto físico, incluyendo fórmula matemática e interpretación
 */

struct TheoryCard {
    std::string id;
    std::string title;
    std::string category;
    std::string description;
    std::string formula;
    std::string interpretation;
    std::vector<std::string> relatedConcepts;
};

/**
 * @brief Catálogo singleton de tarjetas de teoría física
 * @class TheoryCatalog
 * @details Gestiona el registro y recuperación de tarjetas educativas.
 * Proporciona acceso a conceptos físicos con fórmulas e interpretaciones.
 */
class TheoryCatalog {
public:
    /**
     * @brief Obtiene la instancia singleton del catálogo
     * @return Referencia al catálogo único
     */
    static TheoryCatalog& instance();
    
    /**
     * @brief Registra una nueva tarjeta de teoría
     * @param card Tarjeta a registrar
     */
    void registerCard(const TheoryCard& card);
    
    /**
     * @brief Obtiene todas las tarjetas registradas
     * @return Vector con todas las tarjetas
     */
    std::vector<TheoryCard> getAllCards() const;
    
    /**
     * @brief Obtiene una tarjeta por su identificador
     * @param id Identificador de la tarjeta
     * @return Tarjeta si existe, vacio en caso contrario
     */
    std::optional<TheoryCard> getCard(const std::string& id) const;
    
    /**
     * @brief Obtiene tarjetas filtradas por categoría
     * @param category Categoría a filtrar
     * @return Vector de tarjetas de la categoría
     */
    std::vector<TheoryCard> getCardsByCategory(const std::string& category) const;
    
    /**
     * @brief Inicializa el catálogo con tarjetas predefinidas
     */
    void initializeDefaultCards();
    
private:
    TheoryCatalog() = default;
    std::vector<TheoryCard> cards_;
};

}
