#pragma once

#include "Board.h"
#include <iostream>

Board g_board;

void Print_ASKII_Art() {
    system("cls");
    std::cout << R"(
                                                                                             
                                                                                             
                   hhhhhhh                                                                   
                   h:::::h                                                                   
                   h:::::h                                                                   
                   h:::::h                                                                   
    cccccccccccccccch::::h hhhhh           eeeeeeeeeeee        ssssssssss       ssssssssss   
  cc:::::::::::::::ch::::hh:::::hhh      ee::::::::::::ee    ss::::::::::s    ss::::::::::s  
 c:::::::::::::::::ch::::::::::::::hh   e::::::eeeee:::::eess:::::::::::::s ss:::::::::::::s 
c:::::::cccccc:::::ch:::::::hhh::::::h e::::::e     e:::::es::::::ssss:::::ss::::::ssss:::::s
c::::::c     ccccccch::::::h   h::::::he:::::::eeeee::::::e s:::::s  ssssss  s:::::s  ssssss 
c:::::c             h:::::h     h:::::he:::::::::::::::::e    s::::::s         s::::::s      
c:::::c             h:::::h     h:::::he::::::eeeeeeeeeee        s::::::s         s::::::s   
c::::::c     ccccccch:::::h     h:::::he:::::::e           ssssss   s:::::s ssssss   s:::::s 
c:::::::cccccc:::::ch:::::h     h:::::he::::::::e          s:::::ssss::::::ss:::::ssss::::::s
 c:::::::::::::::::ch:::::h     h:::::h e::::::::eeeeeeee  s::::::::::::::s s::::::::::::::s 
  cc:::::::::::::::ch:::::h     h:::::h  ee:::::::::::::e   s:::::::::::ss   s:::::::::::ss  
    cccccccccccccccchhhhhhh     hhhhhhh    eeeeeeeeeeeeee    sssssssssss      sssssssssss    
                                                                                             
                                                                                             
                                                               
)";

}

void PrintInstructions() {
    std::cout << "\n--- Chess Game Instructions ---\n";
    std::cout << "1. Choose your color: 'w' for white, 'b' for black.\n";
    std::cout << "2. Enter moves in the format 'src dest' (e.g., e2 e4).\n";
    std::cout << "3. The game ends when there's a checkmate, stalemate, or lack of material.\n";
    std::cout << "4. At any point, type 'x' to continue to the main menu.\n";
    std::cout << "5. Have fun playing!\n\n";
}

int startChess() {
    std::string col;

    std::cout << "Pick your color (w / b): ";
    std::cin >> col;

    if (col == "x") return 0;

    g_board.setPlayingAsWhite(col[0] == 'w');

    uint8_t gameStatus;
    while ((gameStatus = g_board.gameOver()) == 0) {
        Print_ASKII_Art();
        std::cout << g_board << '\n';

        Move move;
        do {
            std::string plyMoveSrc, plyMoveDest;
            std::cout << (g_board.whitesTurn() ? "white's" : "black's") << " move (src dest): ";
            std::cin >> plyMoveSrc;

            if (plyMoveSrc == "x") return 0;

            std::cin >> plyMoveDest;

            if (plyMoveDest == "x") return 0;

            move = Move::getMove(plyMoveSrc, plyMoveDest);
            if (Move::outOfBounds(move)) {
                move.setIntention(255);
            }
            else {
                move.setIntention(g_board.findIntention(move));
            }

        } while (!g_board.isValidMove(move));

        g_board.movePiece(move);
    }

    switch (gameStatus) {
    case 1:
        std::cout << "Checkmate! " << (!g_board.whitesTurn() ? "white" : "black") << " won!\n";
        break;
    case 2:
        std::cout << "Stalemate! No moves left for " << (g_board.whitesTurn() ? "white" : "black") << '\n';
        break;
    case 3:
        std::cout << "Stalemate due to lack of material\n";
        break;
    default:
        std::cout << "Game ended because of unknown cause\n";
        break;
    }

    std::cout << g_board;
    return 0;
}

void displayMenuChess() {
    Print_ASKII_Art();
    int choice;
    do {
        std::cout << "\n--- Chess Game Menu ---\n";
        std::cout << "1. Start a new game\n";
        std::cout << "2. View instructions\n";
        std::cout << "3. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (std::cin.fail() || choice == 0) {
            std::cin.clear();
            std::cin.ignore();
            std::cout << "Invalid choice. Please try again.\n";
            continue;
        }

        switch (choice) {
        case 1:
            startChess();
            ;
        case 2:
            PrintInstructions();
            continue;
        case 3:
            std::cout << "Exiting the game. Goodbye!\n";
            continue;
        default:
            std::cout << "Invalid choice. Please try again.\n";
            continue;
        }
    } while (choice != 3);
}

