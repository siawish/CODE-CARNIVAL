#pragma once

#include <string>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>

class Hashing {
public:
    // Function to generate a random salt
    static std::string generateSalt(size_t length = 16) {
        static const char characters[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> distribution(0, sizeof(characters) - 2);

        std::string salt;
        for (size_t i = 0; i < length; ++i) {
            salt += characters[distribution(generator)];
        }
        return salt;
    }

    // Simple hash function (XOR-based) to hash a password with a salt
    static std::string hashWithSalt(const std::string& password, const std::string& salt) {
        std::string input = password + salt;
        std::ostringstream hashedStream;

        for (size_t i = 0; i < input.size(); ++i) {
            char hashedChar = input[i] ^ (i % 256); // XOR each character with its index (basic hashing logic)
            hashedStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hashedChar & 0xFF);
        }

        return hashedStream.str();
    }
};
