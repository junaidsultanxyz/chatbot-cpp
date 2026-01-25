#!/bin/bash

PROJECT_NAME=chatbot
COMPILER=g++
INCLUDE=include
MAIN_FILE=main.cpp

# detect package manager
PACKAGE_MANAGER=""
if command -v apt-get &> /dev/null; then
    PACKAGE_MANAGER="apt-get"
elif command -v dnf &> /dev/null; then
    PACKAGE_MANAGER="dnf"
elif command -v yum &> /dev/null; then
    PACKAGE_MANAGER="yum"
elif command -v pacman &> /dev/null; then
    PACKAGE_MANAGER="pacman"
elif command -v zypper &> /dev/null; then
    PACKAGE_MANAGER="zypper"
else
    echo "Error: Could not detect package manager. Please install dependencies manually."
    exit 1
fi

echo "Detected package manager: $PACKAGE_MANAGER"

# function to install package
install_package() {
    local package=$1
    echo "Installing $package..."
    
    case $PACKAGE_MANAGER in
        apt-get)
            sudo apt-get update && sudo apt-get install -y $package
            ;;
        dnf)
            sudo dnf install -y $package
            ;;
        yum)
            sudo yum install -y $package
            ;;
        pacman)
            sudo pacman -S --noconfirm $package
            ;;
        zypper)
            sudo zypper install -y $package
            ;;
    esac
}

# check if g++ is installed
if ! command -v $COMPILER &> /dev/null; then
    echo "C++ compiler ($COMPILER) not found."
    
    # install appropriate package based on package manager
    case $PACKAGE_MANAGER in
        apt-get|dnf|yum|zypper)
            install_package "g++"
            ;;
        pacman)
            install_package "gcc"
            ;;
    esac
    
    # verify installation
    if ! command -v $COMPILER &> /dev/null; then
        echo "Error: Failed to install $COMPILER"
        exit 1
    fi
    echo "$COMPILER installed successfully!"
fi

# check if tmux is installed
if ! command -v tmux &> /dev/null; then
    echo "tmux not found."
    install_package "tmux"
    
    # verify installation
    if ! command -v tmux &> /dev/null; then
        echo "Error: Failed to install tmux"
        exit 1
    fi
    echo "tmux installed successfully!"
fi

sleep 1
clear

$COMPILER -I./$INCLUDE $MAIN_FILE -o bin/$PROJECT_NAME
./bin/$PROJECT_NAME