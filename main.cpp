#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include "chatbot.hpp"
#include "tmux_manager.hpp"
#include "conversation.hpp"
#include "utils.hpp"

using namespace std;

void printWelcome() {
    cout << "\n+========================================+\n";
    cout << "|   Welcome to C++ Chatbot System!       |\n";
    cout << "|   Powered by Tmux & System Concepts    |\n";
    cout << "+========================================+\n\n";
    cout << "Type 'help' to see available commands.\n";
}

void autoSaveThread(Conversation* conv, atomic<bool>* running) {
    const int AUTOSAVE_INTERVAL_SECONDS = 1;
    
    while (running->load()) {
        this_thread::sleep_for(chrono::seconds(AUTOSAVE_INTERVAL_SECONDS));
        
        if (running->load() && conv->needsAutoSave()) {
            conv->autoSave();
        }
    }
}

int main() {
    Chatbot chatbot;
    TmuxManager tmux;
    Conversation conversation;
    
    tmux.ensureTmuxSession();
    
    printWelcome();
    
    atomic<bool> running(true);
    thread autoSaver(autoSaveThread, &conversation, &running);
    
    string userInput;
    bool isRunning = true;
    
    while (isRunning) {
        cout << "\nYou: ";
        getline(cin, userInput);
        
        if (userInput.empty()) continue;
        
        if (chatbot.isClearCommand(userInput)) {
            system("clear");
            printWelcome();
            continue;
        }
        
        if (chatbot.isHelpCommand(userInput)) {
            chatbot.showHelp();
            continue;
        }
        
        if (chatbot.isListQuestionCommand(userInput)) {
            chatbot.listQuestions();
            continue;
        }
        
        if (toLower(trim(userInput)).find("load question") == 0) {
            string numStr = trim(userInput.substr(13));
            try {
                int num = stoi(numStr);
                string question = chatbot.getQuestionByNumber(num);
                if (!question.empty()) {
                    cout << "Loading question: " << question << "\n";
                    string answer = chatbot.findAnswer(question);
                    if (!answer.empty()) {
                        tmux.openAnswerPanel(answer);
                        conversation.addMessage("user", question);
                        conversation.addMessage("bot", answer);
                        cout << "Bot: Answer displayed in side panel ➜\n";
                    }
                } else {
                    cout << "Invalid question number.\n";
                }
            } catch (...) {
                cout << "Usage: load question <number>\n";
            }
            continue;
        }
        
        if (toLower(trim(userInput)).find("load convo") == 0) {
            string numStr = trim(userInput.substr(10));
            try {
                int num = stoi(numStr);
                string convoTitle = Conversation::getConversationByNumber(num);
                if (!convoTitle.empty()) {
                    conversation.loadConversationIntoSession(convoTitle);
                } else {
                    cout << "Invalid conversation number.\n";
                }
            } catch (...) {
                cout << "Usage: load convo <number>\n";
            }
            continue;
        }
        
        if (chatbot.isSaveCommand(userInput)) {
            if (conversation.isEmpty()) {
                cout << "No conversation to save.\n";
            } else {
                string title = conversation.getTitle();
                if (title.empty() || title.find("autosave_") == 0) {
                    cout << "Enter conversation title (or press Enter for auto-title): ";
                    getline(cin, title);
                    
                    if (title.empty()) {
                        title = "conversation_" + to_string(time(0));
                    }
                }
                conversation.saveConversation(title);
            }
            continue;
        }
        
        conversation.addMessage("user", userInput);
        
        if (chatbot.isExitCommand(userInput)) {
            cout << "\nSaving conversation before exit...\n";
            
            if (!conversation.isEmpty()) {
                string title = conversation.getTitle();
                if (title.empty() || title.find("autosave_") == 0) {
                    cout << "Enter conversation title (or press Enter for auto-title): ";
                    getline(cin, title);
                    
                    if (title.empty()) {
                        title = "conversation_" + to_string(time(0));
                    }
                }
                conversation.saveConversation(title);
            }
            
            cout << "Goodbye!\n";
            isRunning = false;
            continue;
        }
        
        if (chatbot.isCloseCommand(userInput)) {
            tmux.closeAnswerPanel();
            conversation.addMessage("bot", "Panel closed");
            continue;
        }
        
        if (chatbot.isNewConversationCommand(userInput)) {
            if (!conversation.isEmpty()) {
                cout << "Save current conversation? (y/n): ";
                string save;
                getline(cin, save);
                
                if (toLower(save) == "y" || toLower(save) == "yes") {
                    string title = conversation.getTitle();
                    if (title.empty()) {
                        cout << "Enter conversation title: ";
                        getline(cin, title);
                        
                        if (title.empty()) {
                            title = "conversation_" + to_string(time(0));
                        }
                    }
                    conversation.saveConversation(title);
                }
            }
            
            conversation.clear();
            tmux.closeAnswerPanel();
            cout << "Started new conversation.\n";
            continue;
        }
        
        if (chatbot.isListCommand(userInput)) {
            Conversation::listConversations();
            continue;
        }
        
        string answer = chatbot.findAnswer(userInput);
        
        if (!answer.empty()) {
            tmux.openAnswerPanel(answer);
            conversation.addMessage("bot", answer);
            cout << "Bot: Answer displayed in side panel ➜\n";
        } else {
            cout << "Bot: I'm sorry, I don't have an answer to that question.\n";
            cout << "     Please try rephrasing or ask something else.\n";
            conversation.addMessage("bot", "Answer not found");
        }
    }
    
    running.store(false);
    autoSaver.join();
    
    tmux.killSession();
    
    return 0;
}