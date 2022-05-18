// Adam Geppert, Piotr Bodnar Grupa 7

#include "Client.h"
#include "Frame.h"
#include "BinFile.h"
#include "asio.hpp"
#include <iostream>

Client::Client()
{

}

Client::~Client()
{
}

void Client::Echo(std::string text, const asio::ip::address_v4 ip, const unsigned short port)
{
	asio::io_service io_service; 
	asio::ip::tcp::socket socket(io_service);
	Frame frame;

	frame.type = Frame::Type::ECHO;
	frame.len = text.size();
	memcpy(frame.data, text.data(), text.size());
	
	std::cout << "Client: Connect to server " << ip.to_string() << ":" << port << "\n";
	socket.connect(asio::ip::tcp::endpoint(ip, port));
	std::cout << "Client: Send " << text.size() << " bytes of text data" << "\n";
 	asio::write(socket, asio::buffer(&frame, sizeof(Frame)));


	asio::read(socket, asio::buffer(&frame, sizeof(Frame)));
	std::cout << "Client: Recived response: " << std::string((const char*)frame.data, frame.len) << "\n";
	socket.close();
}

void Client::SendText(const std::string text, const asio::ip::address_v4 ip, const unsigned short port)
{
	asio::io_service io_service;
	asio::ip::tcp::socket socket(io_service);
	Frame frame;

	frame.type = Frame::Type::TEXT;
	frame.len = text.size();
	memcpy(frame.data, text.data(), text.size());

	std::cout << "Client: Connect to server " << ip.to_string() << ":" << port << "\n";
	socket.connect(asio::ip::tcp::endpoint(ip, port));
	std::cout << "Client: Send " << text.size() << " bytes of text data" << "\n";
	asio::write(socket, asio::buffer(&frame, sizeof(Frame)));
	socket.close();
}

bool Client::RecvFile(const std::string readFilename, const std::string writeFilename, const asio::ip::address_v4 ip, const unsigned short port)
{

	BinFile file(writeFilename, true);

	if (file.IsOpen())
	{
		asio::io_service io_service;
		asio::ip::tcp::socket socket(io_service);
		Frame frame;
		
		frame.type = Frame::Type::RECV_FILE;
		memcpy(frame.data, readFilename.data(), readFilename.size());
		frame.len = readFilename.size();
		socket.connect(asio::ip::tcp::endpoint(ip, port));
		asio::write(socket, asio::buffer(&frame, sizeof(Frame)));

		do {
			asio::read(socket, asio::buffer(&frame, sizeof(Frame)));
			if (frame.type == Frame::Type::FILE_DATA)
			{
				file.Append(frame.data, frame.len);
			}
			else if (frame.type == Frame::Type::END_OF_FILE)
			{
				std::cout << "Client: File recived\n";
				return true;
			}
		} while (frame.type == Frame::Type::FILE_DATA);
	}
	std::cout << "Client: File transfer fail\n";
	return false;
}

bool Client::SendFile(const std::string readFilename, const std::string writeFilename, const asio::ip::address_v4 ip, const unsigned short port)
{
	asio::io_service io_service;
	asio::ip::tcp::socket socket(io_service);
	Frame frame;

	frame.type = Frame::Type::SEND_FILE;
	memcpy(frame.data, writeFilename.data(), writeFilename.size());
	frame.len = writeFilename.size();
	
	socket.connect(asio::ip::tcp::endpoint(ip, port));
	asio::write(socket, asio::buffer(&frame, sizeof(Frame)));

	BinFile file(readFilename, false);

	if (file.IsOpen())
	{
		size_t recv_back = file.Read(frame.data, 1024);
		while (recv_back == 1024)
		{
			frame.len = recv_back;
			frame.type = Frame::Type::FILE_DATA;
			asio::write(socket, asio::buffer(&frame, sizeof(Frame)));
			recv_back = file.Read(frame.data, 1024);
		}
		frame.len = recv_back;
		frame.type = Frame::Type::FILE_DATA; 
		asio::write(socket, asio::buffer(&frame, sizeof(Frame)));
		frame.type = Frame::Type::END_OF_FILE;
		frame.len = 0;
		asio::write(socket, asio::buffer(&frame, sizeof(Frame)));
		return true;
	}
	socket.close();
	return false;
}
