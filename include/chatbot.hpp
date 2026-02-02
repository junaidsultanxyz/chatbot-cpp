#ifndef CHATBOT_H
#define CHATBOT_H

#include <string>
#include <map>
#include <vector>
#include "utils.hpp"

class Chatbot {
private:
    std::map<std::string, std::string> questionBank;
    std::vector<std::string> exitCommands;
    std::vector<std::string> closeCommands;
    std::vector<std::string> newConversationCommands;
    std::vector<std::string> listCommands;

    void initializeQuestions() {
        // Add your hardcoded questions and answers here
        questionBank["what is c++"] = 
            "C++ is a high-performance programming language that supports object-oriented, "
            "procedural, and generic programming. It was developed by Bjarne Stroustrup "
            "as an extension of C.";
        
        questionBank["what is tmux"] = 
            "Tmux is a terminal multiplexer that allows you to create, access, and control "
            "multiple terminals from a single screen. It's useful for managing multiple "
            "terminal sessions.";
        
        questionBank["what is a process"] = 
            "A process is an instance of a running program. It contains the program code "
            "and its current activity. Each process has its own memory space and system resources.";
        
        questionBank["what is fork"] = 
            "fork() is a system call that creates a new process by duplicating the calling "
            "process. The new process is called the child process, and it runs concurrently "
            "with the parent process.";
        
        questionBank["how are you"] = 
            "I'm doing great! I'm a C++ chatbot built to answer your questions about "
            "programming and operating systems. How can I help you today?";
        
        questionBank["what is multithreading"] = 
            "Multithreading is the ability of a CPU to execute multiple threads concurrently. "
            "Threads are lightweight processes that share the same memory space, making "
            "communication between them faster than inter-process communication.";
    }

    void initializeCommands() {
        exitCommands = {"exit", "quit", "bye", "goodbye"};
        closeCommands = {"close panel", "close", "hide answer"};
        newConversationCommands = {"new conversation", "start new", "restart", "clear"};
        listCommands = {"list conversations", "show conversations", "list", "history"};
    }

    bool matchesAnyCommand(const std::string& input, const std::vector<std::string>& commands) {
        std::string lowerInput = toLower(trim(input));
        for (const auto& cmd : commands) {
            if (lowerInput.find(cmd) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

public:
    Chatbot() {
        initializeQuestions();
        initializeCommands();
    }

    std::string findAnswer(const std::string& question) {
        std::string lowerQuestion = toLower(trim(question));
        
        // Try to find exact or partial match
        for (const auto& pair : questionBank) {
            if (lowerQuestion.find(pair.first) != std::string::npos) {
                return pair.second;
            }
        }
        
        return ""; // No answer found
    }

    bool isExitCommand(const std::string& input) {
        return matchesAnyCommand(input, exitCommands);
    }

    bool isCloseCommand(const std::string& input) {
        return matchesAnyCommand(input, closeCommands);
    }

    bool isNewConversationCommand(const std::string& input) {
        return matchesAnyCommand(input, newConversationCommands);
    }

    bool isListCommand(const std::string& input) {
        return matchesAnyCommand(input, listCommands);
    }

    void addQuestion(const std::string& question, const std::string& answer) {
        questionBank[toLower(question)] = answer;
    }
};

#endif