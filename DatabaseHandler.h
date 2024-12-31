#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include "Hashing.h"
#include <utility>
class DBHandler {
private:
    const std::wstring connectionString; // Fixed connection string as wide string
    SQLHENV sqlEnvHandle;
    SQLHDBC sqlConnHandle;
    SQLHSTMT sqlStmtHandle;
    SQLRETURN retCode;

public:
    // Constructor
    DBHandler()
        : connectionString(L"Driver={ODBC Driver 17 for SQL Server};Server=tcp:localhost,1433;Database=CodeCarnival;Uid=SA;Pwd=StrongPass@123;Encrypt=no;TrustServerCertificate=yes;Connection Timeout=30;"),
        sqlEnvHandle(NULL), sqlConnHandle(NULL), sqlStmtHandle(NULL) {
        connect();
    }

    // Destructor
    ~DBHandler() {
        disconnect();
    }

    // Connection management
    bool connect();
    void disconnect();
    bool isConnected();

    // User operations
    bool addUser(const std::string& username, const std::string& email, const std::string& passwordHash);
    std::string getUserByUsername(const std::string& username);
    bool validateUser(const std::string& username, const std::string& passwordHash);

    int getUserIdByUsername(const std::string& username) {
        int userId = -1;  // Default to -1 if the user is not found

        // Convert username to a wide string
        std::wstring wUsername(username.begin(), username.end());
        std::wstring query = L"SELECT UserID FROM Users WHERE Username = ?";

        SQLHSTMT localStmtHandle = SQL_NULL_HSTMT;  // Use a local statement handle

        try {
            // Allocate a local statement handle
            if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &localStmtHandle)) {
                displayError(SQL_HANDLE_DBC, sqlConnHandle);
                return userId;
            }

            // Prepare the SQL statement
            if (SQL_SUCCESS != SQLPrepare(localStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
                displayError(SQL_HANDLE_STMT, localStmtHandle);
                SQLFreeHandle(SQL_HANDLE_STMT, localStmtHandle);
                return userId;
            }

            // Bind the parameter (Username)
            if (SQL_SUCCESS != SQLBindParameter(localStmtHandle, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, wUsername.length(), 0, (SQLWCHAR*)wUsername.c_str(), 0, NULL)) {
                displayError(SQL_HANDLE_STMT, localStmtHandle);
                SQLFreeHandle(SQL_HANDLE_STMT, localStmtHandle);
                return userId;
            }

            // Execute the query
            if (SQL_SUCCESS != SQLExecute(localStmtHandle)) {
                displayError(SQL_HANDLE_STMT, localStmtHandle);
                SQLFreeHandle(SQL_HANDLE_STMT, localStmtHandle);
                return userId;
            }

            // Fetch the result
            if (SQL_SUCCESS == SQLFetch(localStmtHandle)) {
                SQLGetData(localStmtHandle, 1, SQL_C_SLONG, &userId, sizeof(userId), NULL);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }

        // Free the local statement handle
        if (localStmtHandle != SQL_NULL_HSTMT) {
            SQLFreeHandle(SQL_HANDLE_STMT, localStmtHandle);
        }

        return userId;
    }

    std::string getNamebyGameId(int gameID);


    // Game operations
    bool addGame(const std::string& gameName, const std::string& description, int maxLevels);
    std::vector<std::pair<std::pair<int, std::string>, std::pair<std::string, int>>> getAllGames();

    std::string getGameByID(int gameID);
    bool updateMVP(int gameID, int userID);

    // Level operations
    bool addLevel(int gameID, int levelNumber, const std::string& levelDescription);
    std::vector<std::string> getLevelsByGameID(int gameID);

    // Score operations
    bool addScore(int userID, int gameID, int score, int timeSpent);
    std::vector<std::string> getScoresByGame(int gameID);
    std::vector<std::string> getTopScoresByGame(int gameID, int limit);

    // Progress operations
    bool updateGameProgress(int userID, int gameID, int currentLevel);
    std::string getProgressByUserAndGame(int userID, int gameID);

    // Reward operations
    bool addReward(int userID, int gameID, const std::string& rewardName);
    std::vector<std::string> getRewardsByUser(int userID);

    // Logging
    bool logGameData(int userID, int gameID, const std::string& logData);
    std::vector<std::string> getLogsByGame(int gameID);

    // Utility functions
    bool executeQuery(const std::string& query);
    /* bool executePreparedQuery(const std::string& query, const std::vector<std::string>& params);*/
    std::vector<std::string> fetchResults();

    void displayError(SQLSMALLINT handleType, SQLHANDLE handle);

    void updateGameState(int userID, int gameID, int currentLevel, int score = -1, int timeSpent = -1, const std::wstring& logData = L"") {
        try {
            std::wstring query = L"{CALL updateGameState (?, ?, ?, ?, ?, ?)}";

            // Allocate statement handle
            if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle)) {
                displayError(SQL_HANDLE_DBC, sqlConnHandle);
                return;
            }

            // Prepare the SQL statement
            if (SQL_SUCCESS != SQLPrepare(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
                displayError(SQL_HANDLE_STMT, sqlStmtHandle);
                SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                return;
            }

            // Bind parameters
            if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &userID, 0, NULL)) {
                displayError(SQL_HANDLE_STMT, sqlStmtHandle);
                SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                return;
            }

            if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &gameID, 0, NULL)) {
                displayError(SQL_HANDLE_STMT, sqlStmtHandle);
                SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                return;
            }

            if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &currentLevel, 0, NULL)) {
                displayError(SQL_HANDLE_STMT, sqlStmtHandle);
                SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                return;
            }

            if (score != -1) {
                if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &score, 0, NULL)) {
                    displayError(SQL_HANDLE_STMT, sqlStmtHandle);
                    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                    return;
                }
            }
            else {
                if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 4, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_INTEGER, 0, 0, NULL, 0, NULL)) {
                    displayError(SQL_HANDLE_STMT, sqlStmtHandle);
                    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                    return;
                }
            }

            if (timeSpent != -1) {
                if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 5, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &timeSpent, 0, NULL)) {
                    displayError(SQL_HANDLE_STMT, sqlStmtHandle);
                    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                    return;
                }
            }
            else {
                if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 5, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_INTEGER, 0, 0, NULL, 0, NULL)) {
                    displayError(SQL_HANDLE_STMT, sqlStmtHandle);
                    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                    return;
                }
            }

            if (!logData.empty()) {
                if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 6, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WLONGVARCHAR, logData.size() * sizeof(wchar_t), 0, (SQLPOINTER)logData.c_str(), 0, NULL)) {
                    displayError(SQL_HANDLE_STMT, sqlStmtHandle);
                    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                    return;
                }
            }
            else {
                if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 6, SQL_PARAM_INPUT, SQL_C_DEFAULT, SQL_WLONGVARCHAR, 0, 0, NULL, 0, NULL)) {
                    displayError(SQL_HANDLE_STMT, sqlStmtHandle);
                    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                    return;
                }
            }

            // Execute the statement
            if (SQL_SUCCESS != SQLExecute(sqlStmtHandle)) {
                displayError(SQL_HANDLE_STMT, sqlStmtHandle);
            }
            else {
                std::wcout << L"Game state updated successfully.\n";
            }

            // Free the statement handle
            SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        }
        catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }

    int getCurrentLevel(int userID, int gameID) {
        int currentLevel = 1; // Default to level 1 if no progress is found

        // SQL query to fetch the current level
        std::wstring query = L"SELECT CurrentLevel FROM UserGameProgress WHERE UserID = ? AND GameID = ?";

        try {
            // Allocate statement handle
            if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle)) {
                displayError(SQL_HANDLE_DBC, sqlConnHandle);
                return currentLevel;
            }

            // Prepare the SQL statement
            if (SQL_SUCCESS != SQLPrepare(sqlStmtHandle, (SQLWCHAR*)query.c_str(), SQL_NTS)) {
                displayError(SQL_HANDLE_STMT, sqlStmtHandle);
                SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                return currentLevel;
            }

            // Bind the parameters (UserID and GameID)
            if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &userID, 0, NULL)) {
                displayError(SQL_HANDLE_STMT, sqlStmtHandle);
                SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                return currentLevel;
            }

            if (SQL_SUCCESS != SQLBindParameter(sqlStmtHandle, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &gameID, 0, NULL)) {
                displayError(SQL_HANDLE_STMT, sqlStmtHandle);
                SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                return currentLevel;
            }

            // Execute the query
            if (SQL_SUCCESS != SQLExecute(sqlStmtHandle)) {
                displayError(SQL_HANDLE_STMT, sqlStmtHandle);
                SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
                return currentLevel;
            }

            // Fetch the result
            if (SQL_SUCCESS == SQLFetch(sqlStmtHandle)) {
                SQLGetData(sqlStmtHandle, 1, SQL_C_SLONG, &currentLevel, sizeof(currentLevel), NULL);
            }

            // Free the statement handle
            SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
        }
        catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }

        return currentLevel;
    }

};
