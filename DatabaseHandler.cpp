#include "DatabaseHandler.h"


using namespace std;
#include<windows.h>
//_CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
// Destructor


// Connect to the database
bool DBHandler::connect() {
    // Allocate environment handle
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle)) {
        std::cerr << "Unable to allocate environment handle" << std::endl;
        return false;
    }

    // Set ODBC version
    if (SQL_SUCCESS != SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0)) {
        std::cerr << "Unable to set environment attribute" << std::endl;
        return false;
    }

    // Allocate connection handle
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle)) {
        std::cerr << "Unable to allocate connection handle" << std::endl;
        return false;
    }

    // Connect to the database
    SQLWCHAR retConString[1024]; // Wide-character buffer for return connection string
    retCode = SQLDriverConnect(
        sqlConnHandle,
        NULL,
        (SQLWCHAR*)connectionString.c_str(), // Pass as SQLWCHAR
        SQL_NTS,
        retConString,
        1024,
        NULL,
        SQL_DRIVER_NOPROMPT);

    if (SQL_SUCCESS != retCode && SQL_SUCCESS_WITH_INFO != retCode) {
        std::cerr << "Error connecting to the database" << std::endl;
        SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
        SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);
        sqlConnHandle = NULL;
        sqlEnvHandle = NULL;
        return false;
    }

    //std::cout << "Connected to the database successfully!" << std::endl;
    return true;
}

// Disconnect from the database
void DBHandler::disconnect() {
    if (sqlStmtHandle != SQL_NULL_HSTMT) {
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        sqlStmtHandle = NULL;
    }
    if (sqlConnHandle != SQL_NULL_HDBC) {
        SQLDisconnect(sqlConnHandle);
        SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
        sqlConnHandle = NULL;
    }
    if (sqlEnvHandle != SQL_NULL_HENV) {
        SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);
        sqlEnvHandle = NULL;
    }
    std::cout << "Disconnected from the database successfully!" << std::endl;
}



// Check if the connection is still active
bool DBHandler::isConnected() {
    return sqlConnHandle != NULL;
}
#include <algorithm>

// Function to trim leading and trailing whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');

    if (first == std::string::npos || last == std::string::npos)
        return "";
    return str.substr(first, (last - first + 1));
}

bool DBHandler::addUser(const std::string& username, const std::string& email, const std::string& password) {
    // Ensure the database connection is valid
    if (sqlConnHandle == SQL_NULL_HDBC) {
        std::cerr << "Database connection not initialized." << std::endl;
        return false;
    }

    // Free any existing statement handle
    if (sqlStmtHandle != SQL_NULL_HSTMT) {
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        sqlStmtHandle = SQL_NULL_HSTMT;
    }

    // Allocate a new statement handle
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle)) {
        std::cerr << "Error allocating statement handle." << std::endl;
        displayError(SQL_HANDLE_DBC, sqlConnHandle);
        return false;
    }

    // Trim the input to remove leading/trailing spaces
    std::string trimmedUsername = trim(username);
    std::string trimmedEmail = trim(email);
    std::string trimmedPassword = trim(password);

    // Validate input fields
    if (trimmedUsername.empty() || trimmedEmail.empty() || trimmedPassword.empty()) {
        std::cerr << "Error: All fields (username, email, password) must be provided." << std::endl;
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }

    // Generate a random salt and hash the password
    std::string salt = Hashing::generateSalt();  // Generate a random salt
    std::string passwordHash = Hashing::hashWithSalt(trimmedPassword, salt);  // Hash the password with the salt

    // Prepare the SQL query using parameterized statements
    std::wstring query = L"INSERT INTO Users (Username, Email, PasswordHash, CreatedAt, Salt) VALUES (?, ?, ?, GETDATE(), ?)";
    if (SQL_SUCCESS != SQLPrepare(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        std::cerr << "Error preparing SQL statement." << std::endl;
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }

    // Convert input strings to wide-character for SQL binding
    SQLWCHAR usernameParam[51], emailParam[101], passwordHashParam[256], saltParam[51];
    std::mbstowcs(usernameParam, trimmedUsername.c_str(), trimmedUsername.size() + 1);
    std::mbstowcs(emailParam, trimmedEmail.c_str(), trimmedEmail.size() + 1);
    std::mbstowcs(passwordHashParam, passwordHash.c_str(), passwordHash.size() + 1);
    std::mbstowcs(saltParam, salt.c_str(), salt.size() + 1);

    // Bind parameters to the SQL statement
    if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 50, 0, usernameParam, 0, NULL) ||
        SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0, emailParam, 0, NULL) ||
        SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 255, 0, passwordHashParam, 0, NULL) ||
        SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 50, 0, saltParam, 0, NULL)) {
        std::cerr << "Error binding SQL parameters." << std::endl;
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }

    // Execute the SQL statement
    if (SQL_SUCCESS != SQLExecute(sqlStmtHandle)) {
        std::cerr << "Error executing SQL statement." << std::endl;
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }

    // Clean up the statement handle
    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    sqlStmtHandle = SQL_NULL_HSTMT;

    std::cout << "User added successfully!" << std::endl;
    return true;
}



std::string DBHandler::getUserByUsername(const std::string& username) {
    // Allocate statement handle
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle)) {
        std::cerr << "Error allocating statement handle" << std::endl;
        return "";
    }

    // Prepare the query using parameterized SQL to prevent SQL injection
    std::wstring query = L"SELECT Username FROM Users WHERE Username = ?";

    if (SQL_SUCCESS != SQLPrepare(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return "";
    }

    // Bind the parameter
    SQLWCHAR usernameParam[50];
    std::mbstowcs(usernameParam, username.c_str(), 50); // Convert std::string to wide string
    if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 50, 0, usernameParam, 0, NULL)) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return "";
    }

    // Execute the query
    if (SQL_SUCCESS != SQLExecute(sqlStmtHandle)) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return "";
    }

    // Fetch the result
    SQLCHAR buffer[256];
    if (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
        SQLGetData(sqlStmtHandle, 1, SQL_C_CHAR, buffer, sizeof(buffer), NULL); // Retrieve 'Username' (column index 1)
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return std::string((char*)buffer);
    }
    else {
        std::cerr << "No matching user found" << std::endl;
    }

    // Free the statement handle
    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    return "";
}

//bool DBHandler::validateUser(const std::string& username, const std::string& passwordHash) {
//    std::wstring query = L"SELECT PasswordHash FROM Users WHERE Username = '" + std::wstring(username.begin(), username.end()) + L"'";
//
//    // Execute the query
//    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
//        cout << "\n yeh wala error ";
//        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
//        return false;
//    }
//
//    SQLCHAR buffer[256];  // Buffer to store the password hash
//    if (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
//        // Retrieve the stored password hash
//        SQLGetData(sqlStmtHandle, 1, SQL_C_CHAR, buffer, sizeof(buffer), NULL);
//        std::string storedPasswordHash((char*)buffer);  // Convert to std::string
//
//        // Trim any leading/trailing whitespaces
//        storedPasswordHash = trim(storedPasswordHash);
//        std::string enteredPasswordHash = trim(passwordHash);  // Trim the entered password hash
//
//        // Debugging: Print both the stored and entered password hashes
//        std::cout << "Validating user: " << username << std::endl;
//        std::cout << "Stored Password Hash: [" << storedPasswordHash << "]" << std::endl;
//        std::cout << "Entered Password Hash: [" << enteredPasswordHash << "]" << std::endl;
//
//        // Compare the hashes
//        return storedPasswordHash == enteredPasswordHash;
//    }
//
//    // If no user found with the provided username
//    std::cout << "User not found: " << username << std::endl;
//    return false;
//}
bool DBHandler::validateUser(const std::string& username, const std::string& enteredPassword) {
    SQLHSTMT stmtHandle = SQL_NULL_HSTMT; // Use a local statement handle for this query

    // Prepare the SQL query to retrieve PasswordHash and Salt
    std::wstring query = L"SELECT PasswordHash, Salt FROM Users WHERE Username = ?";

    // Allocate a statement handle
    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &stmtHandle)) {
        displayError(SQL_HANDLE_DBC, sqlConnHandle);
        return false;
    }

    // Convert the username to SQLWCHAR
    SQLWCHAR usernameParam[256];
    std::mbstowcs(usernameParam, username.c_str(), username.size() + 1);

    // Prepare the SQL statement
    if (SQL_SUCCESS != SQLPrepare(stmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        displayError(SQL_HANDLE_STMT, stmtHandle);
        SQLFreeHandle(SQL_HANDLE_STMT, stmtHandle);
        return false;
    }

    // Bind the username parameter
    if (SQL_SUCCESS != SQLBindParameter(stmtHandle, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 255, 0, usernameParam, 0, NULL)) {
        displayError(SQL_HANDLE_STMT, stmtHandle);
        SQLFreeHandle(SQL_HANDLE_STMT, stmtHandle);
        return false;
    }

    // Execute the query
    if (SQL_SUCCESS != SQLExecute(stmtHandle)) {
        displayError(SQL_HANDLE_STMT, stmtHandle);
        SQLFreeHandle(SQL_HANDLE_STMT, stmtHandle);
        return false;
    }

    // Retrieve the stored PasswordHash and Salt
    SQLCHAR passwordHashBuffer[256] = { 0 };
    SQLCHAR saltBuffer[51] = { 0 };

    if (SQLFetch(stmtHandle) == SQL_SUCCESS) {
        // Get PasswordHash and Salt
        if (SQL_SUCCESS != SQLGetData(stmtHandle, 1, SQL_C_CHAR, passwordHashBuffer, sizeof(passwordHashBuffer), NULL) ||
            SQL_SUCCESS != SQLGetData(stmtHandle, 2, SQL_C_CHAR, saltBuffer, sizeof(saltBuffer), NULL)) {
            displayError(SQL_HANDLE_STMT, stmtHandle);
            SQLFreeHandle(SQL_HANDLE_STMT, stmtHandle);
            return false;
        }

        // Convert retrieved data to std::string
        std::string storedPasswordHash(reinterpret_cast<char*>(passwordHashBuffer));
        std::string storedSalt(reinterpret_cast<char*>(saltBuffer));

        // Rehash the entered password using the retrieved salt
        std::string computedHash = Hashing::hashWithSalt(enteredPassword, storedSalt);

        // Free the statement handle
        SQLFreeHandle(SQL_HANDLE_STMT, stmtHandle);

        // Compare the computed hash with the stored hash
        if (computedHash == storedPasswordHash) {
            std::cout << "User validated successfully!" << std::endl;
            return true;
        }
        else {
            std::cerr << "Invalid credentials." << std::endl;
            return false;
        }
    }

    // Handle case where no matching user is found
    SQLFreeHandle(SQL_HANDLE_STMT, stmtHandle);
    std::cerr << "User not found." << std::endl;
    return false;
}





// starting froom here 

// Utility to execute a general query
bool DBHandler::executeQuery(const std::string& query) {
    std::wstring wideQuery(query.begin(), query.end());
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)wideQuery.c_str(), SQL_NTS)) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }
    return true;
}

// Fetch results from the executed query
std::vector<std::string> DBHandler::fetchResults() {
    std::vector<std::string> results;
    SQLCHAR buffer[256];
    while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
        SQLGetData(sqlStmtHandle, 1, SQL_C_CHAR, buffer, sizeof(buffer), NULL);
        results.push_back(std::string((char*)buffer));
    }
    return results;
}

// Display detailed error messages
void DBHandler::displayError(SQLSMALLINT handleType, SQLHANDLE handle) {
    SQLWCHAR sqlState[1024];
    SQLWCHAR message[1024];
    if (SQL_SUCCESS == SQLGetDiagRec(handleType, handle, 1, sqlState, NULL, message, 1024, NULL)) {
        std::wcerr << "SQL Error [" << sqlState << "] : " << message << std::endl;
    }
}
// yahan tak
bool DBHandler::addGame(const std::string& gameName, const std::string& description, int maxLevels) {
    // Prepare the SQL query using parameterized statements
    std::wstring query = L"INSERT INTO Games (GameName, Description, MaxLevels) VALUES (?, ?, ?)";

    // Allocate statement handle if not already allocated
    if (sqlStmtHandle == SQL_NULL_HSTMT) {
        if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle)) {
            std::cerr << "Error allocating statement handle" << std::endl;
            return false;
        }
    }

    // Prepare the query for execution
    if (SQL_SUCCESS != SQLPrepare(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }

    // Convert input strings to wide-character format
    SQLWCHAR gameNameParam[101], descriptionParam[501];
    std::mbstowcs(gameNameParam, gameName.c_str(), gameName.size() + 1);
    std::mbstowcs(descriptionParam, description.c_str(), description.size() + 1);

    // Bind the parameters
    SQLBindParameter(sqlStmtHandle, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 100, 0, gameNameParam, 0, NULL);
    SQLBindParameter(sqlStmtHandle, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 500, 0, descriptionParam, 0, NULL);
    SQLBindParameter(sqlStmtHandle, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &maxLevels, 0, NULL);

    // Execute the query
    if (SQL_SUCCESS != SQLExecute(sqlStmtHandle)) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }

    std::cout << "Game added successfully!" << std::endl;
    return true;
}


std::vector<std::pair<std::pair<int, std::string>, std::pair<std::string, int>>> DBHandler::getAllGames() {
    // Vector to store game details
    std::vector<std::pair<std::pair<int, std::string>, std::pair<std::string, int>>> games;

    // Query to get game details
    std::string query = "SELECT GameID, GameName, Description, MaxLevels FROM Games";

    // Allocate statement handle if not already allocated
    if (sqlStmtHandle == SQL_NULL_HSTMT) {
        if (SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle) != SQL_SUCCESS) {
            std::cerr << "Error allocating statement handle." << std::endl;
            return games; // Return empty vector if allocation fails
        }
    }

    // Free the previous statement state
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);

    // Execute the query
    if (SQLExecDirectA(sqlStmtHandle, (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS) {
        std::cerr << "Error executing SQL query." << std::endl;
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return games; // Return empty vector if query fails
    }

    // Buffers to hold the data
    SQLINTEGER gameID;
    SQLCHAR gameName[256];
    SQLCHAR gameDescription[1024];
    SQLINTEGER maxLevels;

    // Fetch results
    while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
        SQLGetData(sqlStmtHandle, 1, SQL_C_SLONG, &gameID, 0, NULL); // Get GameID
        SQLGetData(sqlStmtHandle, 2, SQL_C_CHAR, gameName, sizeof(gameName), NULL); // Get GameName
        SQLGetData(sqlStmtHandle, 3, SQL_C_CHAR, gameDescription, sizeof(gameDescription), NULL); // Get Description
        SQLGetData(sqlStmtHandle, 4, SQL_C_SLONG, &maxLevels, 0, NULL); // Get MaxLevels

        // Add the data to the vector
        games.emplace_back(
            std::make_pair(
                std::make_pair(gameID, std::string((char*)gameName)),
                std::make_pair(std::string((char*)gameDescription), maxLevels)
            )
        );
    }

    return games; // Return the list of game details
}

//std::vector<std::pair<std::pair<int, std::string>, std::pair<std::string, int>>> DBHandler::getAllGames() {
//    // Vector to store game details
//    std::vector<std::pair<std::pair<int, std::string>, std::pair<std::string, int>>> games;
//
//    // Query to get game details
//    std::string query = "SELECT (*) FROM Games";
//
//    // Check if statement handle is valid, allocate if necessary
//    if (sqlStmtHandle == SQL_NULL_HSTMT) {
//        if (SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle) != SQL_SUCCESS) {
//            std::cerr << "Error allocating statement handle." << std::endl;
//            return games; // Return empty vector if allocation fails
//        }
//    }
//
//    // Reset the statement state
//    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);
//
//    // Execute the query
//    if (SQLExecDirectA(sqlStmtHandle, (SQLCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS) {
//        std::cerr << "Error executing SQL query: " << query << std::endl;
//        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
//        return games; // Return empty vector if query fails
//    }
//
//    // Buffers to hold retrieved data
//    SQLINTEGER gameID;
//    SQLCHAR gameName[256];
//    SQLCHAR gameDescription[1024];
//    SQLINTEGER maxLevels;
//
//    // Fetch and process results
//    while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
//        // Retrieve each column's data
//        SQLGetData(sqlStmtHandle, 1, SQL_C_SLONG, &gameID, 0, NULL); // GameID
//        SQLGetData(sqlStmtHandle, 2, SQL_C_CHAR, gameName, sizeof(gameName), NULL); // GameName
//        SQLGetData(sqlStmtHandle, 3, SQL_C_CHAR, gameDescription, sizeof(gameDescription), NULL); // Description
//        SQLGetData(sqlStmtHandle, 4, SQL_C_SLONG, &maxLevels, 0, NULL); // MaxLevels
//
//        // Add the data to the vector
//        games.emplace_back(
//            std::make_pair(
//                std::make_pair(gameID, std::string(reinterpret_cast<char*>(gameName))),
//                std::make_pair(std::string(reinterpret_cast<char*>(gameDescription)), maxLevels)
//            )
//        );
//    }
//
//    // Return the list of game details
//    return games;
//}





std::string DBHandler::getGameByID(int gameID) {
    // Ensure the statement handle is allocated
    if (sqlStmtHandle == SQL_NULL_HSTMT) {
        if (SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle) != SQL_SUCCESS) {
            std::cerr << "Error allocating statement handle." << std::endl;
            return "";
        }
    }

    // Free previous statement state
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);

    // Construct the query with parameter
    std::wstring query = L"SELECT GameName FROM Games WHERE GameID = " + std::to_wstring(gameID);

    // Execute the query
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        std::cerr << "Error executing SQL query." << std::endl;
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return "";
    }

    // Buffer to store the result
    SQLCHAR buffer[256] = { 0 };

    // Fetch the result
    if (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
        SQLGetData(sqlStmtHandle, 1, SQL_C_CHAR, buffer, sizeof(buffer), NULL);
        return std::string((char*)buffer);  // Return the game name
    }
    return "";  // Return empty string if no result found
}


bool DBHandler::updateMVP(int gameID, int userID) {
    // Ensure the statement handle is allocated
    if (sqlStmtHandle == SQL_NULL_HSTMT) {
        if (SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle) != SQL_SUCCESS) {
            std::cerr << "Error allocating statement handle." << std::endl;
            return false;
        }
    }

    // Free previous statement state
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);

    // Construct the query
    std::wstring query = L"UPDATE Games SET MVPUserID = " + std::to_wstring(userID) +
        L" WHERE GameID = " + std::to_wstring(gameID);

    // Execute the query
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        std::cerr << "Error executing SQL query." << std::endl;
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }

    std::cout << "MVP updated successfully!" << std::endl;
    return true;
}


bool DBHandler::addLevel(int gameID, int levelNumber, const std::string& levelDescription) {
    // Ensure the statement handle is allocated
    if (sqlStmtHandle == SQL_NULL_HSTMT) {
        if (SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle) != SQL_SUCCESS) {
            std::cerr << "Error allocating statement handle." << std::endl;
            return false;
        }
    }

    // Free previous statement state
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);

    // Prepare the SQL query using parameters (avoids unsafe string concatenation)
    std::wstring query = L"INSERT INTO Levels (GameID, LevelNumber, LevelDescription) VALUES (?, ?, ?)";

    if (SQLPrepare(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }

    // Convert levelDescription to wide-character
    SQLWCHAR levelDescParam[501];
    std::mbstowcs(levelDescParam, levelDescription.c_str(), sizeof(levelDescParam) / sizeof(SQLWCHAR));

    // Bind parameters to the query
    SQLBindParameter(sqlStmtHandle, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &gameID, 0, NULL);
    SQLBindParameter(sqlStmtHandle, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &levelNumber, 0, NULL);
    SQLBindParameter(sqlStmtHandle, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, 500, 0, levelDescParam, 0, NULL);

    // Execute the query
    if (SQLExecute(sqlStmtHandle) != SQL_SUCCESS) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }

    std::cout << "Level added successfully!" << std::endl;
    return true;
}



std::vector<std::string> DBHandler::getLevelsByGameID(int gameID) {
    // Allocate the statement handle first
    if (SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle) != SQL_SUCCESS &&
        SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle) != SQL_SUCCESS_WITH_INFO) {
        std::cerr << "Error allocating statement handle." << std::endl;
        displayError(SQL_HANDLE_DBC, sqlConnHandle);
        return {};
    }

    // Prepare the query
    std::wstring query = L"SELECT LevelNumber FROM Levels WHERE GameID = " + std::to_wstring(gameID);

    // Execute the query
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return {};
    }

    // Fetch and return the results
    return fetchResults();
}



bool DBHandler::addScore(int userID, int gameID, int score, int timeSpent) {
    // Close any previous statement associated with sqlStmtHandle
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);

    // Prepare the SQL INSERT query
    std::wstring query = L"INSERT INTO Scores (UserID, GameID, Score, TimeSpent) VALUES (?, ?, ?, ?)";

    // Prepare the statement
    if (SQLPrepare(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS) != SQL_SUCCESS) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }

    // Bind the parameters
    if (SQLBindParameter(sqlStmtHandle, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &userID, 0, nullptr) != SQL_SUCCESS ||
        SQLBindParameter(sqlStmtHandle, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &gameID, 0, nullptr) != SQL_SUCCESS ||
        SQLBindParameter(sqlStmtHandle, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &score, 0, nullptr) != SQL_SUCCESS ||
        SQLBindParameter(sqlStmtHandle, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &timeSpent, 0, nullptr) != SQL_SUCCESS) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }

    // Execute the statement
    if (SQLExecute(sqlStmtHandle) != SQL_SUCCESS) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }

    std::cout << "Score added successfully!" << std::endl;
    return true;
}




std::vector<std::string> DBHandler::getScoresByGame(int gameID) {
    // Free any previous statement or cursor
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);

    // Query to fetch the scores for the given game ID
    std::wstring query = L"SELECT Score, TimeSpent, AchievedAt FROM Scores WHERE GameID = " + std::to_wstring(gameID);

    // Execute the query
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return {};
    }

    // Fetch results and store them in a vector of strings
    std::vector<std::string> results;
    SQLINTEGER score, timeSpent;
    SQLWCHAR achievedAt[50];
    SQLLEN scoreLen, timeSpentLen, achievedAtLen;

    while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
        // Retrieve each column value
        SQLGetData(sqlStmtHandle, 1, SQL_C_LONG, &score, 0, &scoreLen);
        SQLGetData(sqlStmtHandle, 2, SQL_C_LONG, &timeSpent, 0, &timeSpentLen);
        SQLGetData(sqlStmtHandle, 3, SQL_C_WCHAR, achievedAt, sizeof(achievedAt), &achievedAtLen);

        // Format the result as a single string and add it to the results vector
        std::wstring result = L"Score: " + std::to_wstring(score) +
            L", Time Spent: " + std::to_wstring(timeSpent) +
            L" seconds, Achieved At: " + std::wstring(achievedAt);

        results.push_back(std::string(result.begin(), result.end())); // Convert wstring to string
    }

    return results;
}



std::vector<std::string> DBHandler::getTopScoresByGame(int gameID, int limit) {
    // Free any previous statement or cursor
    SQLFreeStmt(sqlStmtHandle, SQL_CLOSE);

    // Prepare the query
    std::wstring query = L"SELECT TOP " + std::to_wstring(limit) +
        L" Score FROM Scores WHERE GameID = " +
        std::to_wstring(gameID) +
        L" ORDER BY Score DESC";

    // Debug print the query
   /* std::wcout << L"Executing query: " << query << std::endl;*/

    // Execute the query
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return {};
    }

    // Fetch results
    std::vector<std::string> results;
    SQLINTEGER score;
    SQLLEN scoreLen;

    while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
        // Retrieve the Score column
        SQLGetData(sqlStmtHandle, 1, SQL_C_LONG, &score, 0, &scoreLen);

        // Convert the score to string and add to results
        results.push_back(std::to_string(score));
    }

    return results;
}

bool DBHandler::updateGameProgress(int userID, int gameID, int currentLevel) {
    std::wstring procedureCall = L"{CALL updateGameProgress(?, ?, ?)}"; // Stored procedure call
    SQLHSTMT sqlStmtHandle = NULL; // Statement handle

    try {
        // Allocate statement handle
        if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle)) {
            /*displayError(SQL_HANDLE_DBC, sqlConnHandle);*/
            return false;
        }

        // Prepare the stored procedure call
        if (SQL_SUCCESS != SQLPrepare(sqlStmtHandle, (SQLWCHAR*)procedureCall.c_str(), SQL_NTS)) {
            /*displayError(SQL_HANDLE_STMT, sqlStmtHandle);*/
            return false;
        }

        // Bind parameters
        if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &userID, 0, NULL)) {
            /*displayError(SQL_HANDLE_STMT, sqlStmtHandle);*/
            return false;
        }
        if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &gameID, 0, NULL)) {
            /* displayError(SQL_HANDLE_STMT, sqlStmtHandle);*/
            return false;
        }
        if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &currentLevel, 0, NULL)) {
            /*displayError(SQL_HANDLE_STMT, sqlStmtHandle);*/
            return false;
        }

        // Execute the procedure
        if (SQL_SUCCESS != SQLExecute(sqlStmtHandle)) {
            /*displayError(SQL_HANDLE_STMT, sqlStmtHandle);*/
            return false;
        }

        // Free the statement handle
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in updateGameProgress: " << e.what() << std::endl;
        if (sqlStmtHandle) {
            SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        }
        return false;
    }
}

std::string DBHandler::getProgressByUserAndGame(int userID, int gameID) {
    std::wstring query = L"SELECT CurrentLevel FROM Progress WHERE UserID = " + std::to_wstring(userID) +
        L" AND GameID = " + std::to_wstring(gameID);

    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return "";
    }

    SQLCHAR buffer[256];
    if (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
        SQLGetData(sqlStmtHandle, 1, SQL_C_CHAR, buffer, sizeof(buffer), NULL);
        return std::string((char*)buffer);
    }

    return "";
}


bool DBHandler::addReward(int userID, int gameID, const std::string& rewardName) {
    std::wstring query = L"INSERT INTO Rewards (UserID, GameID, RewardName) VALUES (" +
        std::to_wstring(userID) + L", " + std::to_wstring(gameID) + L", '" +
        std::wstring(rewardName.begin(), rewardName.end()) + L"')";

    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }

    std::cout << "Reward added successfully!" << std::endl;
    return true;
}


std::vector<std::string> DBHandler::getRewardsByUser(int userID) {
    std::wstring query = L"SELECT RewardName FROM Rewards WHERE UserID = " + std::to_wstring(userID);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return {};
    }

    return fetchResults();
}


bool DBHandler::logGameData(int userID, int gameID, const std::string& logData) {
    std::wstring query = L"INSERT INTO Logs (UserID, GameID, LogData) VALUES (" +
        std::to_wstring(userID) + L", " + std::to_wstring(gameID) + L", '" +
        std::wstring(logData.begin(), logData.end()) + L"')";

    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return false;
    }

    std::cout << "Log data inserted successfully!" << std::endl;
    return true;
}


std::vector<std::string> DBHandler::getLogsByGame(int gameID) {
    std::wstring query = L"SELECT LogData FROM Logs WHERE GameID = " + std::to_wstring(gameID);
    if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
        displayError(SQL_HANDLE_STMT, sqlStmtHandle);
        return {};
    }

    return fetchResults();
}


#include <stdexcept>

std::string DBHandler::getNamebyGameId(int gameID) {
    std::string gameName = "";  // Default empty string if the game is not found
    std::wstring query = L"SELECT GameName FROM Games WHERE GameID = ?";

    try {
        if (!sqlConnHandle) {
            std::cerr << "SQL connection handle is not initialized." << std::endl;
            return gameName;
        }

        // Allocate statement handle
        if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle)) {
            displayError(SQL_HANDLE_DBC, sqlConnHandle);
            return gameName;
        }

        // Prepare the SQL statement
        if (SQL_SUCCESS != SQLPrepare(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
            displayError(SQL_HANDLE_STMT, sqlStmtHandle);
            SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
            return gameName;
        }

        // Bind the parameter (GameID)
        if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &gameID, 0, NULL)) {
            displayError(SQL_HANDLE_STMT, sqlStmtHandle);
            SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
            return gameName;
        }

        // Execute the query
        if (SQL_SUCCESS != SQLExecute(sqlStmtHandle)) {
            displayError(SQL_HANDLE_STMT, sqlStmtHandle);
            SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
            return gameName;
        }

        // Fetch the result
        SQLCHAR fetchedName[256];
        if (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
            SQLGetData(sqlStmtHandle, 1, SQL_C_CHAR, fetchedName, sizeof(fetchedName), NULL);
            gameName = std::string((char*)fetchedName);  // Convert SQLCHAR to std::string
        }
        else {
            std::cerr << "No data found for GameID: " << gameID << std::endl;
        }

        // Free the statement handle
        SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return gameName;
}
