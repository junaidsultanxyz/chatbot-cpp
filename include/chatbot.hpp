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
        
        questionBank["what are pipes"] = 
            "Pipes are a method of inter-process communication (IPC) that allow data to flow "
            "from one process to another. The pipe() system call creates a unidirectional "
            "communication channel where one process writes to the pipe and another reads from it.";
        
        questionBank["fifo pipes"] = 
            "FIFO (First In First Out) pipes, also called named pipes, are similar to regular "
            "pipes but have a name in the filesystem. Unlike anonymous pipes, FIFOs can be used "
            "by unrelated processes and persist beyond the process that created them. Created "
            "using mkfifo() system call.";
        
        questionBank["shared resources"] = 
            "Shared resources are memory regions or system resources that multiple processes or "
            "threads can access concurrently. Common examples include shared memory segments, "
            "files, and hardware devices. Proper synchronization mechanisms like semaphores, "
            "mutexes, or locks are needed to prevent race conditions.";
        
        questionBank["what is exec"] = 
            "The exec family of functions replaces the current process image with a new program. "
            "Common variants include: execl(), execv(), execle(), execve(), execlp(), execvp(). "
            "They differ in how arguments are passed (list vs array) and whether they use PATH "
            "or environment variables. After exec, the new program runs in the same process ID.";
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