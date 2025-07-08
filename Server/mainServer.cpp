#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include <mutex>

void clientHandler(std::unique_ptr<sf::TcpSocket> socket, std::vector<sf::TcpSocket*>& clients, std::mutex& clientsMutex){
	{
		std::lock_guard<std::mutex> lock(clientsMutex);
		clients.push_back(socket.get());
	}

	while (true) {
		sf::Packet packet;
		if (socket->receive(packet) == sf::Socket::Done) {
			std::string msg;
			packet >> msg;
			std::cout << "Client [" << socket->getRemoteAddress() << "]: " << msg << std::endl;

			std::lock_guard<std::mutex> lock(clientsMutex);
			for (auto client : clients) {
				if (client != socket.get()) {
					sf::Packet sendPacket;
					sendPacket << msg;
					client->send(sendPacket);
				}
			}
		}
		else {
			std::cout << "Client disconnected: " << socket->getRemoteAddress() << std::endl;
			break;
		}
	}

	std::lock_guard<std::mutex> lock(clientsMutex);
	clients.erase(std::remove(clients.begin(), clients.end(), socket.get()), clients.end());
}

int main() {
	sf::TcpListener listener;
	std::vector<sf::TcpSocket*> clients;
	std::mutex clientsMutex;

	if (listener.listen(3000) != sf::Socket::Done) {
		std::cerr << "Error binding port" << std::endl;
		return 1;
	}

	std::cout << "Server running at " << sf::IpAddress::getLocalAddress() << ":3000";

	std::vector<std::thread> threads;

	while (true) {
		std::unique_ptr<sf::TcpSocket> socket(new sf::TcpSocket());
		if (listener.accept(*socket) == sf::Socket::Done) {
			std::cout << "New client connected: " << socket->getRemoteAddress() << std::endl;
			threads.emplace_back(clientHandler, std::move(socket), std::ref(clients), std::ref(clientsMutex));
		}
	}

	for (auto& t : threads) {
		t.join();
	}

	return 0;
}
