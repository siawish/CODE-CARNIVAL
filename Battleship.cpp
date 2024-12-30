#include "Battleship.h"


void Battleship::welcome_menu()
{
    system("CLS");
    std::cout << R"(

        -------------------------------------------------------
        |   ______       _   _   _            _     _         |
        |   | ___ \     | | | | | |          | |   (_)        |
        |   | |_/ / __ _| |_| |_| | ___   ___| |__  _ _ __    |
        |   | ___ \/ _` | __| __| |/ _ \ / __| '_ \| | '_ \   |
        |   | |_/ / (_| | |_| |_| |  __/ \__ \ | | | | |_) |  |
        |   \____/ \__,_|\__|\__|_|\___| |___/_| |_|_| .__/   |
        |                                            | |      |
        |                                            |_|      |
        -------------------------------------------------------

                         PLAY MULTIPLAYER [m]
                         PLAY SINGLE-PLAYER [s]
                         QUIT [q]
    )";

    std::cout << std::endl;
}

void Battleship::print_board(std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> grid) // Print the board of the selected player
{
    std::cout << "   ";

    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        // Convert to char the corresponding ASCII number
        std::cout << static_cast<char>('A' + i) << " ";
    }

    std::cout << std::endl;
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        std::cout << i << "  ";

        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            std::cout << grid[i][j] << " ";
        }
        std::cout << "\n";
    }
}

void Battleship::place_ship(std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE>& grid, int size, char mark)
{
    int row;
    int col;
    char direction; // horizontal - vertical
    bool is_placed{ false }; // Check for a ship to be placed

    // Keep prompting the user to insert a valid input
    while (!is_placed)
    {
        std::cout << "Select row (0 1 2 3 4 5 6 7 8 9) -> ";
        while (true)
        {
            std::cin >> row;
            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Enter a valid input for row (0 1 2 3 4 5 6 7 8 9) -> ";
            }
            else
            {
                break;
            }
        }

        std::cout << "Select col (0 1 2 3 4 5 6 7 8 9) -> ";

        // Keep prompting the user to insert a valid input
        while (true)
        {
            std::cin >> col;
            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Enter a valid input for col (0 1 2 3 4 5 6 7 8 9) -> ";
            }
            else
            {
                break;
            }
        }

        std::cout << "Select direction (h: horizontal - v: vertical) > ";
        std::cin >> direction;

        // Check valid horizontal placement
        if (direction == 'h' || direction == 'H')
        {
            if (col + size > BOARD_SIZE) // Check for board limits
            {
                std::cout << "Invalid placement: out of board limits" << std::endl;
            }
            else
            {
                bool overlap = false; // Check for overlapping ships

                for (int i = col; i < col + size; ++i)
                {
                    if (grid[row][i] != '~')
                    {
                        overlap = true;
                        break;
                    }
                }
                if (overlap)
                {
                    std::cout << "Invalid placement: overlapping existing ship\n";
                }
                else
                {
                    is_placed = true;
                    for (int i = col; i < col + size; ++i)
                    {
                        grid[row][i] = mark; // Placement succeeded
                    }
                }
            }
        }
        else if (direction == 'v' || direction == 'V') // Check valid vertical placement
        {
            if (row + size > BOARD_SIZE) // Check for board limits
            {
                std::cout << "Invalid placement: out of board limits" << std::endl;
            }
            else
            {
                bool overlap = false;

                for (int i = row; i < row + size; ++i)
                {
                    if (grid[i][col] != '~')
                    {
                        overlap = true;
                        break;
                    }
                }
                if (overlap)
                {
                    std::cout << "Invalid placement: overlapping existing ship\n";
                }
                else
                {
                    is_placed = true;
                    for (int i = row; i < row + size; ++i)
                    {
                        grid[i][col] = mark;
                    }
                }
            }
        }
        else
        {
            std::cout << "Invalid input: enter h or v" << std::endl;
        }
    }
}

void Battleship::strike(int turn, std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE>& enemy)
{
    int coordX;
    int coordY;

    std::cout << "Enter X coord. > ";

    // Keep prompting the user to insert the right value 
    while (true)
    {
        std::cin >> coordX; // Accept X coordinate

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Insert valid X cordinate > ";
            break;
        }
        else
        {
            break;
        }
    }

    std::cout << "Enter Y coord. > ";

    while (true)
    {
        std::cin >> coordY; // Accept Y coordinate

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Insert valid Y cordinate > ";
        }
        else
        {
            break;
        }
    }

    if (enemy[coordX][coordY] == '~') // Miss
    {
        std::cout << "You missed!" << std::endl;
        std::cout << "Player " << turn << " shot at coordinates ( " << coordX << "; " << coordX << " )" << std::endl;
        enemy[coordX][coordY] = 'O';
    }
    else if (enemy[coordX][coordY] == 'D' || enemy[coordX][coordY] == 'S' || enemy[coordX][coordY] == 'B'
        || enemy[coordX][coordY] == 'C') // Hit
    {
        std::cout << "Hit!" << std::endl;
        std::cout << "Player " << turn << " shot at coordinates ( " << coordX << "; " << coordX << " )" << std::endl;
        enemy[coordX][coordY] = 'X';
    }
    else // Invalid location
    {
        std::cout << "Invalid location: you already shot here!" << std::endl;
    }
}

int Battleship::random_turn(int initial, int last)
{
    std::random_device rd;  // Random device engine, usually based on /dev/random on UNIX-like systems
    std::mt19937_64 rng(rd());
    std::uniform_int_distribution<int> distribution(initial, last);
    return distribution(rng);  // Use rng as a generator
}

void  Battleship::single_player()
{
    std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> player_board;
    std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> ai_board;

    // Initialize player and AI boards
    for (auto& row : player_board) {
        for (auto& cell : row) {
            cell = '~';
        }
    }
    for (auto& row : ai_board) {
        for (auto& cell : row) {
            cell = '~';
        }
    }

    // Player places ships
    std::cout << "Player - place your ships" << std::endl;
    place_ship(player_board, DESTROYER_SIZE, 'D');
    place_ship(player_board, SUBMARINE_SIZE, 'S');
    place_ship(player_board, BATTLESHIP_SIZE, 'B');
    place_ship(player_board, CARRIER_SIZE, 'C');

    // AI randomly places ships
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, BOARD_SIZE - 1);

    auto ai_place_ship = [&](int size, char mark) {
        bool placed = false;
        while (!placed) {
            int row = dist(gen);
            int col = dist(gen);
            bool horizontal = dist(gen) % 2 == 0;

            if (horizontal && col + size <= BOARD_SIZE) {
                bool overlap = false;
                for (int i = 0; i < size; ++i) {
                    if (ai_board[row][col + i] != '~') {
                        overlap = true;
                        break;
                    }
                }
                if (!overlap) {
                    for (int i = 0; i < size; ++i) {
                        ai_board[row][col + i] = mark;
                    }
                    placed = true;
                }
            }
            else if (!horizontal && row + size <= BOARD_SIZE) {
                bool overlap = false;
                for (int i = 0; i < size; ++i) {
                    if (ai_board[row + i][col] != '~') {
                        overlap = true;
                        break;
                    }
                }
                if (!overlap) {
                    for (int i = 0; i < size; ++i) {
                        ai_board[row + i][col] = mark;
                    }
                    placed = true;
                }
            }
        }
        };

    ai_place_ship(DESTROYER_SIZE, 'D');
    ai_place_ship(SUBMARINE_SIZE, 'S');
    ai_place_ship(BATTLESHIP_SIZE, 'B');
    ai_place_ship(CARRIER_SIZE, 'C');

    // Game loop
    bool game_over = false;
    while (!game_over) {
        // Player turn
        std::cout << "Your turn!" << std::endl;
        print_board(player_board);
        strike(1, ai_board);

        // Check if AI ships are all sunk
        bool ai_ships_sunk = true;
        for (const auto& row : ai_board) {
            for (const auto& cell : row) {
                if (cell == 'D' || cell == 'S' || cell == 'B' || cell == 'C') {
                    ai_ships_sunk = false;
                    break;
                }
            }
            if (!ai_ships_sunk) break;
        }
        if (ai_ships_sunk) {
            std::cout << "You win!" << std::endl;
            game_over = true;
            break;
        }

        // AI turn
        std::cout << "AI's turn!" << std::endl;

        // AI randomly selects coordinates for attack
        int ai_coordX = random_turn(0, BOARD_SIZE - 1);
        int ai_coordY = random_turn(0, BOARD_SIZE - 1);

        // AI attacks with the random coordinates
        if (player_board[ai_coordX][ai_coordY] == 'D' || player_board[ai_coordX][ai_coordY] == 'S' ||
            player_board[ai_coordX][ai_coordY] == 'B' || player_board[ai_coordX][ai_coordY] == 'C') {
            std::cout << "AI hit at (" << ai_coordX << ", " << ai_coordY << ")!" << std::endl;
            player_board[ai_coordX][ai_coordY] = 'X';  // Mark the hit on the player's board
        }
        else {
            std::cout << "AI missed at (" << ai_coordX << ", " << ai_coordY << ")." << std::endl;
            player_board[ai_coordX][ai_coordY] = 'O';  // Mark the miss on the player's board
        }

        // Check if player ships are all sunk
        bool player_ships_sunk = true;
        for (const auto& row : player_board) {
            for (const auto& cell : row) {
                if (cell == 'D' || cell == 'S' || cell == 'B' || cell == 'C') {
                    player_ships_sunk = false;
                    break;
                }
            }
            if (!player_ships_sunk) break;
        }
        if (player_ships_sunk) {
            std::cout << "AI wins!" << std::endl;
            game_over = true;
        }
    }
}

char Battleship::play_quit()
{
    char answer;
    while (true)
    {
        std::cout << " > ";
        std::cin >> answer;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Enter 'm', 's', or 'q'";
        }
        else if (answer == 'm' || answer == 's' || answer == 'q')
        {
            break;
        }
        else
        {
            std::cout << "Invalid input. Enter 'm', 's', or 'q'";
        }
    }
    return answer;
}
