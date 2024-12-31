#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <cstdlib>
#include <ctime> // for srand
#include "DatabaseHandler.h"

using namespace std;

struct Node {
    int x, y;
    Node* next;
    Node(int x, int y) : x(x), y(y), next(nullptr) {}
};

class Snake_Game {
private:
    bool gameover;
    bool returnToMenu; // Flag to return to the Snake Game menu
    const int width = 50;
    const int height = 20;
    int x, y, fruitX, fruitY, score;
    Node* head; // Head of the snake
    Node* tail; // Tail of the snake

    int level;
    int userID;
    int gameID;
    DBHandler& dbHandler; // Reference to the database handler

    enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
    eDirection dir;

    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    void snakeGameLogo() {
        cout << R"(
  _________ _______      _____   ____  __ ___________   ________    _____      _____  ___________
 /   _____/ \      \    /  _  \ |    |/ _|\_   _____/  /  _____/   /  _  \    /     \ \_   _____/
 \_____  \  /   |   \  /  /_\  \|      <   |    __)_  /   \  ___  /  /_\  \  /  \ /  \ |    __)_ 
 /        \/    |    \/    |    \    |  \  |        \ \    \_\  \/    |    \/    Y    \|        \
/_______  /\____|__  /\____|__  /____|__ \/_______  /  \______  /\____|__  /\____|__  /_______  /        
        \/         \/         \/        \/        \/          \/         \/         \/        \/   
)";
    }

    void menu() {
        while (true) {
            system("CLS");
            snakeGameLogo();

            cout << "\nWelcome to the Snake Game, User " << userID << "!" << endl << endl;
            if (level > 1) {
                cout << "Your Previous Level: " << level << endl << endl;
            }

            cout << "Main Menu:\n";
            cout << "1. Start Game\n";
            cout << "2. Instructions\n";
            cout << "3. Back\n";

            cout << "Enter your choice (1-3): ";
            int choice;
            cin >> choice;

            switch (choice) {
            case 1:
                Setup();
                return; // Exit menu and start the game
            case 2:
                showInstructions();
                break; // Show instructions and return to the menu
            case 3:
                cout << "Returning to the main caller...\n";
                Sleep(1000);
                gameover = true; // Signal to exit the game instance
                return; // Exit the menu and return control to the caller
            default:
                cout << "Invalid choice! Please try again.\n";
                Sleep(1000);
                break;
            }
        }
    }

    void showInstructions() {
        system("CLS");
        cout << "Instructions:\n";
        cout << "1. Use 'W', 'A', 'S', 'D' to move the snake.\n";
        cout << "2. Eat '@' to grow and gain points.\n";
        cout << "3. Avoid colliding with walls or yourself.\n";
        cout << "4. Press 'X' to exit the game and return to the Snake Game menu.\n";

        cout << "Press any key to return to the main menu...\n";
        _getch();
    }

    void Setup() {
        gameover = false;
        returnToMenu = false; // Reset the return-to-menu flag
        dir = STOP;
        x = width / 2;
        y = height / 2;
        fruitX = rand() % width;
        fruitY = rand() % height;
        score = 0;

        head = new Node(x, y);
        tail = head;

        level = dbHandler.getCurrentLevel(userID, gameID);
    }

    void MoveTail(bool grow) {
        Node* newHead = new Node(x, y);
        newHead->next = head;
        head = newHead;

        if (!grow) { // Remove the tail only if not growing
            Node* temp = head;
            while (temp->next != tail)
                temp = temp->next;
            delete tail;
            tail = temp;
            tail->next = nullptr;
        }
    }

    void algorithm() {
        switch (dir) {
        case LEFT:  x--; break;
        case RIGHT: x++; break;
        case UP:    y--; break;
        case DOWN:  y++; break;
        default:    break;
        }

        // Wrap around edges
        if (x >= width) x = 0; else if (x < 0) x = width - 1;
        if (y >= height) y = 0; else if (y < 0) y = height - 1;

        // Check for collision with the snake's body
        Node* current = head->next;
        while (current) {
            if (current->x == x && current->y == y) {
                gameover = true;
                return;
            }
            current = current->next;
        }

        bool grow = false;

        // Check if the snake eats the fruit
        if (x == fruitX && y == fruitY) {
            score += 10;
            do {
                fruitX = rand() % width;
                fruitY = rand() % height;
            } while (isSnakeBody(fruitX, fruitY));
            grow = true;

            // Save score to the database
            dbHandler.addScore(userID, gameID, score, 0);

            // Increment level every 100 points
            if (score / 100 > level - 1) { // Check if the score threshold for the next level is reached
                level++;
                dbHandler.updateGameProgress(userID, gameID, level);
                cout << "Level Up! Current Level: " << level << endl;
                Sleep(1000); // Pause briefly to display the level-up message
            }
        }

        MoveTail(grow);
    }

    bool isSnakeBody(int x, int y) {
        Node* temp = head;
        while (temp) {
            if (temp->x == x && temp->y == y)
                return true;
            temp = temp->next;
        }
        return false;
    }

    void Cleanup() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        head = nullptr;
        tail = nullptr; // Safely reset the tail pointer
    }

    void Input() {
        if (_kbhit()) {
            eDirection prevDir = dir;
            switch (_getch()) {
            case 'a': if (prevDir != RIGHT) dir = LEFT; break;
            case 'd': if (prevDir != LEFT) dir = RIGHT; break;
            case 'w': if (prevDir != DOWN) dir = UP; break;
            case 's': if (prevDir != UP) dir = DOWN; break;
            case 'x': // Exit to the Snake Game menu
                gameover = true; // End the current game loop
                returnToMenu = true; // Set flag to return to the menu
                return; // Stop processing further input
            }
        }
    }

    void Draw() {
        MoveCursorTo(0, 0);
        SetColor(14);
        for (int i = 0; i < width + 2; i++) cout << "-";
        cout << endl;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (j == 0) cout << "|";
                if (i == y && j == x) cout << "O";
                else if (i == fruitY && j == fruitX) cout << "@";
                else if (isSnakeBody(j, i)) cout << "o";
                else cout << " ";
                if (j == width - 1) cout << "|";
            }
            cout << endl;
        }

        for (int i = 0; i < width + 2; i++) cout << "-";
        cout << endl;
        cout << "Score: " << score << endl;
    }

    void MoveCursorTo(int x, int y) {
        COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
        SetConsoleCursorPosition(console, coord);
    }

    void SetColor(int color) {
        SetConsoleTextAttribute(console, color);
    }

public:
    Snake_Game(int level, int userID, int gameID, DBHandler& dbHandler)
        : level(level), userID(userID), gameID(gameID), dbHandler(dbHandler) {
        srand(time(0)); // Initialize random seed

        while (true) {
            menu(); // Show the main menu

            if (gameover && !returnToMenu) break; // Exit the game instance if Back is chosen

            // Main game loop
            while (!gameover) {
                Draw();
                Input();
                if (returnToMenu) break; // Return to menu if `x` is pressed
                algorithm();
                Sleep(100); // Adjust speed for gameplay
            }

            Cleanup(); // Clean up resources after the game session

            if (returnToMenu) {
                returnToMenu = false; // Reset the flag for returning to the menu
            }
        }

        cout << "Thank you for playing Snake Game! Goodbye!" << endl;
    }

    ~Snake_Game() {
        Cleanup(); // Ensure resources are cleaned up
        cout << "Snake_Game instance destroyed.\n";
    }
};
