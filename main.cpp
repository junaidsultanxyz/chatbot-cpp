#include <iostream>
#include <string>
#include "chatbot.hpp"
#include "tmux_manager.hpp"
#include "conversation.hpp"
#include "utils.hpp"

using namespace std;

void printWelcome() {
    cout << "\n╔════════════════════════════════════════╗\n";
    cout << "║   Welcome to C++ Chatbot System!      ║\n";
    cout << "║   Powered by Tmux & System Concepts    ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    cout << "Type 'help' to see available commands.\n";
}

int main() {
    Chatbot chatbot;
    TmuxManager tmux;
    Conversation conversation;
    
    // Ensure we're running in tmux
    tmux.ensureTmuxSession();
    
    printWelcome();
    
    string userInput;
    bool running = true;
    
    while (running) {
        cout << "\nYou: ";
        getline(cin, userInput);
        
        if (userInput.empty()) continue;
        
        // Check for clear command
        if (chatbot.isClearCommand(userInput)) {
            system("clear");
            printWelcome();
            continue;
        }
        
        // Check for help command
        if (chatbot.isHelpCommand(userInput)) {
            chatbot.showHelp();
            continue;
        }
        
        // Check for list questions command
        if (chatbot.isListQuestionCommand(userInput)) {
            chatbot.listQuestions();
            continue;
        }
        
        // Check for load question command
        if (toLower(trim(userInput)).find("load question") == 0) {
            string numStr = trim(userInput.substr(13)); // Remove "load question"
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
        
        // Check for load convo command
        if (toLower(trim(userInput)).find("load convo") == 0) {
            string numStr = trim(userInput.substr(10)); // Remove "load convo"
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
        
        // Check for save command
        if (chatbot.isSaveCommand(userInput)) {
            if (conversation.isEmpty()) {
                cout << "No conversation to save.\n";
            } else {
                string title = conversation.getTitle();
                if (title.empty()) {
                    cout << "Enter conversation title (or press Enter for auto-title): ";
                    getline(cin, title);
                    
                    if (title.empty()) {
                        // Use first user message for auto-title
                        title = "conversation_" + to_string(time(0));
                    }
                }
                conversation.saveConversation(title);
            }
            continue;
        }
        
        // Add user message to conversation
        conversation.addMessage("user", userInput);
        
        // Check for exit command
        if (chatbot.isExitCommand(userInput)) {
            cout << "\nSaving conversation before exit...\n";
            
            if (!conversation.isEmpty()) {
                string title = conversation.getTitle();
                if (title.empty()) {
                    cout << "Enter conversation title (or press Enter for auto-title): ";
                    getline(cin, title);
                    
                    if (title.empty()) {
                        title = "conversation_" + to_string(time(0));
                    }
                }
                conversation.saveConversation(title);
            }
            
            cout << "Goodbye!\n";
            running = false;
            continue;
        }
        
        // Check for close panel command
        if (chatbot.isCloseCommand(userInput)) {
            tmux.closeAnswerPanel();
            conversation.addMessage("bot", "Panel closed");
            continue;
        }
        
        // Check for new conversation command
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
        
        // Check for list conversations command
        if (chatbot.isListCommand(userInput)) {
            Conversation::listConversations();
            continue;
        }
        
        // Find answer to question
        string answer = chatbot.findAnswer(userInput);
        
        if (!answer.empty()) {
            // Display answer in new tmux panel
            tmux.openAnswerPanel(answer);
            conversation.addMessage("bot", answer);
            cout << "Bot: Answer displayed in side panel ➜\n";
        } else {
            cout << "Bot: I'm sorry, I don't have an answer to that question.\n";
            cout << "     Please try rephrasing or ask something else.\n";
            conversation.addMessage("bot", "Answer not found");
        }
    }
    
    // Clean up: kill tmux session on exit
    tmux.killSession();
    
    return 0;
}