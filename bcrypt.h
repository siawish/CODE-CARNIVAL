#ifndef BCRYPT_H
#define BCRYPT_H

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>
#include <cstring>

using namespace std;

// A simple SHA256 implementation in C++ (no external libraries)
class SimpleSHA256 {
public:
    static const int SIZE = 32;

    SimpleSHA256() {
        data = new unsigned char[SIZE];
        memset(data, 0, SIZE);
    }

    ~SimpleSHA256() {
        delete[] data;
    }

    unsigned char* hash(const string& input) {
        // A very simple and naive way to hash (this is NOT a real SHA256 implementation)
        // This is a simple example to show hashing logic in a minimalistic way
        for (size_t i = 0; i < input.length(); i++) {
            data[i % SIZE] ^= input[i]; // Simple XOR-based hashing (for demonstration)
        }
        return data;
    }

    string toString() const {
        stringstream ss;
        for (int i = 0; i < SIZE; ++i) {
            ss << hex << setw(2) << setfill('0') << (int)data[i];
        }
        return ss.str();
    }

private:
    unsigned char* data;
};

// A simple BCrypt-like class for password hashing
class BCrypt {
public:
    // Generate a simple random salt (16 bytes)
    static string generateSalt(size_t length = 16) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> dist(0, 255);

        stringstream ss;
        for (size_t i = 0; i < length; ++i) {
            ss << hex << setw(2) << setfill('0') << dist(gen);
        }
        return ss.str();
    }

    // Simple hash function using SHA256 (simplified)
    static string simpleHash(const string& input) {
        SimpleSHA256 sha256;
        unsigned char* hash = sha256.hash(input);
        return sha256.toString();
    }

    // Generate bcrypt-style hash: salt + hashed_password
    static string generateHash(const string& password) {
        string salt = generateSalt();
        string saltedPassword = salt + password;
        string hash = simpleHash(saltedPassword);

        return salt + ":" + hash; // Combine salt and hash
    }

    // Validate password by comparing with stored hash
    static bool validatePassword(const string& password, const string& storedHash) {
        size_t delimiterPos = storedHash.find(':');
        if (delimiterPos == string::npos) {
            return false; // Invalid format
        }

        string salt = storedHash.substr(0, delimiterPos);
        string storedPasswordHash = storedHash.substr(delimiterPos + 1);

        string saltedPassword = salt + password;
        string hash = simpleHash(saltedPassword);

        return hash == storedPasswordHash;
    }
};

#endif // BCRYPT_H
