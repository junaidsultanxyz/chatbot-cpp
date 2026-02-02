#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <algorithm>
#include <cctype>

// Convert string to lowercase for case-insensitive comparison
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return result;
}

// Trim whitespace from both ends
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// Extract first N words from string for filename
std::string getFirstWords(const std::string& str, int wordCount = 5) {
    std::string result;
    int count = 0;
    bool inWord = false;
    
    for (char c : str) {
        if (std::isspace(c)) {
            if (inWord) {
                count++;
                if (count >= wordCount) break;
                result += '_';
            }
            inWord = false;
        } else {
            if (std::isalnum(c)) {
                result += c;
                inWord = true;
            }
        }
    }
    return result.empty() ? "conversation" : result;
}

#endif