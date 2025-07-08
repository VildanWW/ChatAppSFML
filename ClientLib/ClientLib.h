#pragma once
#include <string>
#include <vector>

bool ConnectToServer(const std::string& ip, unsigned short port, std::string namePeople);
void SendChatMessage(const std::string& msg);
void Disconnect();
void receiveLoop();
void GeTReceivedMessages(std::vector<std::string>& outMessages);