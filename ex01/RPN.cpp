#include "RPN.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

RPN::RPN() {}

RPN::RPN(const RPN& other) : _stack(other._stack) {}

RPN& RPN::operator=(const RPN& other) {
    if (this != &other) {
        _stack = other._stack;
    }
    return *this;
}

RPN::~RPN() {}

bool RPN::isOperator(const std::string& token) const {
    return token == "+" || token == "-" || token == "*" || token == "/";
}

bool RPN::isValidNumber(const std::string& token) const {
    if (token.empty()) return false;

    size_t start = 0;
    if (token[0] == '-' || token[0] == '+') {
        if (token.length() == 1) return false;
        start = 1;
    }

    for (size_t i = start; i < token.length(); ++i) {
        if (!std::isdigit(token[i])) {
            return false;
        }
    }
    return true;
}

double RPN::performOperation(double a, double b, const std::string& op) const {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0) {
            throw std::runtime_error("Division by zero is not allowed");
        }
        return a / b;
    }
    throw std::runtime_error("Unknown operator '" + op + "'");
}

double RPN::evaluate(const std::string& expression) {
    // Clear the stack for each evaluation
    while (!_stack.empty()) {
        _stack.pop();
    }

    if (expression.empty()) {
        throw std::runtime_error("Empty expression provided");
    }

    std::istringstream iss(expression);
    std::string token;
    int tokenCount = 0;

    while (iss >> token) {
        tokenCount++;

        if (isValidNumber(token)) {
            double num = std::stod(token);
            if (num >= 10 || num <= -10) {
                throw std::runtime_error("Numbers must be single digits (less than 10), found: " + token);
            }
            _stack.push(num);
        }
        else if (isOperator(token)) {
            if (_stack.size() < 2) {
                throw std::runtime_error("Insufficient operands for operator '" + token + "' (need 2, have " + std::to_string(_stack.size()) + ")");
            }

            double b = _stack.top();
            _stack.pop();
            double a = _stack.top();
            _stack.pop();

            double result = performOperation(a, b, token);
            _stack.push(result);
        }
        else {
            if (token.find('(') != std::string::npos || token.find(')') != std::string::npos) {
                throw std::runtime_error("Parentheses are not supported in RPN notation");
            }
            if (token.find('.') != std::string::npos) {
                throw std::runtime_error("Decimal numbers are not supported, found: " + token);
            }
            throw std::runtime_error("Invalid token '" + token + "' (only single digits and operators +, -, *, / are allowed)");
        }
    }

    if (tokenCount == 0) {
        throw std::runtime_error("No tokens found in expression");
    }

    if (_stack.size() != 1) {
        if (_stack.size() == 0) {
            throw std::runtime_error("Expression resulted in empty stack (possibly too many operators)");
        } else {
            throw std::runtime_error("Expression incomplete: " + std::to_string(_stack.size()) + " values remain on stack (need exactly 1)");
        }
    }

    return _stack.top();
}
