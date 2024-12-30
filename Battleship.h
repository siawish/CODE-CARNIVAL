#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include <array>
#include <string>
#include <limits>
#include <random>
#include <iostream>

using namespace std;

// Constants
const int BOARD_SIZE = 10;
const int DESTROYER_SIZE = 2;
const int SUBMARINE_SIZE = 3;
const int BATTLESHIP_SIZE = 4;
const int CARRIER_SIZE = 5;

class Battleship {
public:
    Battleship() {
        welcome_menu();
        char player_input = play_quit();

        // Ask the user to play or to quit the game
        if (player_input == 'q')
        {
            std::cout << "Bye!" << std::endl;
            return;
        }
        else if (player_input == 'm')
        {
            int turn = random_turn(1, 2); // Random turn generator
            bool start_game{ false }; // Game status

            // Player 1 board variable 
            std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> player1_board;
            // Player 2 board variable
            std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> player2_board;

            // Build the player1 board
            for (auto& row : player1_board)
            {
                for (auto& cell : row)
                {
                    cell = '~';
                }
            }

            // Build the player 2 board
            for (auto& row : player2_board)
            {
                for (auto& cell : row)
                {
                    cell = '~';
                }
            }


            std::cout << "Player 1 - place your ships" << std::endl;

            // Place destroyer
            std::cout << "First placement:  DESTROYER" << std::endl;
            place_ship(player1_board, DESTROYER_SIZE, 'D');
            std::cout << std::endl;

            // // Place submarine
            std::cout << "Second placement: SUBMARINE" << std::endl;
            place_ship(player1_board, SUBMARINE_SIZE, 'S');
            std::cout << std::endl;

            // Place battleship
            std::cout << "Third placement: BATTLESHIP " << std::endl;
            place_ship(player1_board, BATTLESHIP_SIZE, 'B');
            std::cout << std::endl;

            // Place carrier
            std::cout << "Third placement: BATTLESHIP " << std::endl;
            place_ship(player1_board, CARRIER_SIZE, 'C');
            std::cout << std::endl;

            print_board(player1_board);
            std::cout << std::endl;



            std::cout << "Player 2 - place your ships" << std::endl;
            // Place destroyer
            std::cout << "First placement:  DESTROYER" << std::endl;
            place_ship(player2_board, DESTROYER_SIZE, 'D');
            std::cout << std::endl;

            // Place submarine
            std::cout << "Second placement: SUBMARINE" << std::endl;
            place_ship(player2_board, SUBMARINE_SIZE, 'S');
            std::cout << std::endl;

            // Place battleship
            std::cout << "Third placement: BATTLESHIP " << std::endl;
            place_ship(player2_board, BATTLESHIP_SIZE, 'B');
            std::cout << std::endl;

            // Place carrier
            std::cout << "Third placement: BATTLESHIP " << std::endl;
            place_ship(player2_board, CARRIER_SIZE, 'C');
            std::cout << std::endl;

            print_board(player2_board);
            std::cout << std::endl;

            std::cout << std::endl;
            std::cout << "++++++++++++++++++++++++++" << std::endl;
            std::cout << "+ LET THE BATTLE BEGINS! +" << std::endl;
            std::cout << "++++++++++++++++++++++++++\n" << std::endl;

            // Start the game
            while (!start_game)
            {
                if (turn == 1)
                {
                    std::cout << "Player 1's turn!\n";
                    strike(1, player2_board);
                }
                else
                {
                    std::cout << "Player 2's turn!\n";
                    strike(2, player1_board);
                }

                bool ships_sunk{ true };
                // Check for all the ships to be sunk
                for (int i = 0; i < BOARD_SIZE; ++i)
                {
                    for (int j = 0; j < BOARD_SIZE; ++j)
                    {
                        if (turn == 1 && player2_board[i][j] == 'D' || player2_board[i][j] == 'S'
                            || player2_board[i][j] == 'B' || player2_board[i][j] == 'C')
                        {
                            ships_sunk = false;
                            break;
                        }
                        else if (turn == 2 && player1_board[i][j] == 'D')
                        {
                            ships_sunk = false;
                            break;
                        }
                    }
                }
                if (ships_sunk)
                {
                    std::cout << "GAME OVER! Player " << turn << " wins!" << std::endl;
                    start_game = true;
                }
                // Switch turn
                if (turn == 1)
                {
                    turn = 2;
                }
                else
                {
                    turn = 1;
                }
            }
        }
        else if (player_input == 's')  // Single-player option
        {
            std::cout << "Starting single-player game..." << std::endl;
            single_player();  // Call the single-player function
        }
    }

    char play_quit();
    void welcome_menu();
    void print_board(const std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> grid);
    void place_ship(std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE>& grid, int size, char mark);
    void strike(int turn, std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE>& enemy);
    int random_turn(int min, int max);
    void single_player();

private:
    std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> playerBoard;
    std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> aiBoard;


};

#endif // BATTLESHIP_H
