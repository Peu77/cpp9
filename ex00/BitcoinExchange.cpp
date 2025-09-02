//
// Created by Emil Ebert on 27.06.25.
//

#include "BitcoinExchange.h"
#include "colors.h"

#include <iostream>
#include <chrono>

const std::string BitcoinExchange::INPUT_SEPARATOR = " | ";
const std::string BitcoinExchange::DB_FILE_HEADER = "date,exchange_rate";
const std::string BitcoinExchange::INPUT_FILE_HEADER = "date | value";

BitcoinExchange::BitcoinExchange(const std::string &dbFilePath, const std::string &inputFilePath) : error(false) {
    std::optional<std::ifstream> dbFile = openDbFile(dbFilePath);
    if (!dbFile.has_value()) {
        error = true;
        return;
    }
    error = !parseDbFile(dbFile.value());
    dbFile->close();
    if (error)
        return;

    std::optional<std::ifstream> inputFile = openDbFile(inputFilePath);
    if (!inputFile.has_value()) {
        error = true;
        return;
    }
    error = !processInputFile(inputFile.value());
    inputFile->close();
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other) : exchangeRates(other.exchangeRates),
                                                                 error(other.error) {
}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other) {
    if (this != &other) {
        exchangeRates = other.exchangeRates;
        error = other.error;
    }
    return *this;
}

BitcoinExchange::~BitcoinExchange() = default;

bool BitcoinExchange::parseDbFile(std::ifstream &dbFile) {
    std::string line;
    int lineNumber = 1;
    while (std::getline(dbFile, line)) {
        if (lineNumber == 1) {
            if (line != DB_FILE_HEADER) {
                displayError("Invalid header in database file. Expected '" + DB_FILE_HEADER + "'", line, 0, lineNumber);
                return false;
            }
            lineNumber++;
            continue;
        }

        std::string errorMsg;
        size_t errorColumn;
        if (!isValidDbLine(line, errorMsg, errorColumn)) {
            displayError(errorMsg, line, errorColumn, lineNumber);
            return false;
        }
        auto [date, value] = parseDbLine(line);
        exchangeRates[date] = value;
        lineNumber++;
    }

    if (lineNumber == 1) {
        displayError("Database file is empty or only contains the header", "", 0, 0);
        return false;
    }
    return true;
}

bool BitcoinExchange::processInputFile(std::ifstream &inputFile) {
    std::string line;
    int lineNumber = 1;
    while (std::getline(inputFile, line)) {
        if (lineNumber == 1) {
            if (line != INPUT_FILE_HEADER) {
                displayError("Invalid header in input file. Expected '" + INPUT_FILE_HEADER + "'", line, 0, lineNumber);
                return false;
            }
            lineNumber++;
            continue;
        }

        std::string errorMsg;
        size_t errorColumn;
        if (!isValidInputLine(line, errorMsg, errorColumn)) {
            displayError(errorMsg, line, errorColumn, lineNumber);
            return false;
        }

        const size_t pipePos = line.find(INPUT_SEPARATOR);
        const std::string date = line.substr(0, pipePos);
        const std::string valueStr = line.substr(pipePos + INPUT_SEPARATOR.length());
        const float value = std::stof(valueStr);
        const float rate = getExchangeRate(date);
        const float result = value * rate;

        std::cout << date << " => " << valueStr << " = " << result << std::endl;
        lineNumber++;
    }

    if (lineNumber == 1) {
        displayError("Input file is empty or only contains the header", "", 0, 0);
        return false;
    }
    return true;
}

std::pair<std::string, float> BitcoinExchange::parseDbLine(const std::string &line) {
    const size_t commaPos = line.find(',');
    if (commaPos == std::string::npos) {
        return {"", 0.0f};
    }

    const std::string date = line.substr(0, commaPos);
    const std::string valueStr = line.substr(commaPos + 1);
    const float value = std::stof(valueStr);

    return {date, value};
}

bool BitcoinExchange::isValidDbLine(const std::string &line, std::string &errorMsg, size_t &errorColumn) {
    if (line.empty()) {
        errorMsg = "Empty line";
        errorColumn = 1;
        return false;
    }

    const size_t commaPos = line.find(',');
    if (commaPos == std::string::npos) {
        errorMsg = "Missing comma separator";
        errorColumn = line.length();
        return false;
    }

    const std::string date = line.substr(0, commaPos);
    const std::string valueStr = line.substr(commaPos + 1);

    return isValidKeyValue(date, valueStr, errorMsg, errorColumn, 1, commaPos + 2);
}

bool BitcoinExchange::isValidKeyValue(const std::string &key, const
                                      std::string &value,
                                      std::string &errorMsg,
                                      size_t &errorColumn,
                                      const size_t keyStartPos,
                                      const size_t valueStartPos) {
    const std::regex datePattern(R"(^\d{4,}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$)");
    if (!std::regex_match(key, datePattern)) {
        errorMsg = "Invalid date format (expected YYYY-MM-DD with valid ranges)";
        errorColumn = keyStartPos;
        return false;
    }

    const std::string yearStr = key.substr(0, key.find('-'));

    if (yearStr.length() >= 10) {
        errorMsg = "Year is too large";
        errorColumn = keyStartPos;
        return false;
    }

    const int year = std::stoi(yearStr);
    const int month = std::stoi(key.substr(key.find('-') + 1, 2));
    const int day = std::stoi(key.substr(key.rfind('-') + 1));

    if (year < 1970) {
        errorMsg = "Year cannot be before 1970 (Unix epoch)";
        errorColumn = keyStartPos;
        return false;
    }

    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDays = daysInMonth[month - 1];

    if (month == 2) {
        const bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (isLeapYear) {
            maxDays = 29;
        }
    }

    if (day > maxDays) {
        errorMsg = "Invalid day for the given month and year";
        errorColumn = keyStartPos + 8;
        return false;
    }

    const auto now = std::chrono::system_clock::now();
    const auto time_t = std::chrono::system_clock::to_time_t(now);
    const auto tm = *std::localtime(&time_t);

    const int currentYear = tm.tm_year + 1900;
    const int currentMonth = tm.tm_mon + 1;
    const int currentDay = tm.tm_mday;

    if (year > currentYear ||
        (year == currentYear && month > currentMonth) ||
        (year == currentYear && month == currentMonth && day > currentDay)) {
        errorMsg = "Date cannot be in the future";
        errorColumn = keyStartPos;
        return false;
    }

    const std::regex floatPattern(R"(^[+-]?(\d+\.?\d*|\.\d+)[fF]?$)");
    if (!std::regex_match(value, floatPattern)) {
        errorMsg = "Invalid float format";
        errorColumn = valueStartPos;
        return false;
    }

    if (value == "." || value == "f" || value == "F" ||
        value == "+." || value == "-." ||
        value == "+f" || value == "-f" ||
        value == "+F" || value == "-F") {
        errorMsg = "Invalid float format";
        errorColumn = valueStartPos;
        return false;
    }

    return true;
}

bool BitcoinExchange::isStringAsFloatInRange(const std::string &value,
                                             const float min,
                                             const float max,
                                             std::string &errorMsg) {
    try {
        const float floatValue = std::stof(value);
        if (floatValue < min) {
            errorMsg = "Value must be bigger or equal to " + std::to_string((int) min);
            return false;
        }
        if (floatValue > max) {
            errorMsg = "Value must be between " + std::to_string((int) min) + " and " + std::to_string((int) max);
            return false;
        }
    } catch (const std::exception &e) {
        errorMsg = "Invalid numeric value";
        return false;
    }
    return true;
}

bool BitcoinExchange::isValidInputLine(const std::string &line, std::string &errorMsg, size_t &errorColumn) {
    if (line.empty()) {
        errorMsg = "Empty line";
        errorColumn = 1;
        return false;
    }

    const size_t pipePos = line.find(INPUT_SEPARATOR);
    if (pipePos == std::string::npos) {
        errorMsg = "Missing pipe separator (expected format: date" + INPUT_SEPARATOR + "value)";
        errorColumn = line.length();
        return false;
    }

    const std::string date = line.substr(0, pipePos);
    const std::string valueStr = line.substr(pipePos + INPUT_SEPARATOR.length());

    if (!isValidKeyValue(date, valueStr, errorMsg, errorColumn, 1, pipePos + INPUT_SEPARATOR.length() + 1))
        return false;
    if (!isStringAsFloatInRange(valueStr, MIN_VALUE, MAX_VALUE, errorMsg)) {
        errorColumn = pipePos + INPUT_SEPARATOR.length() + 1;
        return false;
    }
    return true;
}

float BitcoinExchange::getExchangeRate(const std::string &date) {
    auto it = exchangeRates.find(date);
    if (it != exchangeRates.end()) {
        return it->second;
    }

    auto lower = exchangeRates.lower_bound(date);
    if (lower == exchangeRates.begin())
        return lower->second;
    if (lower == exchangeRates.end())
        return std::prev(lower)->second;
    return std::prev(lower)->second;
}

std::optional<std::ifstream> BitcoinExchange::openDbFile(const std::string &dbFilePath) {
    try {
        if (!static_cast<bool>(std::filesystem::status(dbFilePath).permissions() &
                               std::filesystem::perms::owner_read)) {
            std::cerr << "Error: No read permissions for file " << dbFilePath << std::endl;
            return std::nullopt;
        }

        if (!std::filesystem::exists(dbFilePath)) {
            std::cerr << "Error: File " << dbFilePath << " does not exist." << std::endl;
            return std::nullopt;
        }

        if (!std::filesystem::is_regular_file(dbFilePath)) {
            std::cerr << "Error: " << dbFilePath << " is not a regular file." << std::endl;
            return std::nullopt;
        }

        std::ifstream file(dbFilePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << dbFilePath << std::endl;
            return std::nullopt;
        }
        return file;
    } catch (std::exception &e) {
        std::cerr << "Error: Exception occurred while accessing file " << dbFilePath << ": " << e.what() << std::endl;
        return std::nullopt;
    }
}

bool BitcoinExchange::isError() const {
    return error;
}

void BitcoinExchange::displayError(
    const std::string &errorMsg,
    const std::string &line,
    const size_t errorColumn,
    const int lineNumber) {
    std::cerr << BOLD << RED << "✗ Error";

    if (lineNumber > 0) {
        std::cerr << " in line " << YELLOW << lineNumber << RED;
    }

    std::cerr << ": " << RESET << RED << errorMsg << RESET << std::endl;
    if (!line.empty()) {
        std::cerr << CYAN << "  │ " << RESET << line << std::endl;

        if (errorColumn > 0) {
            std::cerr << CYAN << "  │ " << RESET;
            std::cerr << std::string(errorColumn - 1, ' ') << BOLD << RED << "^" << RESET << std::endl;
        }
    }
}
