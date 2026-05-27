#include "api.h"
#include <iostream>
#include <string>

namespace API {

    static std::string command(const std::string& cmd) {
        std::cout << cmd << std::endl;
        std::string response;
        std::getline(std::cin, response);
        return response;
    }

    static void commandNoResponse(const std::string& cmd) {
        std::cout << cmd << std::endl;
    }

    int mazeWidth()  { return std::stoi(command("mazeWidth")); }
    int mazeHeight() { return std::stoi(command("mazeHeight")); }

    bool wallFront() { return command("wallFront") == "true"; }
    bool wallRight() { return command("wallRight") == "true"; }
    bool wallLeft()  { return command("wallLeft")  == "true"; }

    void moveForward(int distance) {
        commandNoResponse("moveForward " + std::to_string(distance));
    }
    void turnRight()   { commandNoResponse("turnRight"); }
    void turnLeft()    { commandNoResponse("turnLeft"); }
    void turnAround()  { commandNoResponse("turnAround"); }

    void setWall(int x, int y, char direction) {
        commandNoResponse("setWall " + std::to_string(x) + " " +
                          std::to_string(y) + " " + direction);
    }
    void clearWall(int x, int y, char direction) {
        commandNoResponse("clearWall " + std::to_string(x) + " " +
                          std::to_string(y) + " " + direction);
    }
    void setColor(int x, int y, char color) {
        commandNoResponse("setColor " + std::to_string(x) + " " +
                          std::to_string(y) + " " + color);
    }
    void clearColor(int x, int y) {
        commandNoResponse("clearColor " + std::to_string(x) + " " +
                          std::to_string(y));
    }
    void clearAllColor() { commandNoResponse("clearAllColor"); }
    void setText(int x, int y, const char* text) {
        commandNoResponse("setText " + std::to_string(x) + " " +
                          std::to_string(y) + " " + text);
    }
    void clearText(int x, int y) {
        commandNoResponse("clearText " + std::to_string(x) + " " +
                          std::to_string(y));
    }
    void clearAllText() { commandNoResponse("clearAllText"); }

    bool wasReset() { return command("wasReset") == "true"; }
    void ackReset() { commandNoResponse("ackReset"); }
}
