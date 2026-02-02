#ifndef TMUX_MANAGER_H
#define TMUX_MANAGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

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

    // Start tmux session if not already in one
    void ensureTmuxSession() {
        if (!isInTmux()) {
            // Start new tmux session and re-run the program
            std::string cmd = "tmux new-session -s " + sessionName + " \"" + 
                            std::string(getenv("_")) + " && exec $SHELL\"";
            system(cmd.c_str());
            exit(0);
        }
    }

    // Open answer panel using tmux split
    void openAnswerPanel(const std::string& answer) {
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
        if (!answerPanelOpen) return;
        
        system("tmux kill-pane -t 1");
        answerPanelOpen = false;
        std::cout << "Answer panel closed.\n";
    }

    bool isPanelOpen() const {
        return answerPanelOpen;
    }
};

#endif