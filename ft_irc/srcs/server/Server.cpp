/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yabou-da <yabou-da@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 15:11:29 by yabou-da          #+#    #+#             */
/*   Updated: 2026/09/29 22:09:50 by yabou-da         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"
#include "../../includes/Privmsg.hpp"
#include "../../includes/Quit.hpp"

bool g_serverRunning = true;

void signalHandler(int signum)
{
    (void)signum;
    g_serverRunning = false;
}

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
		this->_clients = obj._clients;
		this->_channel = obj._channel;
	}
	return(*this);
}

Server::~Server(){}

void Server::init_server(const char *port)
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
}

void Server::disconnectClient(size_t i)
{
	int clientFd = this->_pollVec[i].fd;
		// Retire le client de tous les channels
	quitCommand(clientFd, this->_channel);
	close(clientFd);
	this->_pollVec.erase(this->_pollVec.begin() + i);
	this->_clients.erase(clientFd);
	std::cout << "Client on fd " << clientFd << " disconnected." << std::endl;
}

void Server::acceptNewClient()
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);

	int newClient = accept(this->_socketFd, (struct sockaddr *)&clientAddr, &clientLen);
	if (newClient == -1)
	{
		perror("accept() failed");
		return;
	}
	if (fcntl(newClient, F_SETFL, O_NONBLOCK) == -1)
    {
		perror("fcntl() failed");
        close(newClient);
        return;
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

    if (bytesRead <= 0)
    {
        if (bytesRead == 0)
            std::cout << "client " << clientFd << " has closed connection" << std::endl;
        else
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
				return;
		}
		perror("recv() failed");
		disconnectClient(i);
		return;
    }
    // Assurer la terminaison de la chaîne reçue
    buffer[bytesRead] = '\0';
    std::string receivedData(buffer);
	
	std::cout << "Buffer reçu du client " << clientFd << " : " << receivedData << std::endl;
	this->_clients[clientFd].appendBuffer(receivedData);
	if (this->_clients[clientFd].getBuffer().find("\n") == std::string::npos 
        && this->_clients[clientFd].getBuffer().size() > 512)
	{
		this->_clients[clientFd].clearBufferpos(510, this->_clients[clientFd].getBuffer().size() - 510);
        this->_clients[clientFd].appendBuffer("\r\n");
	}
	size_t pos;
	while((pos = this->_clients[clientFd].getBuffer().find("\n")) != std::string::npos)
	{
		std::string command = this->_clients[clientFd].getBuffer().substr(0, pos);
		this->_clients[clientFd].clearBufferpos(0, pos + 1);

		if (!command.empty() && command[command.size() - 1] == '\r')
            command.erase(command.size() - 1);
		
		if (command.size() > 510)
            command = command.substr(0, 510);
		if (!command.empty())
			this->commandParser(clientFd, command);
		if (this->_clients.find(clientFd) == this->_clients.end())
            break;
	}
}

void Server::commandParser(int clientFd, const std::string &line)
{
	if (line.empty())
	{
		return;
	}
	std::string command = line;
	size_t space = command.find(' ');
	if (space != std::string::npos)
	{
		command = command.substr(0, space);
	}
	for (size_t i = 0; i < command.size(); ++i)
	{
		if (command[i] >= 'a' && command[i] <= 'z')
		{
			command[i] = command[i] - 'a' + 'A';
		}
	}
	if (command == "PASS")
		handlePass(clientFd, line);
	else if (command == "NICK")
		handleNick(clientFd, line);
	else if (command == "USER")
		handleUser(clientFd, line);
	else if (command == "QUIT")
		handleQuit(clientFd, line);
	else if (command == "PRIVMSG")
		handlePrivmsg(clientFd, line);
	else if (command == "KICK")
		handleKick(clientFd, line);
	else if (command == "JOIN")
		handleJoin(clientFd, line);
	else if (command == "PART")
		handlePart(clientFd, line);
	else if (command == "INVITE")
		handleInvite(clientFd, line);
	else if (command == "TOPIC")
		handleTopic(clientFd, line);
	else if (command == "MODE")
		handleMode(clientFd, line);
	else
	{
		std::cout << "Unknown command from client "
				<< clientFd << ": " << line << std::endl;
	}
}

void Server::startLoop(){

	std::signal(SIGINT, signalHandler);
    std::signal(SIGQUIT, signalHandler);

	this->_pollVec.clear();

	struct pollfd listen_fds;
	listen_fds.fd = this->_socketFd;
	listen_fds.events = POLLIN;
	listen_fds.revents = 0;

	this->_pollVec.push_back(listen_fds);
	this->_running = true;
    std::cout << "Waiting for connexions..." << std::endl;
	while (g_serverRunning)
	{
		int c = poll(&this->_pollVec[0], this->_pollVec.size(), -1);
		if (c == -1)
		{
			if (this->_running == false || errno == EINTR)
                break;
			perror("poll failed");
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
			if (this->_pollVec[i].revents & POLLOUT)
			{
				int clientFd = this->_pollVec[i].fd;
				std::string recivedbuffer = this->_clients[clientFd].getBuffer();

				if(!recivedbuffer.empty())
				{
					size_t sent = send(clientFd, recivedbuffer.c_str(), recivedbuffer.size(), MSG_NOSIGNAL);
					if (sent > 0){
						recivedbuffer.erase(0, sent);
					}
					else if ((int)sent == -1){
						this->disconnectClient(i);
						--i;
						continue;
					}
					if (recivedbuffer.empty())
					{
						this->_pollVec[i].events &= ~POLLOUT;
					}
				}
			}
		}
	}
	std::cout << "\nShutting down server..." << std::endl;
	for (size_t i = 0; i < this->_pollVec.size(); ++i)
    {
        if (this->_pollVec[i].fd != -1)
            close(this->_pollVec[i].fd);
    }
    this->_pollVec.clear();
	this->_clients.clear();
}


void Server::sendMessage(int clientFd, const std::string &message)
{
    send(clientFd, message.c_str(), message.size(), 0);
}

static std::string toLowerNickname(const std::string& str)
{
    std::string result = str;

    for (size_t i = 0; i < result.size(); ++i)
    {
        if (result[i] >= 'A' && result[i] <= 'Z')
            result[i] = result[i] - 'A' + 'a';
    }

    return result;
}

int Server::findClientFdByNickname(const std::string &nickname) const
{
    std::map<int, Client>::const_iterator it;

    for (it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (toLowerNickname(it->second.getNickname())
            == toLowerNickname(nickname))
            return (it->first);
    }
    return (-1);
}

std::string Server::clientPrefix(int clientFd) const
{
    std::map<int, Client>::const_iterator it = _clients.find(clientFd);

    if (it == _clients.end())
        return (":unknown!unknown@localhost");

    std::string nick = it->second.getNickname();
    std::string user = it->second.getUsername();

    if (nick.empty())
        nick = "*";
    if (user.empty())
        user = "unknown";
    return (":" + nick + "!" + user + "@localhost");
}
