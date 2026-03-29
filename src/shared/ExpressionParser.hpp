#pragma once

/**
 * @file ExpressionParser.hpp
 * @brief Parser de expresiones matemáticas
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Parser para expresiones matemáticas:
 * - Operadores: +, -, *, /, ^
 * - Funciones: sin, cos, tan, sqrt, abs, exp, log
 * - Variables: x, y
 * - Constantes: pi, e
 * 
 * @note Usado para campos vectoriales personalizados
 * @see MathExpression para implementación alternativa
 */

#include <string>
#include <cmath>
#include <stdexcept>
#include <map>
#include <functional>

// Define M_PI and M_E if not already defined (common on Windows)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

namespace tp::shared {

// Simple recursive descent parser for mathematical expressions
class ExpressionParser {
public:
    using VariableMap = std::map<std::string, double>;
    using FunctionMap = std::map<std::string, std::function<double(double)>>;

    ExpressionParser() {
        // Initialize standard functions
        functions_["sin"] = [](double x) { return std::sin(x); };
        functions_["cos"] = [](double x) { return std::cos(x); };
        functions_["tan"] = [](double x) { return std::tan(x); };
        functions_["exp"] = [](double x) { return std::exp(x); };
        functions_["log"] = [](double x) { return std::log(x); };
        functions_["log10"] = [](double x) { return std::log10(x); };
        functions_["sqrt"] = [](double x) { return std::sqrt(x); };
        functions_["abs"] = [](double x) { return std::abs(x); };
        
        // Constants
        constants_["pi"] = M_PI;
        constants_["e"] = M_E;
    }

    double evaluate(const std::string& expression, const VariableMap& variables = {}) {
        expr_ = expression;
        pos_ = 0;
        variables_ = variables;
        return parseExpression();
    }

    bool validate(const std::string& expression, std::string& errorMsg) {
        try {
            expr_ = expression;
            pos_ = 0;
            variables_ = {};
            parseExpression();
            skipWhitespace();
            if (pos_ < expr_.length()) {
                errorMsg = "Unexpected character at position " + std::to_string(pos_);
                return false;
            }
            return true;
        } catch (const std::exception& e) {
            errorMsg = e.what();
            return false;
        }
    }

private:
    std::string expr_;
    size_t pos_;
    VariableMap variables_;
    FunctionMap functions_;
    std::map<std::string, double> constants_;

    void skipWhitespace() {
        while (pos_ < expr_.length() && std::isspace(expr_[pos_])) {
            pos_++;
        }
    }

    char peek() {
        skipWhitespace();
        return pos_ < expr_.length() ? expr_[pos_] : '\0';
    }

    char get() {
        skipWhitespace();
        return pos_ < expr_.length() ? expr_[pos_++] : '\0';
    }

    double parseExpression() {
        double left = parseTerm();
        
        while (true) {
            char op = peek();
            if (op == '+' || op == '-') {
                get();
                double right = parseTerm();
                if (op == '+') {
                    left += right;
                } else {
                    left -= right;
                }
            } else {
                break;
            }
        }
        
        return left;
    }

    double parseTerm() {
        double left = parseFactor();
        
        while (true) {
            char op = peek();
            if (op == '*' || op == '/') {
                get();
                double right = parseFactor();
                if (op == '*') {
                    left *= right;
                } else {
                    if (right == 0) {
                        throw std::runtime_error("Division by zero");
                    }
                    left /= right;
                }
            } else if (op == '^') {
                get();
                double right = parseFactor();
                left = std::pow(left, right);
            } else {
                break;
            }
        }
        
        return left;
    }

    double parseFactor() {
        char c = peek();
        
        // Handle unary minus
        if (c == '-') {
            get();
            return -parseFactor();
        }
        
        // Handle unary plus
        if (c == '+') {
            get();
            return parseFactor();
        }
        
        // Handle parentheses
        if (c == '(') {
            get();
            double result = parseExpression();
            if (peek() != ')') {
                throw std::runtime_error("Missing closing parenthesis");
            }
            get();
            return result;
        }
        
        // Handle numbers
        if (std::isdigit(c) || c == '.') {
            return parseNumber();
        }
        
        // Handle identifiers (variables, functions, constants)
        if (std::isalpha(c) || c == '_') {
            return parseIdentifier();
        }
        
        throw std::runtime_error(std::string("Unexpected character: ") + c);
    }

    double parseNumber() {
        size_t start = pos_;
        bool hasDecimal = false;
        
        while (pos_ < expr_.length() && (std::isdigit(expr_[pos_]) || expr_[pos_] == '.')) {
            if (expr_[pos_] == '.') {
                if (hasDecimal) {
                    throw std::runtime_error("Invalid number format");
                }
                hasDecimal = true;
            }
            pos_++;
        }
        
        return std::stod(expr_.substr(start, pos_ - start));
    }

    double parseIdentifier() {
        size_t start = pos_;
        
        while (pos_ < expr_.length() && (std::isalnum(expr_[pos_]) || expr_[pos_] == '_')) {
            pos_++;
        }
        
        std::string name = expr_.substr(start, pos_ - start);
        
        // Check for function call
        if (peek() == '(') {
            auto it = functions_.find(name);
            if (it == functions_.end()) {
                throw std::runtime_error("Unknown function: " + name);
            }
            get(); // consume '('
            double arg = parseExpression();
            if (peek() != ')') {
                throw std::runtime_error("Missing closing parenthesis");
            }
            get(); // consume ')'
            return it->second(arg);
        }
        
        // Check for constant
        auto constIt = constants_.find(name);
        if (constIt != constants_.end()) {
            return constIt->second;
        }
        
        // Check for variable
        auto varIt = variables_.find(name);
        if (varIt != variables_.end()) {
            return varIt->second;
        }
        
        throw std::runtime_error("Unknown identifier: " + name);
    }
};

// Helper class to create vector fields from expressions
class VectorFieldExpression {
public:
    VectorFieldExpression(const std::string& fxExpr, const std::string& fyExpr) {
        fxExpr_ = fxExpr;
        fyExpr_ = fyExpr;
        
        // Validate expressions
        std::string error;
        if (!parser_.validate(fxExpr, error)) {
            throw std::runtime_error("Invalid Fx expression: " + error);
        }
        if (!parser_.validate(fyExpr, error)) {
            throw std::runtime_error("Invalid Fy expression: " + error);
        }
    }
    
    double evaluateFx(double x, double y) {
        ExpressionParser::VariableMap vars = {{"x", x}, {"y", y}};
        return parser_.evaluate(fxExpr_, vars);
    }
    
    double evaluateFy(double x, double y) {
        ExpressionParser::VariableMap vars = {{"x", x}, {"y", y}};
        return parser_.evaluate(fyExpr_, vars);
    }
    
private:
    std::string fxExpr_;
    std::string fyExpr_;
    ExpressionParser parser_;
};

} // namespace tp::shared
