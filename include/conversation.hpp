#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>
#include <mutex>
#include <atomic>

using namespace std;

struct Message {
    string type;
    string content;
    string timestamp;
};

class Conversation {
private:
    vector<Message> messages;
    string title;
    string filename;
    const string SAVE_DIR = "conversations/";
    mutable mutex conversationMutex;
    atomic<bool> isDirty;
    time_t lastSaveTime;

    string getCurrentTimestamp() {
        time_t now = time(0);
        char buf[80];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return string(buf);
    }

    void createDirectoryIfNotExists() {
        struct stat info;
        if (stat(SAVE_DIR.c_str(), &info) != 0) {
            mkdir(SAVE_DIR.c_str(), 0755);
        }
    }

public:
    Conversation() : title(""), filename(""), isDirty(false), lastSaveTime(0) {
        createDirectoryIfNotExists();
    }

    void addMessage(const string& type, const string& content) {
        lock_guard<mutex> lock(conversationMutex);
        Message msg;
        msg.type = type;
        msg.content = content;
        msg.timestamp = getCurrentTimestamp();
        messages.push_back(msg);
        isDirty = true;
    }

    void setTitle(const string& t) {
        lock_guard<mutex> lock(conversationMutex);
        title = t;
        isDirty = true;
    }

    string getTitle() const {
        lock_guard<mutex> lock(conversationMutex);
        return title;
    }

    bool isEmpty() const {
        lock_guard<mutex> lock(conversationMutex);
        return messages.empty();
    }

    void saveConversation(const string& conversationTitle) {
        lock_guard<mutex> lock(conversationMutex);
        if (messages.empty()) return;

        string oldFilename = filename;
        string oldTitle = title;
        
        title = conversationTitle;
        filename = SAVE_DIR + title + ".txt";

        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not save conversation\n";
            return;
        }

        file << "Title: " << title << "\n";
        file << "Date: " << getCurrentTimestamp() << "\n";
        file << "=====================================\n\n";

        for (const auto& msg : messages) {
            file << "[" << msg.timestamp << "] " << msg.type << ": " << msg.content << "\n\n";
        }

        file.close();

        if (!oldFilename.empty() && oldTitle.find("autosave_") == 0 && oldFilename != filename) {
            remove(oldFilename.c_str());
            cout << "Conversation renamed to: " << filename << endl;
        } else {
            cout << "Conversation saved as: " << filename << endl;
        }
        
        isDirty = false;
        lastSaveTime = time(0);
    }

    void clear() {
        lock_guard<mutex> lock(conversationMutex);
        messages.clear();
        title = "";
        filename = "";
        isDirty = false;
    }

    static vector<string> listConversations() {
        const string SAVE_DIR = "conversations/";
        DIR* dir = opendir(SAVE_DIR.c_str());
        vector<string> conversations;
        
        if (!dir) {
            cout << "No previous conversations found.\n";
            return conversations;
        }

        cout << "\n=== Previous Conversations ===\n";
        struct dirent* entry;
        int count = 0;
        
        while ((entry = readdir(dir)) != nullptr) {
            string fname = entry->d_name;
            if (fname.length() > 4 && fname.substr(fname.length() - 4) == ".txt") {
                string title = fname.substr(0, fname.length() - 4);
                conversations.push_back(title);
                cout << ++count << ". " << title << "\n";
            }
        }
        
        closedir(dir);
        if (count == 0) {
            cout << "No conversations found.\n";
        }
        return conversations;
    }

    static void loadConversation(const string& conversationTitle) {
        string filename = "conversations/" + conversationTitle + ".txt";
        ifstream file(filename);
        
        if (!file.is_open()) {
            cout << "Conversation not found.\n";
            return;
        }

        cout << "\n=== Loading Conversation ===\n";
        string line;
        while (getline(file, line)) {
            cout << line << "\n";
        }
        file.close();
    }

    static string getConversationByNumber(int number) {
        const string SAVE_DIR = "conversations/";
        DIR* dir = opendir(SAVE_DIR.c_str());
        
        if (!dir) {
            return "";
        }

        struct dirent* entry;
        int count = 0;
        string result;
        
        while ((entry = readdir(dir)) != nullptr) {
            string fname = entry->d_name;
            if (fname.length() > 4 && fname.substr(fname.length() - 4) == ".txt") {
                if (++count == number) {
                    result = fname.substr(0, fname.length() - 4);
                    break;
                }
            }
        }
        
        closedir(dir);
        return result;
    }

    bool loadConversationIntoSession(const string& conversationTitle) {
        string fname = SAVE_DIR + conversationTitle + ".txt";
        ifstream file(fname);
        
        if (!file.is_open()) {
            cout << "Conversation not found.\n";
            return false;
        }

        messages.clear();
        title = conversationTitle;
        filename = fname;

        cout << "\n=== Loading Conversation: " << conversationTitle << " ===\n";
        
        string line;
        bool inMessages = false;
        
        while (getline(file, line)) {
            if (line.find("=====") != string::npos) {
                inMessages = true;
                continue;
            }
            
            if (!inMessages) {
                cout << line << "\n";
                continue;
            }
            
            if (line.find("[") == 0 && line.find("]") != string::npos) {
                size_t endBracket = line.find("]");
                string timestamp = line.substr(1, endBracket - 1);
                
                size_t colonPos = line.find(": ", endBracket);
                if (colonPos != string::npos) {
                    string typeAndContent = line.substr(endBracket + 2);
                    size_t typeEnd = typeAndContent.find(": ");
                    
                    if (typeEnd != string::npos) {
                        string type = typeAndContent.substr(0, typeEnd);
                        string content = typeAndContent.substr(typeEnd + 2);
                        
                        Message msg;
                        msg.type = type;
                        msg.content = content;
                        msg.timestamp = timestamp;
                        messages.push_back(msg);
                        
                        cout << "[" << timestamp << "] " << type << ": " << content << "\n";
                    }
                }
            }
        }
        
        file.close();
        cout << "\n✓ Conversation loaded. You can continue from here.\n";
        return true;
    }

    void autoSave() {
        lock_guard<mutex> lock(conversationMutex);
        
        if (!isDirty || messages.empty()) {
            return;
        }

        string saveTitle = title;
        if (saveTitle.empty()) {
            saveTitle = "autosave_" + to_string(time(0));
            title = saveTitle;
        }

        filename = SAVE_DIR + saveTitle + ".txt";
        ofstream file(filename);
        if (!file.is_open()) {
            return;
        }

        file << "Title: " << saveTitle << "\n";
        file << "Date: " << getCurrentTimestamp() << "\n";
        file << "=====================================\n\n";

        for (const auto& msg : messages) {
            file << "[" << msg.timestamp << "] " << msg.type << ": " << msg.content << "\n\n";
        }

        file.close();
        isDirty = false;
        lastSaveTime = time(0);
    }

    bool needsAutoSave() const {
        return isDirty.load() && !messages.empty();
    }

    bool hasTitleForAutoSave() const {
        lock_guard<mutex> lock(conversationMutex);
        return !title.empty();
    }
};

#endif