//
// Created by Emil Ebert on 27.06.25.
//

#ifndef BITCOINEXCHANGE_H
#define BITCOINEXCHANGE_H

#include <map>
#include <string>
#include <fstream>
#include <optional>
#include <filesystem>
#include <regex>
#include <chrono>


class BitcoinExchange {
private:
    std::map<std::string, float> exchangeRates;
    bool error;
    static constexpr float MIN_VALUE = 0.0f;
    static constexpr float MAX_VALUE = 1000.0f;
    static const std::string INPUT_SEPARATOR;;
    static const std::string DB_FILE_HEADER;
    static const std::string INPUT_FILE_HEADER;

public:
    BitcoinExchange(const std::string &dbFilePath, const std::string &inputFilePath);

    BitcoinExchange(const BitcoinExchange &other);

    BitcoinExchange &operator=(const BitcoinExchange &other);

    ~BitcoinExchange();

public:
    [[nodiscard]] bool parseDbFile(std::ifstream &dbFile);

    [[nodiscard]] bool processInputFile(std::ifstream &inputFile);

    [[nodiscard]] float getExchangeRate(const std::string &date);

    [[nodiscard]] static std::pair<std::string, float> parseDbLine(const std::string &line);

    [[nodiscard]] static bool isValidDbLine(const std::string &line, std::string& errorMsg, size_t& errorColumn);

    [[nodiscard]] static bool isValidKeyValue(const std::string &key, const std::string &value, std::string& errorMsg, size_t& errorColumn, size_t keyStartPos = 1, size_t valueStartPos = 1);

    [[nodiscard]] static bool isValidInputLine(const std::string &line, std::string& errorMsg, size_t& errorColumn);

    [[nodiscard]] static bool isStringAsFloatInRange(const std::string &value, float min, float max, std::string& errorMsg);

    [[nodiscard]] static std::optional<std::ifstream> openDbFile(const std::string &dbFilePath);

    static void displayError(const std::string& errorMsg, const std::string& line = "", size_t errorColumn = 0, int lineNumber = 0);

    [[nodiscard]] bool isError() const;
};


#endif //BITCOINEXCHANGE_H
