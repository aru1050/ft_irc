/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yabou-da <yabou-da@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 15:11:29 by yabou-da          #+#    #+#             */
/*   Updated: 2026/06/21 19:30:30 by yabou-da         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::string port, std::string password) : _port(port), _password(password), _socketFd(-1){}

Server::Server(const Server &obj)
{
	*this=obj;
}

Server &Server::operator=(const Server &obj)
{
	if(this != &obj)
	{
		this->_port = obj._port;
		this->_password = obj._password;
		this->_socketFd = obj._socketFd;
		this->_pollVec= obj._pollVec;
		// this->_clients = obj._clients;
		// this->_channel = obj._channel;
	}
	return(*this);
}

Server::~Server(){}

void Server::ft_getaddrinfo()
{
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(NULL, "6667", &hints, &servinfo);
	if (status != 0)
	{
		throw Server::initNetworkException();
	}
	std::cout<<"get address info : OK! (Port : "<<"6667"<<")"<< std::endl;
	freeaddrinfo(servinfo);
}

void Server::initSocket()
{
	this->_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if(this->_socketFd == -1)
		throw Server::initNetworkException();
	std::cout<<"listening socket creation : OK! (fd : "<<_socketFd<<")"<< std::endl;
	int opt = 1;
	int set_socket = setsockopt(this->_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	if(set_socket == -1)
		throw Server::initNetworkException();
	std::cout<<"option socket setup : OK!"<< std::endl;
	//int f = fcntl(_socketFd, );
}

// void Server::startLoop()
// {
// 	poll(_pollVec);
	
// }