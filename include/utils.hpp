#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

string toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return tolower(c); });
    return result;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

string getFirstWords(const string& str, int wordCount = 5) {
    string result;
    int count = 0;
    bool inWord = false;
    
    for (char c : str) {
        if (isspace(c)) {
            if (inWord) {
                count++;
                if (count >= wordCount) break;
                result += '_';
            }
            inWord = false;
        } else {
            if (isalnum(c)) {
                result += c;
                inWord = true;
            }
        }
    }
    return result.empty() ? "conversation" : result;
}

#endif