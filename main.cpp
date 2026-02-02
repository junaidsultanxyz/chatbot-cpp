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
    cout << "Commands:\n";
    cout << "  - Ask any question\n";
    cout << "  - 'close panel' - Close answer panel\n";
    cout << "  - 'new conversation' - Start fresh\n";
    cout << "  - 'list conversations' - View history\n";
    cout << "  - 'exit' - Quit application\n\n";
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
        
        // Add user message to conversation
        conversation.addMessage("user", userInput);
        
        // Check for exit command
        if (chatbot.isExitCommand(userInput)) {
            cout << "\nSaving conversation before exit...\n";
            
            if (!conversation.isEmpty()) {
                string title;
                cout << "Enter conversation title (or press Enter for auto-title): ";
                getline(cin, title);
                
                if (title.empty()) {
                    title = getFirstWords(userInput);
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
                    string title;
                    cout << "Enter conversation title: ";
                    getline(cin, title);
                    
                    if (title.empty()) {
                        title = getFirstWords(userInput);
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
    
    return 0;
}