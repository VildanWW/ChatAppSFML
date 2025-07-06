#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>

void receive(sf::TcpSocket& socket) {
	while (true) {
		sf::Packet packet;
		if (socket.receive(packet) == sf::Socket::Done) {
			std::string msg;
			packet >> msg;
			std::cout << "[Server]:" << msg << std::endl;
		}
	}
}

int main() {
	sf::TcpSocket socket;
	std::string ip;
	std::cout << "Enter server IP: ";
	std::cin >> ip;

	if (socket.connect(ip, 4000) != sf::Socket::Done) {
		std::cerr << "Couldn't connect to server!" << std::endl;
		return 2;
	}

	std::cout << "Connected to server" << std::endl;

	std::thread receiveThread(receive, std::ref(socket));

	while (true) {
		std::string msg;
		std::getline(std::cin, msg);
		sf::Packet packet;
		packet << msg;
		socket.send(packet);
	}

	receiveThread.join();

	return 0;
}