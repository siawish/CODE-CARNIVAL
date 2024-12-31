#pragma once
#include<iostream>
#include"DatabaseHandler.h"
#include<string>


#include <thread>
#include <chrono>
#include <iomanip> // for std::setw
#include<math.h>
#include<Windows.h>
#include <algorithm> // For std::max
#include <vector> 
#include"TicTacToe.h"
#include "Battleship.h"
#include"The_Maze.h"
#include"Snake_Game.h"
#include"callChess.h"




using namespace std;

void setColor(int colorCode) {
    cout << "\033[1;" << colorCode << "m";
}

void welcome(const string& username) {
    // Clear screen with spacing for effect
    cout << string(5, '\n');

    string message = "Welcome to Code Carnival, " + username + "!";

    // Begin carnival experience
    setColor(33); // Yellow for festive touch
    cout << "\t\t\t\t-- The Code Carnival is starting! --" << endl;
    this_thread::sleep_for(chrono::milliseconds(1000));

    // Typing animation for the welcome message
    cout << "\t\t\tWelcome message: ";
    this_thread::sleep_for(chrono::milliseconds(500));

    for (char c : message) {
        setColor(34); // Blue for carnival text
        cout << c;
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(150));
    }

    // Add a pause for dramatic effect
    cout << endl;
    this_thread::sleep_for(chrono::milliseconds(300));

    // Add festive flair with colorful messages
    setColor(35); // Magenta for excitement
    cout << "\n\t\t\t\t  ---- The fun begins now, " << username << "! ----" << endl;
    this_thread::sleep_for(chrono::milliseconds(500));

    setColor(32); // Green for lively touch
    cout << "\t\t\t-- Get ready to play and have fun at the Carnival! --" << endl;
    this_thread::sleep_for(chrono::milliseconds(500));

    setColor(31); // Red for dramatic effect
    cout << "\n\t\t\t-- The greatest game show on Earth is here! --" << endl;
    this_thread::sleep_for(chrono::milliseconds(500));

    // Reset color to default
    setColor(0);

    // Prompt to continue
    cout << "\n\nPress any key to continue...";
    _getch();

    // Clear screen after message (optional)
    cout << string(50, '\n');
}
// Assuming getAllGames returns a vector of pairs




void printAllGames(DBHandler DB) {
    auto games = DB.getAllGames();

    // Check if games list is empty
    if (games.empty()) {
        std::cout << "No games found in the database." << std::endl;
        return;
    }

    // Variables to determine column widths
    size_t maxIDWidth = 2;             // Minimum width for "ID"
    size_t maxNameWidth = 4;           // Minimum width for "Name"
    size_t maxDescriptionWidth = 11;  // Minimum width for "Description"
    size_t maxLevelsWidth = 10;       // Minimum width for "Max Levels"

    // Manually calculate maximum width for each column
    for (const auto& game : games) {
        int gameID = game.first.first;
        std::string gameName = game.first.second;
        std::string description = game.second.first;
        int maxLevels = game.second.second;

        // Update max widths if the current string length is greater
        if (std::to_string(gameID).length() > maxIDWidth) {
            maxIDWidth = std::to_string(gameID).length();
        }
        if (gameName.length() > maxNameWidth) {
            maxNameWidth = gameName.length();
        }
        if (description.length() > maxDescriptionWidth) {
            maxDescriptionWidth = description.length();
        }
        if (std::to_string(maxLevels).length() > maxLevelsWidth) {
            maxLevelsWidth = std::to_string(maxLevels).length();
        }
    }

    // Add padding for readability
    maxIDWidth += 2;
    maxNameWidth += 2;
    maxDescriptionWidth += 2;
    maxLevelsWidth += 2;

    // Print the top border of the table
    std::cout << "+" << std::string(maxIDWidth, '-') << "+"
        << std::string(maxNameWidth, '-') << "+"
        << std::string(maxDescriptionWidth, '-') << "+"
        << std::string(maxLevelsWidth, '-') << "+" << std::endl;

    // Print the header row
    std::cout << "| " << std::setw(maxIDWidth - 1) << std::left << "ID" << "| "
        << std::setw(maxNameWidth - 1) << std::left << "Name" << "| "
        << std::setw(maxDescriptionWidth - 1) << std::left << "Description" << "| "
        << std::setw(maxLevelsWidth - 1) << std::left << "Max Levels" << "|" << std::endl;

    // Print the separator border
    std::cout << "+" << std::string(maxIDWidth, '-') << "+"
        << std::string(maxNameWidth, '-') << "+"
        << std::string(maxDescriptionWidth, '-') << "+"
        << std::string(maxLevelsWidth, '-') << "+" << std::endl;

    // Print each row with details
    for (const auto& game : games) {
        int gameID = game.first.first;
        std::string gameName = game.first.second;
        std::string description = game.second.first;
        int maxLevels = game.second.second;

        // Split the description at the first period
        size_t periodPos = description.find('.');
        std::string beforePeriod = description.substr(0, periodPos + 1);
        std::string afterPeriod = (periodPos != std::string::npos) ? description.substr(periodPos + 1) : "";

        // Print the row
        std::cout << "| " << std::setw(maxIDWidth - 1) << std::left << gameID << "| "
            << std::setw(maxNameWidth - 1) << std::left << gameName << "| ";

        // Print description with color for the text after the period
        std::cout << beforePeriod;
        setColor(34); // Set color to blue
        std::cout << afterPeriod;
        setColor(0); // Reset color
        std::cout << std::setw(maxDescriptionWidth - beforePeriod.length() - afterPeriod.length() - 1) << " " << "| "
            << std::setw(maxLevelsWidth - 1) << std::left << maxLevels << "|" << std::endl;
    }

    // Print the bottom border of the table
    std::cout << "+" << std::string(maxIDWidth, '-') << "+"
        << std::string(maxNameWidth, '-') << "+"
        << std::string(maxDescriptionWidth, '-') << "+"
        << std::string(maxLevelsWidth, '-') << "+" << std::endl;
}

void printGameID(DBHandler DB) {
    auto games = DB.getAllGames();

    // Check if games list is empty
    if (games.empty()) {
        std::cout << "No games found in the database." << std::endl;
        return;
    }

    // Print the game details
    std::cout << "Games List:" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "ID\tName\t\tMax Levels" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    for (const auto& game : games) {
        int gameID = game.first.first;
        std::string gameName = game.first.second;

        int maxLevels = game.second.second;

        std::cout << gameID << "\t"
            << gameName << "\t\t"

            << maxLevels << std::endl;
    }
}



void displayPlayerMenu(string username) {

    system("cls");
    welcome(username);
    system("cls");
    cout << endl;


    cout << endl;
    int choice;
    do {

        cout << "\n Press Any Key To Continue...";
        _getch();

        system("cls");
        cout << "\n"
            << "\t\t\t+--------------------------------------------------+ " << endl;
        cout << "\t\t\t|                  PLAYER MENU                     |" << endl;
        cout << "\t\t\t+--------------------------------------------------+" << endl;
        cout << "\t\t\t| 1. View Available Games                          |" << endl;
        cout << "\t\t\t+--------------------------------------------------+" << endl;
        cout << "\t\t\t| 2. View My Progress                              |" << endl;
        cout << "\t\t\t+--------------------------------------------------+" << endl;
        cout << "\t\t\t| 3. Start Playing a Game                          |" << endl;
        cout << "\t\t\t+--------------------------------------------------+" << endl;
        cout << "\t\t\t| 4. View My Scores and Rewards                    |" << endl;
        cout << "\t\t\t+--------------------------------------------------+" << endl;
        cout << "\t\t\t| 0. Exit                                          |" << endl;
        cout << "\t\t\t+--------------------------------------------------+" << endl;
        cout << "\t\t\tEnter your choice: ";
        cin >> choice;




        DBHandler DB;
        // Get the player's choice
        int userId = DB.getUserIdByUsername(username);

        cin.ignore(); // To handle any leftover newline characters in the input buffer

        int gameID, currentLevel, score, timeSpent;
        string rewardName;


        if (choice == 1)
        {
            printAllGames(DB);

        }




        else if (choice == 2) // View My Progress
        {
            printGameID(DB);
            cout << "Enter game ID to view progress: ";
            cin >> gameID;

            string progress = DB.getProgressByUserAndGame(userId, gameID);
            cout << "Your Progress: " << progress << endl;
        }


        else if (choice == 3) // Start Playing a Game
        {
            // Display all available games
            printAllGames(DB);

            // Get user input for game ID
            int gameID;
            std::cout << "Enter game ID to start playing: ";
            std::cin >> gameID;

            // Retrieve game name by ID
           /* std::string gameName = DB.getNamebyGameId(gameID);
            if (gameName.empty()) {
                std::cout << "Invalid Game ID. Please try again." << std::endl;
                _getch();
                continue;
            }

            std::cout << "Starting game: " << gameName << "..." << std::endl;*/
            if (choice == 0) // Exit
            {
                cout << "Goodbye, " << username << "!" << endl;
                cout << "\n\n  Press Any Key To Continue...";
                _getch();

            }
            // Launch the selected game
            else if (gameID == 12) {
                std::cout << "Starting game: Maze Runner ..." << std::endl;
                cout << "\n Press Any Key To Continue...";
                _getch();

                The_Maze mazeRunnerGame(userId, gameID);
                /* mazeRunnerGame.play();*/ // Assuming a play() method in the constructor
            }
            else if (gameID == 3) {
                cout << "\n starting snake game ";
                _getch();
                int currentLevel = DB.getCurrentLevel(userId, gameID);
                Snake_Game snakeGame(currentLevel, userId, gameID, DB);

            }
            else if (gameID == 4) {
                displayMenuChess();
                cout << "\n Press Any Key To Contine...";
                char c = _getch();
            }
            else if (gameID == 8) {
                TicTacToe ticTacToeGame;
            }

            else if (gameID == 10) {
                Battleship battleShipGame;
                //battleShipGame.play();
            }
            else if (gameID == 11) {
                /*  FlappyBird flappyBirdGame;
                  flappyBirdGame.play();*/
                cout << "\n Game not yet implemented.";
            }
            else {
                std::cout << "Game not yet implemented OR invalid entry ." << std::endl;
                break;
            }


        }



        else if (choice == 4) // View My Scores and Rewards
        {
            vector<string> scores = DB.getScoresByGame(userId);
            cout << "Your Scores and Rewards:" << endl;
            for (const auto& score : scores) {
                cout << score << endl;
            }

            vector<string> rewards = DB.getRewardsByUser(userId);
            cout << "Your Rewards:" << endl;
            for (const auto& reward : rewards) {
                cout << reward << endl;
            }
        }
        else
        {
            cout << "Invalid choice, please try again!" << endl;
        }

    } while (choice != 0);
}


