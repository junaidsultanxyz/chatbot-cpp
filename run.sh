#!/bin/bash

PROJECT_NAME=chatbot
COMPILER=g++
INCLUDE=include
MAIN_FILE=main.cpp

# check if g++ is installed
if ! command -v $COMPILER &> /dev/null; then
    echo "C++ compiler ($COMPILER) not found. Installing..."
    
    # detect package manager and install g++
    if command -v apt-get &> /dev/null; then
        sudo apt-get update && sudo apt-get install -y g++
    elif command -v dnf &> /dev/null; then
        sudo dnf install -y gcc-c++
    elif command -v yum &> /dev/null; then
        sudo yum install -y gcc-c++
    elif command -v pacman &> /dev/null; then
        sudo pacman -S --noconfirm gcc
    elif command -v zypper &> /dev/null; then
        sudo zypper install -y gcc-c++
    else
        echo "Error: Could not detect package manager. Please install g++ manually."
        exit 1
    fi
    
    # verify installation
    if ! command -v $COMPILER &> /dev/null; then
        echo "Error: Failed to install $COMPILER"
        exit 1
    fi
    echo "$COMPILER installed successfully!"
fi

$COMPILER -I./$INCLUDE $MAIN_FILE -o bin/$PROJECT_NAME
./bin/$PROJECT_NAME
