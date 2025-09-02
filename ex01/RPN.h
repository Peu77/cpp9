#ifndef RPN_H
#define RPN_H

#include <string>
#include <stack>

class RPN {
private:
    std::stack<double> _stack;

    bool isOperator(const std::string& token) const;
    bool isValidNumber(const std::string& token) const;
    double performOperation(double a, double b, const std::string& op) const;

public:
    RPN();
    RPN(const RPN& other);
    RPN& operator=(const RPN& other);
    ~RPN();

    double evaluate(const std::string& expression);
};

#endif
