#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>
#include <sys/stat.h>
#include <dirent.h>

struct Message {
    std::string type; // "user" or "bot"
    std::string content;
    std::string timestamp;
};

class Conversation {
private:
    std::vector<Message> messages;
    std::string title;
    std::string filename;
    const std::string SAVE_DIR = "conversations/";

    std::string getCurrentTimestamp() {
        time_t now = time(0);
        char buf[80];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return std::string(buf);
    }

    void createDirectoryIfNotExists() {
        struct stat info;
        if (stat(SAVE_DIR.c_str(), &info) != 0) {
            mkdir(SAVE_DIR.c_str(), 0755);
        }
    }

public:
    Conversation() : title(""), filename("") {
        createDirectoryIfNotExists();
    }

    void addMessage(const std::string& type, const std::string& content) {
        Message msg;
        msg.type = type;
        msg.content = content;
        msg.timestamp = getCurrentTimestamp();
        messages.push_back(msg);
    }

    void setTitle(const std::string& t) {
        title = t;
    }

    std::string getTitle() const {
        return title;
    }

    bool isEmpty() const {
        return messages.empty();
    }

    void saveConversation(const std::string& conversationTitle) {
        if (messages.empty()) return;

        title = conversationTitle;
        filename = SAVE_DIR + title + ".txt";

        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not save conversation\n";
            return;
        }

        file << "Title: " << title << "\n";
        file << "Date: " << getCurrentTimestamp() << "\n";
        file << "=====================================\n\n";

        for (const auto& msg : messages) {
            file << "[" << msg.timestamp << "] " << msg.type << ": " << msg.content << "\n\n";
        }

        file.close();
        std::cout << "Conversation saved as: " << filename << std::endl;
    }

    void clear() {
        messages.clear();
        title = "";
        filename = "";
    }

    static void listConversations() {
        const std::string SAVE_DIR = "conversations/";
        DIR* dir = opendir(SAVE_DIR.c_str());
        
        if (!dir) {
            std::cout << "No previous conversations found.\n";
            return;
        }

        std::cout << "\n=== Previous Conversations ===\n";
        struct dirent* entry;
        int count = 0;
        
        while ((entry = readdir(dir)) != nullptr) {
            std::string fname = entry->d_name;
            if (fname.length() > 4 && fname.substr(fname.length() - 4) == ".txt") {
                std::cout << ++count << ". " << fname.substr(0, fname.length() - 4) << "\n";
            }
        }
        
        closedir(dir);
        if (count == 0) {
            std::cout << "No conversations found.\n";
        }
    }

    static void loadConversation(const std::string& conversationTitle) {
        std::string filename = "conversations/" + conversationTitle + ".txt";
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            std::cout << "Conversation not found.\n";
            return;
        }

        std::cout << "\n=== Loading Conversation ===\n";
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << "\n";
        }
        file.close();
    }
};

#endif