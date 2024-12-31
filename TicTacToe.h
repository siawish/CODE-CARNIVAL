#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <conio.h> // For _getch()

using namespace std;

class TicTacToe {
private:
    char board[3][3];
    int cursorX, cursorY; // Cursor position for WASD navigation
    char currentPlayer;
    bool singlePlayer;

public:
    TicTacToe() : cursorX(0), cursorY(0), currentPlayer('X'), singlePlayer(false) {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                board[i][j] = '-';
        run();
    }

    void displayBoard() {
        system("cls"); // Clear the console
        printTicTacToe();

        // Center the grid horizontally
        const int padding = 15;
        cout << string(padding, ' ') << "\n";

        for (int i = 0; i < 3; i++) {
            for (int line = 0; line < 3; line++) { // Each cell will have 3 lines
                cout << string(padding, ' '); // Add horizontal padding
                for (int j = 0; j < 3; j++) {
                    if (line == 1) { // Middle line for actual content
                        if (i == cursorY && j == cursorX)
                            cout << " | [" << board[i][j] << "] |"; // Highlight the cursor position
                        else
                            cout << " |  " << board[i][j] << "  |";
                    }
                    else {
                        cout << " |     |"; // Empty spaces
                    }
                }
                cout << "\n";
            }
            cout << string(padding, ' ') << "\n"; // Space between rows with padding
        }

        cout << string(padding, ' ') << "Use WASD to move, Enter to place your mark.\n";
    }

    void moveCursor(char key) {
        if (key == 'W' || key == 'w') cursorY = (cursorY - 1 + 3) % 3; // Move up
        if (key == 'S' || key == 's') cursorY = (cursorY + 1) % 3;     // Move down
        if (key == 'A' || key == 'a') cursorX = (cursorX - 1 + 3) % 3; // Move left
        if (key == 'D' || key == 'd') cursorX = (cursorX + 1) % 3;     // Move right
    }

    bool placeMark() {
        if (board[cursorY][cursorX] == '-') {
            board[cursorY][cursorX] = currentPlayer;
            return true;
        }
        return false;
    }

    void computerMove() {
        srand(time(0));
        int row, col;
        do {
            row = rand() % 3;
            col = rand() % 3;
        } while (board[row][col] != '-'); // Find an empty spot
        board[row][col] = 'O';
    }

    char checkWin() {
        // Check rows and columns
        for (int i = 0; i < 3; i++) {
            if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != '-')
                return board[i][0];
            if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != '-')
                return board[0][i];
        }
        // Check diagonals
        if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != '-')
            return board[0][0];
        if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != '-')
            return board[0][2];
        return '-';
    }

    bool isBoardFull() {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                if (board[i][j] == '-')
                    return false;
        return true;
    }
    void printTicTacToe() {
        system("CLS");
        cout << R"(
__/\\\\\\\\\\\\\\\__/\\\\\\\\\\\________/\\\\\\\\\____________/\\\\\\\\\\\\\\\_____/\\\\\\\\\___________/\\\\\\\\\____________/\\\\\\\\\\\\\\\_______/\\\\\_______/\\\\\\\\\\\\\\\_        
 _\///////\\\/////__\/////\\\///______/\\\////////____________\///////\\\/////____/\\\\\\\\\\\\\______/\\\////////____________\///////\\\/////______/\\\///\\\____\/\\\///////////__       
  _______\/\\\___________\/\\\_______/\\\/___________________________\/\\\________/\\\/////////\\\___/\\\/___________________________\/\\\_________/\\\/__\///\\\__\/\\\_____________      
   _______\/\\\___________\/\\\______/\\\_____________________________\/\\\_______\/\\\_______\/\\\__/\\\_____________________________\/\\\________/\\\______\//\\\_\/\\\\\\\\\\\_____     
    _______\/\\\___________\/\\\_____\/\\\_____________________________\/\\\_______\/\\\\\\\\\\\\\\\_\/\\\_____________________________\/\\\_______\/\\\_______\/\\\_\/\\\///////______    
     _______\/\\\___________\/\\\_____\//\\\____________________________\/\\\_______\/\\\/////////\\\_\//\\\____________________________\/\\\_______\//\\\______/\\\__\/\\\_____________   
      _______\/\\\___________\/\\\______\///\\\__________________________\/\\\_______\/\\\_______\/\\\__\///\\\__________________________\/\\\________\///\\\__/\\\____\/\\\_____________  
       _______\/\\\________/\\\\\\\\\\\____\////\\\\\\\\\_________________\/\\\_______\/\\\_______\/\\\____\////\\\\\\\\\_________________\/\\\__________\///\\\\\/_____\/\\\\\\\\\\\\\\\_ 
        _______\///________\///////////________\/////////__________________\///________\///________\///________\/////////__________________\///_____________\/////_______\///////////////__
)";
    }
    void run() {

        char choice;
        printTicTacToe();
        cout << "\n\t +--------------------------------------------------+      +--------------------------------------------------+       +--------------------------------------------------+"
            << "\n\t |                                                  |      |                                                  |       |                                                  |"
            << "\n\t |       1. SINGLE PLAYER (PLAY WITH BOT)           |      |            2. MULTIPLAYER                        |       |             3. EXIT.                             |"
            << "\n\t |                                                  |      |                                                  |       |                                                  |"
            << "\n\t +--------------------------------------------------+      +--------------------------------------------------+       +--------------------------------------------------+"

            << endl << endl;
        string input;
        cout << "\n << ";
        cin >> choice;



        if (choice == '3') {
            cout << "Exiting the game. Goodbye!\n";
            return;
        }

        singlePlayer = (choice == '1');

        while (true) {
            displayBoard();
            char winner = checkWin();
            if (winner != '-') {
                cout << winner << " wins!\n";
                break;
            }
            if (isBoardFull()) {
                cout << "It's a draw!\n";
                break;
            }

            if (currentPlayer == 'X') {
                // Player's turn
                char key;
                do {
                    displayBoard();
                    key = _getch();
                    if ((key == 'W' || key == 'A' || key == 'S' || key == 'D') ||
                        (key == 'w' || key == 'a' || key == 's' || key == 'd')) {
                        moveCursor(key);
                    }
                } while (key != '\r' || !placeMark()); // '\r' is Enter key
            }
            else {
                if (singlePlayer) {
                    // Computer's turn
                    computerMove();
                }
                else {
                    // Player 2's turn in multi-player
                    char key;
                    do {
                        displayBoard();
                        key = _getch();
                        if ((key == 'W' || key == 'A' || key == 'S' || key == 'D') ||
                            (key == 'w' || key == 'a' || key == 's' || key == 'd')) {
                            moveCursor(key);
                        }
                    } while (key != '\r' || !placeMark()); // '\r' is Enter key
                }
            }

            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        }
    }
};
