// Adam Geppert, Piotr Bodnar Grupa 7

#include <iostream>
#include <string>
#include <chrono>
#include "Server.h"
#include "Client.h"

#define PORT 10000

int main()
{
	Server server(PORT);
	server.Start();

	while (true)
	{
		/*std::this_thread::sleep_for(std::chrono::milliseconds(250));
		std::cout << "Message: ";
		std::string msg;
		std::cin >> msg;*/
		//Client::Echo(msg, asio::ip::address_v4::from_string("127.0.0.1"), PORT);
		//Client::SendText(msg, asio::ip::address_v4::from_string("127.0.0.1"), PORT);
		 
		//Client::SendFile("img.png", "wynik.png", asio::ip::address_v4::from_string("127.0.0.1"), PORT);
		//Client::RecvFile("img.png", "wynik.png", asio::ip::address_v4::from_string("127.0.0.1"), PORT);
	}

	server.Stop();
}