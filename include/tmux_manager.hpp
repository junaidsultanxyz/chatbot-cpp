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

using namespace std;

class TmuxManager {
private:
    bool answerPanelOpen;
    string sessionName;

    int executeTmuxCommand(const char* arg1, const char* arg2 = nullptr, 
                          const char* arg3 = nullptr, const char* arg4 = nullptr) {
        pid_t pid = fork();
        
        if (pid < 0) {
            cerr << "Fork failed\n";
            return -1;
        }
        
        if (pid == 0) {
            if (arg4) {
                execl("/usr/bin/tmux", "tmux", arg1, arg2, arg3, arg4, (char*)NULL);
            } else if (arg3) {
                execl("/usr/bin/tmux", "tmux", arg1, arg2, arg3, (char*)NULL);
            } else if (arg2) {
                execl("/usr/bin/tmux", "tmux", arg1, arg2, (char*)NULL);
            } else {
                execl("/usr/bin/tmux", "tmux", arg1, (char*)NULL);
            }
            exit(1);
        }
        
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }

public:
    TmuxManager() : answerPanelOpen(false), sessionName("chatbot_session") {}

    bool isInTmux() {
        return getenv("TMUX") != nullptr;
    }

    void killExistingSession() {
        pid_t pid = fork();
        
        if (pid < 0) {
            return;
        }
        
        if (pid == 0) {
            int devnull = open("/dev/null", O_WRONLY);
            dup2(devnull, STDERR_FILENO);
            close(devnull);
            
            execl("/usr/bin/tmux", "tmux", "has-session", "-t", 
                  sessionName.c_str(), (char*)NULL);
            exit(1);
        }
        
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            executeTmuxCommand("kill-session", "-t", sessionName.c_str());
        }
    }

    void ensureTmuxSession() {
        if (!isInTmux()) {
            killExistingSession();
            
            char exePath[1024];
            ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath)-1);
            if (len != -1) {
                exePath[len] = '\0';
            } else {
                strcpy(exePath, "./bin/chatbot");
            }
            
            string cmd = "tmux new-session -s " + sessionName + " \"" + 
                            string(exePath) + " && exec $SHELL\"";
            system(cmd.c_str());
            exit(0);
        }
    }

    bool paneExists() {
        return system("tmux list-panes | grep -q '^1:'") == 0;
    }

    void killSession() {
        executeTmuxCommand("kill-session", "-t", sessionName.c_str());
    }

    void openAnswerPanel(const string& answer) {
        if (!paneExists()) {
            answerPanelOpen = false;
        }
        
        if (answerPanelOpen) {
            updateAnswerPanel(answer);
            return;
        }

        string tempFile = "/tmp/chatbot_answer.txt";
        ofstream file(tempFile);
        file << answer;
        file.close();

        string cmd = "tmux split-window -h 'cat " + tempFile + 
                         " && echo \"\" && echo \"Press Enter to continue...\" && read'";
        system(cmd.c_str());
        
        answerPanelOpen = true;
    }

    void updateAnswerPanel(const string& answer) {
        string tempFile = "/tmp/chatbot_answer.txt";
        ofstream file(tempFile);
        file << answer;
        file.close();

        system("tmux send-keys -t 1 C-c");
        string cmd = "tmux send-keys -t 1 'cat " + tempFile + 
                         " && echo \"\" && echo \"Press Enter to continue...\" && read' Enter";
        system(cmd.c_str());
    }

    void closeAnswerPanel() {
        if (paneExists()) {
            executeTmuxCommand("kill-pane", "-t", "1");
            cout << "Answer panel closed.\n";
        }
        answerPanelOpen = false;
    }

    bool isPanelOpen() const {
        return answerPanelOpen;
    }
};

#endif