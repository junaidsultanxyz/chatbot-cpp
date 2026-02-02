#ifndef TMUX_MANAGER_H
#define TMUX_MANAGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

class TmuxManager {
private:
    bool answerPanelOpen;
    std::string sessionName;

public:
    TmuxManager() : answerPanelOpen(false), sessionName("chatbot_session") {}

    // Check if we're running inside tmux
    bool isInTmux() {
        return getenv("TMUX") != nullptr;
    }

    // Kill existing chatbot session if it exists
    void killExistingSession() {
        std::string checkCmd = "tmux has-session -t " + sessionName + " 2>/dev/null";
        if (system(checkCmd.c_str()) == 0) {
            std::string killCmd = "tmux kill-session -t " + sessionName;
            system(killCmd.c_str());
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
            std::string cmd = "tmux new-session -s " + sessionName + " \"" + 
                            std::string(exePath) + " && exec $SHELL\"";
            system(cmd.c_str());
            exit(0);
        }
    }

    // Check if pane 1 exists
    bool paneExists() {
        return system("tmux list-panes | grep -q '^1:'") == 0;
    }

    // Kill the tmux session
    void killSession() {
        std::string cmd = "tmux kill-session -t " + sessionName;
        system(cmd.c_str());
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

        // Send keys to the answer pane
        system("tmux send-keys -t 1 C-c");
        std::string cmd = "tmux send-keys -t 1 'cat " + tempFile + 
                         " && echo \"\" && echo \"Press Enter to continue...\" && read' Enter";
        system(cmd.c_str());
    }

    // Close answer panel
    void closeAnswerPanel() {
        if (paneExists()) {
            system("tmux kill-pane -t 1");
            std::cout << "Answer panel closed.\n";
        }
        answerPanelOpen = false;
    }

    bool isPanelOpen() const {
        return answerPanelOpen;
    }
};

#endif