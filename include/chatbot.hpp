#ifndef CHATBOT_H
#define CHATBOT_H

#include <string>
#include <map>
#include <vector>
#include "utils.hpp"

using namespace std;

class Chatbot {
private:
    map<string, string> questionBank;
    vector<string> exitCommands;
    vector<string> closeCommands;
    vector<string> newConversationCommands;
    vector<string> listCommands;
    vector<string> listQuestionCommands;
    vector<string> saveCommands;
    vector<string> clearCommands;
    vector<string> helpCommands;

    void initializeQuestions() {
        
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
        
        questionBank["file operations"] = 
            "File operations in Unix/Linux include: open() to open files, read() to read data, "
            "write() to write data, close() to close files, lseek() to move file pointer, "
            "stat() to get file information, and unlink() to delete files. File descriptors "
            "(integers) are used to reference open files. Standard descriptors: 0=stdin, 1=stdout, 2=stderr.";
        
        questionBank["what are semaphores"] = 
            "Semaphores are synchronization primitives used to control access to shared resources. "
            "They maintain a counter: P() (wait/down) decrements it, V() (signal/up) increments it. "
            "Binary semaphores (0 or 1) act like locks. Counting semaphores allow multiple access. "
            "POSIX provides sem_init(), sem_wait(), sem_post(), sem_destroy(). Used to prevent race conditions.";
        
        questionBank["process management"] = 
            "Process management involves creating (fork()), executing (exec()), terminating (exit()), "
            "and monitoring processes. Key system calls: fork() creates child process, wait()/waitpid() "
            "waits for child termination, getpid() returns process ID, getppid() returns parent ID, "
            "kill() sends signals to processes. Process states: running, waiting, stopped, zombie.";
    }

    void initializeCommands() {
        exitCommands = {"exit", "quit", "bye", "goodbye"};
        closeCommands = {"close panel", "close", "hide answer"};
        newConversationCommands = {"new conversation", "start new", "restart", "new"};
        listCommands = {"list convo"};
        listQuestionCommands = {"list question"};
        saveCommands = {"save"};
        clearCommands = {"clear"};
        helpCommands = {"help"};
    }

    bool matchesAnyCommand(const string& input, const vector<string>& commands) {
        string lowerInput = toLower(trim(input));
        for (const auto& cmd : commands) {
            if (lowerInput.find(cmd) != string::npos) {
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

    string findAnswer(const string& question) {
        string lowerQuestion = toLower(trim(question));
        
        for (const auto& pair : questionBank) {
            if (lowerQuestion.find(pair.first) != string::npos) {
                return pair.second;
            }
        }
        
        return "";
    }

    bool isExitCommand(const string& input) {
        return matchesAnyCommand(input, exitCommands);
    }

    bool isCloseCommand(const string& input) {
        return matchesAnyCommand(input, closeCommands);
    }

    bool isNewConversationCommand(const string& input) {
        return matchesAnyCommand(input, newConversationCommands);
    }

    bool isListCommand(const string& input) {
        return matchesAnyCommand(input, listCommands);
    }

    bool isListQuestionCommand(const string& input) {
        return matchesAnyCommand(input, listQuestionCommands);
    }

    bool isSaveCommand(const string& input) {
        return matchesAnyCommand(input, saveCommands);
    }

    bool isClearCommand(const string& input) {
        return matchesAnyCommand(input, clearCommands);
    }

    bool isHelpCommand(const string& input) {
        return matchesAnyCommand(input, helpCommands);
    }

    void showHelp() {
        cout << "\n=== Available Commands ===\n";
        cout << "  help                  - Show this help message\n";
        cout << "  list question         - List all available questions\n";
        cout << "  load question <n>     - Load and display question by number\n";
        cout << "  list convo            - View saved conversations\n";
        cout << "  load convo <n>        - Load and continue conversation by number\n";
        cout << "  save                  - Save current conversation\n";
        cout << "  new                   - Start new conversation\n";
        cout << "  clear                 - Clear screen\n";
        cout << "  close                 - Close answer panel\n";
        cout << "  exit                  - Quit application\n";
        cout << "\nOr simply ask any question!\n";
    }

    void listQuestions() {
        cout << "\n=== Available Questions ===\n";
        int count = 0;
        for (const auto& pair : questionBank) {
            cout << ++count << ". " << pair.first << "\n";
        }
    }

    string getQuestionByNumber(int number) {
        if (number < 1 || number > questionBank.size()) {
            return "";
        }
        
        int count = 0;
        for (const auto& pair : questionBank) {
            if (++count == number) {
                return pair.first;
            }
        }
        return "";
    }

    void addQuestion(const string& question, const string& answer) {
        questionBank[toLower(question)] = answer;
    }
};

#endif