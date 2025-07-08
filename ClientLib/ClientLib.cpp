#include "ClientLib.h"
#include <SFML/Network.hpp>
#include <thread>
#include <iostream>
#include <mutex>

sf::TcpSocket socket;
std::thread receiveThread;
std::string namePeople;
bool running = false;

std::vector<std::string> receivedMessages;
std::mutex messagesMutex;

void receiveLoop() {
    while (running) {
        sf::Packet packet;
        if (socket.receive(packet) == sf::Socket::Done) {
            std::string msg;
            packet >> msg;
            std::cout << namePeople << ':' << msg << std::endl;

            // Сохраняем сообщение в список
            std::lock_guard<std::mutex> lock(messagesMutex);
            receivedMessages.push_back(namePeople + ':' + msg);
        }
    }
}

void GeTReceivedMessages(std::vector<std::string>& outMessages) {
    std::lock_guard<std::mutex> lock(messagesMutex);
    outMessages = receivedMessages;
}

bool ConnectToServer(const std::string& ip, unsigned short port, std::string name) {
    namePeople = name;
    if (socket.connect(ip, port) != sf::Socket::Done) {
        return false;
    }
    running = true;
    receiveThread = std::thread(receiveLoop);
    return true;
}

void SendChatMessage(const std::string& msg) {
    sf::Packet packet;
    packet << msg;
    socket.send(packet);
}

void Disconnect() {
    running = false;
    if (receiveThread.joinable())
        receiveThread.join();
    socket.disconnect();
}
