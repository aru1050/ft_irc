/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 15:11:29 by yabou-da          #+#    #+#             */
/*   Updated: 2026/09/22 22:58:05 by athamilc         ###   ########.fr       */
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
	freeaddrinfo(res);
	return(res);
}

void Server::disconnectClient(size_t i)
{
	int clientFd = this->_pollVec[i].fd;
	close(clientFd);
	this->_pollVec.erase(this->_pollVec.begin() + i);
	std::cout << "Client on fd " << clientFd << " disconnected." << std::endl;
}

void Server::acceptNewClient()
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	int newClient = accept(this->_socketFd, (struct sockaddr *)&clientAddr, &clientLen);
	if (newClient == -1)
	{
		std::cout<<"accept : failed" << std::endl;
		// attention au valgrind
		throw Server::initNetworkException();
	}
	if (fcntl(newClient, F_SETFL, O_NONBLOCK) == -1)
    {
		std::cout<<"fcntl : failed" << std::endl;
		// attention au valgrind
		close(newClient);
		throw Server::initNetworkException();
    }
	struct pollfd clientFd;
	clientFd.fd = newClient;
    clientFd.events = POLLIN;
    clientFd.revents = 0;
    this->_pollVec.push_back(clientFd);
	this->_clients[newClient] = Client(newClient);
	std::cout << "New client accepted with fd " << newClient<< std::endl;
}

void Server::handleClientData(size_t i)
{
    char buffer[512];
    int clientFd = this->_pollVec[i].fd;

    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

    // Cas d'erreur ou de fermeture de connexion par le client
    if (bytesRead <= 0)
    {
        if (bytesRead == 0)
            std::cout << "client " << clientFd << " has closed connection" << std::endl;
        else
            perror("recv() failed");
        this->disconnectClient(i);
        return;
    }
    // Assurer la terminaison de la chaîne reçue
    buffer[bytesRead] = '\0';
    std::string receivedData(buffer);
	
	std::cout << "Buffer reçu du client " << clientFd << " : " << receivedData << std::endl;
	this->_clients[clientFd].appendBuffer(receivedData);
	size_t pos;
	while((pos = this->_clients[clientFd].getBuffer().find("\n")) != std::string::npos)
	{
		std::string command = this->_clients[clientFd].getBuffer().substr(0, pos);
		this->_clients[clientFd].clearBufferpos(0, pos + 1);
		this->commandParser(clientFd, command);
	}
}

void Server::commandParser(int clientFd, const std::string &line)
{
	(void) clientFd;
    if (line.empty())
        return;
	
	else if (line.compare(0, 5, "PASS ") == 0)
    {
		std::cout << "PASS" << " : " << "activated" << std::endl;
		// a toi de jouer Aru
    }
    else if (line.compare(0, 5, "USER ") == 0)
    {
		std::cout << "USER" << " : " << "activated" << std::endl;
        // a toi de jouer Aru
    }
    else if (line.compare(0, 5, "NICK ") == 0)
    {
		std::cout << "NICK" << " : " << "activated" << std::endl;
        // a toi de jouer Aru
    }
    else if (line.compare(0, 5, "QUIT ") == 0)
    {
		std::cout << "QUIT" << " : " << "activated" << std::endl;
        // a toi de jouer Aru
    }
    else if (line.compare(0, 5, "KICK ") == 0)
    {
		std::cout << "KICK" << " : " << "activated" << std::endl;
        // a toi de jouer Vincent
    }
    else if (line.compare(0, 7, "INVITE ") == 0)
    {
		std::cout << "INVITE" << " : " << "activated" << std::endl;
         // a toi de jouer Vincent
    }
    else if (line.compare(0, 5, "TOPIC ") == 0)
    {
		std::cout << "TOPIC" << " : " << "activated" << std::endl;
         // a toi de jouer Vincent
    }
	else if (line.compare(0, 5, "MODE ") == 0)
    {
		std::cout << "MODE" << " : " << "activated" << std::endl;
         // a toi de jouer Vincent
    }
}

void Server::startLoop(){
	this->_pollVec.clear();

	struct pollfd listen_fds;
	listen_fds.fd = this->_socketFd;
	listen_fds.events = POLLIN;
	listen_fds.revents = 0;

	this->_pollVec.push_back(listen_fds);
	this->_running = true;
    std::cout << "Waiting for connexions..." << std::endl;
	while (this->_running)
	{
		int c = poll(&this->_pollVec[0], this->_pollVec.size(), -1);
		if (c == -1)
		{
			if (this->_running == false)
                break;
			std::cout<<"pool: failed" << std::endl;
			close(this->_socketFd);
			throw Server::initNetworkException();
		}
		for (size_t i = 0; i < this->_pollVec.size(); ++i)
		{
			if (this->_pollVec[i].revents & (POLLHUP | POLLERR | POLLNVAL))
			{
				this->disconnectClient(i);
				--i;
				continue;
			}
			if (this->_pollVec[i].revents & POLLIN)
			{
				if (this->_pollVec[i].fd == this->_socketFd)
				{
					this->acceptNewClient();
				}
				else
				{
					size_t initialSize = this->_pollVec.size();
					this->handleClientData(i);
					if (this->_pollVec.size() < initialSize)
                		--i;
				}
			}
		}
	}
	for (size_t i = 0; i < this->_pollVec.size(); ++i)
    {
        if (this->_pollVec[i].fd != -1)
            close(this->_pollVec[i].fd);
    }
    this->_pollVec.clear();
}