// Adam Geppert, Piotr Bodnar Grupa 7

#pragma once
#include <string>
#include "asio.hpp"

class Client
{
public:
	Client();
	virtual ~Client();
	static void Echo(const std::string, const asio::ip::address_v4, const unsigned short);
	static void SendText(const std::string, const asio::ip::address_v4, const unsigned short);
	static bool RecvFile(const std::string, const std::string, const asio::ip::address_v4, const unsigned short);
	static bool SendFile(const std::string , const std::string , const asio::ip::address_v4 , const unsigned short);
};