#include <iostream>
#include "RPN.h"
#include "colors.h"

int main(const int argc, char **argv) {
    if (argc != 2) {
        std::cerr << RED << "Error: " << RESET << "Invalid number of arguments." << std::endl;
        std::cerr << YELLOW << "Usage: " << RESET << argv[0] << " \"<RPN expression>\"" << std::endl;
        std::cerr << CYAN << "Example: " << RESET << argv[0] << " \"8 9 * 9 - 9 - 9 - 4 - 1 +\"" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        RPN calculator;
        double result = calculator.evaluate(argv[1]);
        std::cout << GREEN << "Result: " << RESET << result << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << RED << "Error: " << RESET << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}