# C++ Chatbot with Tmux Integration

A terminal-based interactive chatbot application built in C++ that demonstrates operating system concepts including process management, inter-process communication, file I/O, and terminal multiplexing using tmux.

## Overview

This chatbot application runs in a tmux session and displays answers in separate terminal panes. It features conversation management with persistent storage, question-answer matching, and an interactive command-line interface.

## Features

- Interactive Q&A system with hardcoded knowledge base
- Tmux-based split panel for displaying answers
- Conversation persistence with file I/O operations
- Load and continue previous conversations
- Auto-detection and installation of dependencies
- Clean architecture with separated concerns

## Technology Stack

### Core Technologies
- **Language**: C++ (C++11 or later)
- **Build System**: g++ compiler with manual compilation
- **Terminal Multiplexer**: tmux for panel management
- **Platform**: Linux (POSIX-compliant systems)

### Standard Libraries Used
- `<iostream>` - Input/output operations
- `<fstream>` - File stream operations
- `<string>` - String handling
- `<vector>` - Dynamic arrays
- `<map>` - Key-value storage for Q&A pairs
- `<cstdlib>` - System command execution
- `<ctime>` - Timestamp generation
- `<unistd.h>` - POSIX API for process management
- `<sys/wait.h>` - Process control
- `<sys/stat.h>` - File/directory status
- `<dirent.h>` - Directory operations

## Project Structure

```
chatbot-cpp/
├── main.cpp              # Entry point and main application loop
├── include/              # Header files
│   ├── chatbot.hpp       # Q&A matching and command handling
│   ├── conversation.hpp  # Conversation storage and retrieval
│   ├── tmux_manager.hpp  # Tmux session and panel management
│   └── utils.hpp         # Utility functions (string processing)
├── bin/                  # Compiled executables
│   └── chatbot
├── conversations/        # Saved conversation files
└── run.sh               # Build and dependency management script
```

## Operating System Concepts Used

### System Calls and POSIX APIs

1. **Process Management**
   - `readlink("/proc/self/exe", ...)` - Get current executable path
   - `exit(0)` - Terminate process
   - `getenv("TMUX")` - Read environment variables
   - `system()` - Execute shell commands

2. **File and Directory Operations**
   - `mkdir(path, 0755)` - Create directory with permissions
   - `opendir()` - Open directory stream
   - `readdir()` - Read directory entries
   - `closedir()` - Close directory stream
   - `stat()` - Get file/directory status

3. **File I/O**
   - `std::ofstream` - Write to files (conversation storage)
   - `std::ifstream` - Read from files (conversation loading)
   - File operations for temporary data storage (`/tmp/chatbot_answer.txt`)

4. **Tmux Commands (via system())**
   - `tmux new-session -s <name>` - Create new tmux session
   - `tmux has-session -t <name>` - Check if session exists
   - `tmux kill-session -t <name>` - Terminate tmux session
   - `tmux split-window -h` - Split window horizontally
   - `tmux list-panes` - List all panes in session
   - `tmux kill-pane -t <id>` - Close specific pane
   - `tmux send-keys -t <id>` - Send keyboard input to pane

5. **Shell Commands**
   - `clear` - Clear terminal screen
   - `cat` - Display file contents
   - `grep` - Pattern matching for pane detection

## Custom Bash Script (run.sh)

The `run.sh` script is a comprehensive build and dependency management tool that handles:

### Purpose
1. **Dependency Detection and Installation**
   - Automatically detects the system's package manager (apt-get, dnf, yum, pacman, zypper)
   - Checks for g++ compiler and tmux installation
   - Installs missing dependencies with appropriate package manager commands

2. **Compilation**
   - Compiles the C++ source code with include directory flags
   - Places the executable in the `bin/` directory

3. **Execution**
   - Automatically runs the compiled chatbot application

### How It Works
```bash
# Detects package manager
command -v apt-get &> /dev/null

# Installs dependencies
sudo apt-get install -y g++
sudo apt-get install -y tmux

# Compiles the project
g++ -I./include main.cpp -o bin/chatbot

# Runs the application
./bin/chatbot
```

## Installation and Usage

### Prerequisites
- Linux operating system
- Root/sudo access for package installation (first run only)

### Quick Start

1. Clone the repository:
```bash
git clone <repository-url>
cd chatbot-cpp
```

2. Make the script executable:
```bash
chmod +x run.sh
```

3. Run the application:
```bash
./run.sh
```

The script will automatically:
- Detect your package manager
- Install g++ if not present
- Install tmux if not present
- Compile the source code
- Launch the chatbot in a tmux session

### Manual Compilation

If you prefer manual compilation:

```bash
# Create bin directory if it doesn't exist
mkdir -p bin

# Compile
g++ -I./include main.cpp -o bin/chatbot

# Run in tmux
tmux new-session -s chatbot_session "./bin/chatbot"
```

## Available Commands

Type `help` in the chatbot to see all available commands:

- `help` - Display help message
- `list question` - List all available questions
- `load question <n>` - Load and display a question by number
- `list convo` - View saved conversations
- `load convo <n>` - Load and continue a conversation by number
- `save` - Save the current conversation
- `new` - Start a new conversation
- `clear` - Clear the screen
- `close` - Close the answer panel
- `exit` - Quit the application

## Knowledge Base Topics

The chatbot can answer questions about:
- C++ programming language
- Tmux terminal multiplexer
- Process management (fork, exec)
- Inter-process communication (pipes, FIFO)
- Multithreading
- Shared resources
- Operating system concepts

## Conversation Management

Conversations are automatically saved to the `conversations/` directory with:
- Timestamps for each message
- Conversation title (auto-generated or user-specified)
- Full message history
- Support for loading and continuing previous conversations

## Technical Highlights

### Architecture Design
- **Modular Design**: Separate headers for distinct functionality
- **Object-Oriented**: Classes for Chatbot, TmuxManager, and Conversation
- **Clean Separation**: Business logic separated from UI and storage

### Operating System Integration
- **Terminal Multiplexing**: Uses tmux for advanced terminal management
- **File System Operations**: Directory creation, file I/O, and persistence
- **Process Control**: Session management and cleanup
- **Environment Interaction**: Reading environment variables and executing system commands

## License

This project is provided as-is for educational purposes demonstrating operating system concepts in C++.
