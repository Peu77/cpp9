#include <iostream>

#include "BitcoinExchange.h"
#include "colors.h"

int main(const int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <inputfile>" << std::endl;
        return EXIT_FAILURE;
    }

    if (const BitcoinExchange btc("data.csv", argv[1]); btc.isError()) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}