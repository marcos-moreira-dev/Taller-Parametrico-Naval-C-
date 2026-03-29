/**
 * @file MathExpression.hpp
 * @brief Evaluador de expresiones matemáticas para campos vectoriales
 * @author Proyecto Taller Paramétrico
 * @version 0.2.0
 * 
 * Permite definir campos vectoriales mediante expresiones matemáticas
 * en formato texto. Soporta funciones trigonométricas, exponenciales,
 * y operadores aritméticos básicos.
 * 
 * @see VectorField para uso en simulación
 * @see ExpressionParser para parsing alternativo
 */

#pragma once

#include <string>
#include <functional>
#include <cmath>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include "Vec2.hpp"

namespace tp::domain {

using namespace tp::shared;

/**
 * @brief Evaluador de expresiones matemáticas con variables x, y
 * 
 * Convierte expresiones en formato texto a funciones evaluables.
 * Utiliza notación postfix (RPN) internamente para el cálculo.
 * 
 * Funciones soportadas:
 * - Trigonométricas: sin, cos, tan
 * - Matemáticas: sqrt, abs, exp, log, ln
 * - Constantes: pi, e
 * - Operadores: +, -, *, /, ^
 * 
 * Ejemplo de uso:
 * @code
 * MathExpression expr;
 * expr.parse("sin(x)", "cos(y)");
 * Vec2d result = expr.evaluate(3.14, 1.0);
 * @endcode
 */
class MathExpression {
public:
    /**
     * @brief Tipo de función evaluable con dos variables
     */
    using EvalFunction = std::function<double(double, double)>;

    /**
     * @brief Constructor por defecto
     */
    MathExpression() : func_(nullptr) {}

    /**
     * @brief Parsea expresión para función escalar (fx)
     * @param expr Expresión matemática con variables x, y
     * @return true si el parsing fue exitoso
     * 
     * La expresión debe contener solo fx (componente X).
     * Retorna Vec2d(f(x,y), 0) al evaluar.
     */
    bool parse(const std::string& expr) {
        try {
            evaluateExpression(expr, 0.0, 0.0);
            func_ = [expr](double x, double y) -> Vec2d {
                double fx = evaluateExpression(expr, x, y);
                return Vec2d(fx, 0.0);
            };
            return true;
        } catch (...) {
            return false;
        }
    }

    /**
     * @brief Parsea expresión para campo vectorial (fx, fy)
     * @param fxExpr Expresión para componente X
     * @param fyExpr Expresión para componente Y
     * @return true si el parsing fue exitoso
     */
    bool parse(const std::string& fxExpr, const std::string& fyExpr) {
        try {
            evaluateExpression(fxExpr, 0.0, 0.0);
            evaluateExpression(fyExpr, 0.0, 0.0);
            func_ = [fxExpr, fyExpr](double x, double y) -> Vec2d {
                double fx = evaluateExpression(fxExpr, x, y);
                double fy = evaluateExpression(fyExpr, x, y);
                return Vec2d(fx, fy);
            };
            return true;
        } catch (...) {
            return false;
        }
    }

    /**
     * @brief Evalúa la expresión en un punto (x,y)
     * @param x Valor de la variable x
     * @param y Valor de la variable y
     * @return Vector resultado (fx, fy)
     */
    Vec2d evaluate(double x, double y) const {
        if (func_) {
            return func_(x, y);
        }
        return Vec2d(0.0, 0.0);
    }

    /**
     * @brief Verifica si la expresión es válida
     * @return true si hay una función definida
     */
    bool isValid() const {
        return func_ != nullptr;
    }

    /**
     * @brief Evalúa una expresión matemática directamente
     * @param expr Expresión a evaluar
     * @param x Valor de x
     * @param y Valor de y
     * @return Resultado numérico
     * 
     * Método estático para evaluación directa sin crear instancia.
     */
    static double evaluateExpression(const std::string& expr, double x, double y) {
        struct Parser {
            const std::string& s;
            size_t pos;
            double x;
            double y;

            void skip() {
                while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos]))) pos++;
            }

            bool consume(char c) {
                skip();
                if (pos < s.size() && s[pos] == c) {
                    pos++;
                    return true;
                }
                return false;
            }

            char peek() {
                skip();
                return pos < s.size() ? s[pos] : '\0';
            }

            double parseExpression() {
                double value = parseTerm();
                while (true) {
                    if (consume('+')) value += parseTerm();
                    else if (consume('-')) value -= parseTerm();
                    else break;
                }
                return value;
            }

            double parseTerm() {
                double value = parsePower();
                while (true) {
                    if (consume('*')) value *= parsePower();
                    else if (consume('/')) {
                        double rhs = parsePower();
                        value = std::abs(rhs) < 1e-12 ? 0.0 : value / rhs;
                    } else break;
                }
                return value;
            }

            double parsePower() {
                double value = parseUnary();
                if (consume('^')) {
                    value = std::pow(value, parsePower());
                }
                return value;
            }

            double parseUnary() {
                if (consume('+')) return parseUnary();
                if (consume('-')) return -parseUnary();
                return parsePrimary();
            }

            std::string parseIdentifier() {
                skip();
                std::string id;
                while (pos < s.size() && (std::isalnum(static_cast<unsigned char>(s[pos])) || s[pos] == '_')) {
                    id += s[pos++];
                }
                return id;
            }

            double parseNumber() {
                skip();
                size_t start = pos;
                while (pos < s.size() && (std::isdigit(static_cast<unsigned char>(s[pos])) || s[pos] == '.')) pos++;
                return std::stod(s.substr(start, pos - start));
            }

            double applyFunction(const std::string& id, const std::vector<double>& args) {
                if (id == "sin") return std::sin(args.at(0));
                if (id == "cos") return std::cos(args.at(0));
                if (id == "tan") return std::tan(args.at(0));
                if (id == "sqrt") return std::sqrt(std::max(0.0, args.at(0)));
                if (id == "abs") return std::abs(args.at(0));
                if (id == "exp") return std::exp(args.at(0));
                if (id == "log" || id == "ln") return std::log(std::max(args.at(0), 1e-12));
                if (id == "atan") return std::atan(args.at(0));
                if (id == "atan2") return std::atan2(args.at(0), args.at(1));
                if (id == "min") return std::min(args.at(0), args.at(1));
                if (id == "max") return std::max(args.at(0), args.at(1));
                throw std::invalid_argument("Funcion no soportada: " + id);
            }

            double parsePrimary() {
                skip();
                if (consume('(')) {
                    double v = parseExpression();
                    if (!consume(')')) throw std::invalid_argument("Falta ')' ");
                    return v;
                }

                char c = peek();
                if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
                    return parseNumber();
                }

                if (std::isalpha(static_cast<unsigned char>(c))) {
                    std::string id = parseIdentifier();
                    if (id == "x") return x;
                    if (id == "y") return y;
                    if (id == "pi") return 3.14159265358979323846;
                    if (id == "e") return 2.71828182845904523536;

                    if (!consume('(')) throw std::invalid_argument("Token inesperado: " + id);
                    std::vector<double> args;
                    args.push_back(parseExpression());
                    while (consume(',')) {
                        args.push_back(parseExpression());
                    }
                    if (!consume(')')) throw std::invalid_argument("Falta ')' en funcion");
                    return applyFunction(id, args);
                }

                throw std::invalid_argument("Expresion invalida");
            }
        };

        Parser parser{expr, 0, x, y};
        double result = parser.parseExpression();
        parser.skip();
        if (parser.pos != expr.size()) {
            throw std::invalid_argument("Sobran caracteres en la expresion");
        }
        return result;
    }

private:
    std::function<Vec2d(double, double)> func_;

    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            if (from.length() == 1 && isalpha(result[pos])) {
                pos++;
                continue;
            }
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        return result;
    }

    static std::string replaceVariable(const std::string& str, const std::string& var, double value) {
        std::string result = str;
        std::string valStr = std::to_string(value);
        
        size_t pos = 0;
        while ((pos = result.find(var, pos)) != std::string::npos) {
            bool isWordBoundary = (pos == 0 || !isalnum(result[pos-1])) && 
                                  (pos + var.length() >= result.length() || 
                                   !isalnum(result[pos + var.length()]));
            if (isWordBoundary) {
                result.replace(pos, var.length(), valStr);
                pos += valStr.length();
            } else {
                pos++;
            }
        }
        return result;
    }

    static double evaluateSimpleExpression(const std::string&) { return 0.0; }
    static double evaluateRPN(const std::vector<std::string>&) { return 0.0; }
};

}
