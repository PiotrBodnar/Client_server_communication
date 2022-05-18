// Adam Geppert, Piotr Bodnar Grupa 7

#include <iostream>
#include <thread> 
#include "Server.h"
#include "asio.hpp"
#include "Frame.h"
#include "BinFile.h"
#include <string>

Server::Server(unsigned short port) : port(port)
{
}


Server::~Server()
{
}


//communication session handler (ECHO)
void session(asio::ip::tcp::socket socket)
{
	try
	{
		while(true)
		{
			Frame rframe;
			Frame sframe;
			asio::read(socket, asio::buffer(&rframe, sizeof(Frame)));

			switch (rframe.type)
			{
			case Frame::Type::ECHO:
				std::cout << "Session: sending echo. \n";
				asio::write(socket, asio::buffer(&rframe, sizeof(Frame)));
				break;

			case Frame::Type::TEXT:
				std::cout << "Session: recived text: " << std::string( (const char*)rframe.data, rframe.len) << "\n";
				break;

			case Frame::Type::SEND_FILE:
			{
				std::string a((const char*)rframe.data);
				BinFile file(std::string((const char*)rframe.data, rframe.len), true);
				
				do {
					asio::read(socket, asio::buffer(&sframe, sizeof(Frame)));
					if (sframe.type == Frame::Type::FILE_DATA)
					{
						file.Append(sframe.data, sframe.len);
					}
				} while (sframe.type == Frame::Type::FILE_DATA);

				std::cout << "Session: File recived\n";
			}
			break;

			case Frame::Type::RECV_FILE:
			{
				char* path =  reinterpret_cast<char*>(rframe.data);

				BinFile bFile(std::string(path, rframe.len), false);

				if (bFile.IsOpen())
				{
					size_t recv_back = bFile.Read(rframe.data, 1024);
					while (recv_back == 1024)
					{
						rframe.len = recv_back;
						rframe.type = Frame::Type::FILE_DATA;
						asio::write(socket, asio::buffer(&rframe, sizeof(Frame)));
						recv_back = bFile.Read(rframe.data, 1024);
					}
					rframe.len = recv_back;
					rframe.type = Frame::Type::FILE_DATA;
					asio::write(socket, asio::buffer(&rframe, sizeof(Frame)));
					rframe.type = Frame::Type::END_OF_FILE;
					rframe.len = 0;
					asio::write(socket, asio::buffer(&rframe, sizeof(Frame)));
				}
			}
			break;

			default:
				sframe.type = Frame::Type::INVALID_COMMAND;
				sframe.len = 0;
				asio::write(socket, asio::buffer(&sframe, sizeof(Frame)));
				break;
			}

		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in thread: " << e.what() << "\n";
	}
}

void Server::ThreadRoutine()
{
	asio::io_service io_service; // Any program that uses asio need to have at least one io_service object
	asio::ip::tcp::acceptor acceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)); //create acceptor for listening purposes
	acceptor.listen(1024);
	while (IsRunning())
	{
		asio::ip::tcp::socket socket(io_service); //create a socket for communication purposes
		acceptor.accept(socket); //then accept connection
		
		//read data sent by the client, then send echo
		std::thread(session, std::move(socket)).detach();

		std::this_thread::yield();
	}
}
