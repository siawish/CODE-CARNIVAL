#pragma once

#include <iostream>
#include "DatabaseHandler.h"
#include "bcrypt.h"

using namespace std;

class User {
private:
    int userID;
    string username;
    string email;
    string passwordHash;
    string createdAt;
    string address;

    DatabaseHandler dbHandler;  // Using DatabaseHandler for DB operations

public:
    // Constructor
    User(int id, const string& uname, const string& pwdHash, const string& mail = "", const string& Address = "", const string& created = "")
        : userID(id), username(uname), email(mail), passwordHash(pwdHash), createdAt(created), address(Address) {
    }

    // Register user in the database
    void registerUser() {
        if (!dbHandler.connectToDatabase(L"Driver={ODBC Driver 17 for SQL Server};Server=tcp:localhost,1433;Database=CodeCarnival;Uid=SA;Pwd=StrongPass@123;Encrypt=no;TrustServerCertificate=yes;Connection Timeout=30;")) {
            cout << "Database connection failed!" << endl;
            return;
        }

        // SQL query to insert a new user
        string query = "INSERT INTO Users (Username, Email, PasswordHash, CreatedAt, Address) VALUES (?, ?, ?, ?, ?)";
        SQLHDBC hDbc = dbHandler.getDbc();

        // Prepare the statement and bind parameters
        SQLHSTMT hStmt;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        SQLPrepare(hStmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

        SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLCHAR*)username.c_str(), 0, NULL);
        SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLCHAR*)email.c_str(), 0, NULL);
        SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 255, 0, (SQLCHAR*)passwordHash.c_str(), 0, NULL);
        SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLCHAR*)createdAt.c_str(), 0, NULL);
        SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 100, 0, (SQLCHAR*)address.c_str(), 0, NULL);

        // Execute the query
        if (SQLExecute(hStmt) != SQL_SUCCESS) {
            cout << "Error executing query!" << endl;
            return;
        }

        cout << "User registered successfully!" << endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    }

    // Login user by validating credentials
    bool loginUser(const string& inputPassword) {
        if (!dbHandler.connectToDatabase(L"Driver={ODBC Driver 17 for SQL Server};Server=tcp:localhost,1433;Database=CodeCarnival;Uid=SA;Pwd=StrongPass@123;Encrypt=no;TrustServerCertificate=yes;Connection Timeout=30;")) {
            cout << "Database connection failed!" << endl;
            return false;
        }

        // Prepare query to fetch password hash for the user
        string query = "SELECT PasswordHash FROM Users WHERE Username = ?";
        SQLHDBC hDbc = dbHandler.getDbc();

        SQLHSTMT hStmt;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
        SQLPrepare(hStmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
        SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0, (SQLCHAR*)username.c_str(), 0, NULL);

        if (SQLExecute(hStmt) != SQL_SUCCESS) {
            cout << "Error executing query!" << endl;
            return false;
        }

        // Fetch result
        SQLCHAR storedHash[255];
        SQLBindCol(hStmt, 1, SQL_C_CHAR, storedHash, sizeof(storedHash), NULL);

        if (SQLFetch(hStmt) != SQL_SUCCESS) {
            cout << "No user found!" << endl;
            return false;
        }

        string storedHashStr((char*)storedHash);
        if (BCrypt::validatePassword(inputPassword, storedHashStr)) {
            cout << "Login successful!" << endl;
            return true;
        }
        else {
            cout << "Invalid username or password!" << endl;
            return false;
        }
    }
};
