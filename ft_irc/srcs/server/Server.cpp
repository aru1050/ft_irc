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

#include "../../includes/Server.hpp"

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

addrinfo *Server::init_server(const char *port)
{
	int status;
	struct addrinfo hints;
	struct addrinfo *res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// part 1 : getaddrinfo() to prepare all the structs we need
	status = getaddrinfo(NULL, port, &hints, &res);
	if (status != 0)
	{
		std::cout<<"getaddrinfo : failed" << std::endl;
		throw Server::initNetworkException();
	}
	std::cout<<"getaddrinfo: OK! (Port : "<< port <<")"<< std::endl;

	// part 2 : socket() to open a socket fd for listening 
	this->_socketFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(this->_socketFd == -1)
	{
		std::cout<<"getaddrinfo : failed" << std::endl;
		freeaddrinfo(res);
		close(this->_socketFd);
		throw Server::initNetworkException();
	}
	std::cout<<"listening socket creation : OK! (fd : "<<_socketFd<<")"<< std::endl;

	// part 2.5 : setsockopt() to to configure socket_fd
	int opt = 1;
	status = setsockopt(this->_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	if(status == -1)
	{
		std::cout<<"setsockopt: failed" << std::endl;
		freeaddrinfo(res);
		close(this->_socketFd);
		throw Server::initNetworkException();
	}
	if (fcntl(this->_socketFd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cout << "fcntl O_NONBLOCK : failed" << std::endl;
        freeaddrinfo(res);
        close(this->_socketFd);
        throw Server::initNetworkException();
    }
	std::cout<<"option socket setup : OK!"<< std::endl;

	// part 3 : bind() to link socket_fd and port
	status = bind(this->_socketFd, res->ai_addr, res->ai_addrlen);
	if(status == -1)
	{
		std::cout<<"bind: failed" << std::endl;
		freeaddrinfo(res);
		close(this->_socketFd);
		throw Server::initNetworkException();
	}
	std::cout<<"bind : OK!"<< std::endl;

	//part 4 : listen() to wait for entering connection
	status = listen(this->_socketFd, 20);
	if(status == -1)
	{
		std::cout<<"listen : failed" << std::endl;
		freeaddrinfo(res);
		close(this->_socketFd);
		throw Server::initNetworkException();
	}
	std::cout<<"listen : OK!"<< std::endl;
	return(res);
}

// void Server::startLoop(){

// }