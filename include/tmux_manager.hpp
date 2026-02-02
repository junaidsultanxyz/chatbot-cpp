#ifndef TMUX_MANAGER_H
#define TMUX_MANAGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <fcntl.h>

class TmuxManager {
private:
    bool answerPanelOpen;
    std::string sessionName;

    // Helper function to execute tmux command using fork + exec
    int executeTmuxCommand(const char* arg1, const char* arg2 = nullptr, 
                          const char* arg3 = nullptr, const char* arg4 = nullptr) {
        pid_t pid = fork();
        
        if (pid < 0) {
            std::cerr << "Fork failed\n";
            return -1;
        }
        
        if (pid == 0) {
            // Child process - execute tmux command
            if (arg4) {
                execl("/usr/bin/tmux", "tmux", arg1, arg2, arg3, arg4, (char*)NULL);
            } else if (arg3) {
                execl("/usr/bin/tmux", "tmux", arg1, arg2, arg3, (char*)NULL);
            } else if (arg2) {
                execl("/usr/bin/tmux", "tmux", arg1, arg2, (char*)NULL);
            } else {
                execl("/usr/bin/tmux", "tmux", arg1, (char*)NULL);
            }
            // If exec fails
            exit(1);
        }
        
        // Parent process - wait for child
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }

public:
    TmuxManager() : answerPanelOpen(false), sessionName("chatbot_session") {}

    // Check if we're running inside tmux
    bool isInTmux() {
        return getenv("TMUX") != nullptr;
    }

    // Kill existing chatbot session if it exists using fork + exec
    void killExistingSession() {
        // Check if session exists using fork + exec
        pid_t pid = fork();
        
        if (pid < 0) {
            return; // Fork failed, skip cleanup
        }
        
        if (pid == 0) {
            // Child process - check session existence
            // Redirect stderr to /dev/null
            int devnull = open("/dev/null", O_WRONLY);
            dup2(devnull, STDERR_FILENO);
            close(devnull);
            
            execl("/usr/bin/tmux", "tmux", "has-session", "-t", 
                  sessionName.c_str(), (char*)NULL);
            exit(1);
        }
        
        // Parent process - wait and check exit status
        int status;
        waitpid(pid, &status, 0);
        
        // If session exists (exit code 0), kill it
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            executeTmuxCommand("kill-session", "-t", sessionName.c_str());
        }
    }

    // Start tmux session if not already in one
    void ensureTmuxSession() {
        if (!isInTmux()) {
            // Kill any existing chatbot session first
            killExistingSession();
            
            // Get the current executable path
            char exePath[1024];
            ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath)-1);
            if (len != -1) {
                exePath[len] = '\0';
            } else {
                strcpy(exePath, "./bin/chatbot");
            }
            
            // Start new tmux session and re-run the program
            // This needs system() due to complex shell syntax with quotes and &&
            std::string cmd = "tmux new-session -s " + sessionName + " \"" + 
                            std::string(exePath) + " && exec $SHELL\"";
            system(cmd.c_str());
            exit(0);
        }
    }

    // Check if pane 1 exists (uses system due to pipe and grep)
    bool paneExists() {
        return system("tmux list-panes | grep -q '^1:'") == 0;
    }

    // Kill the tmux session using fork + exec
    void killSession() {
        executeTmuxCommand("kill-session", "-t", sessionName.c_str());
    }

    // Open answer panel using tmux split
    void openAnswerPanel(const std::string& answer) {
        // Check if pane actually exists, reset flag if it doesn't
        if (!paneExists()) {
            answerPanelOpen = false;
        }
        
        if (answerPanelOpen) {
            // If panel is already open, update it
            updateAnswerPanel(answer);
            return;
        }

        // Create a temporary file with the answer
        std::string tempFile = "/tmp/chatbot_answer.txt";
        std::ofstream file(tempFile);
        file << answer;
        file.close();

        // Split window horizontally and display answer
        // Uses system() due to complex shell syntax with nested quotes and &&
        std::string cmd = "tmux split-window -h 'cat " + tempFile + 
                         " && echo \"\" && echo \"Press Enter to continue...\" && read'";
        system(cmd.c_str());
        
        answerPanelOpen = true;
    }

    // Update existing answer panel
    void updateAnswerPanel(const std::string& answer) {
        std::string tempFile = "/tmp/chatbot_answer.txt";
        std::ofstream file(tempFile);
        file << answer;
        file.close();

        // Send keys to the answer pane (uses system() due to complex command construction)
        system("tmux send-keys -t 1 C-c");
        std::string cmd = "tmux send-keys -t 1 'cat " + tempFile + 
                         " && echo \"\" && echo \"Press Enter to continue...\" && read' Enter";
        system(cmd.c_str());
    }

    // Close answer panel using fork + exec
    void closeAnswerPanel() {
        if (paneExists()) {
            executeTmuxCommand("kill-pane", "-t", "1");
            std::cout << "Answer panel closed.\n";
        }
        answerPanelOpen = false;
    }

    bool isPanelOpen() const {
        return answerPanelOpen;
    }
};

#endif