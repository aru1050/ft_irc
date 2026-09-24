/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 19:14:42 by yabou-da          #+#    #+#             */
/*   Updated: 2026/09/24 14:58:43 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <vector>
#include <poll.h>
#include <map>
#include <cstring>
#include <sys/socket.h>
#include <exception>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>

#include "Client.hpp"
#include "Channel.hpp"
#include "Command.hpp"

class Client;
class Channel;
class Privmsg;

extern bool g_serverRunning;
void signalHandler(int signum);

class Server
{
	private:
		std::string						_port;
		std::string						_password;
		int								_socketFd;
		std::vector<struct pollfd>		_pollVec;
		bool							_running;
		std::map<int, Client>			_clients;
		std::map<std::string, Channel>	_channel;
	public:
				Server(std::string port, std::string password);
				Server(const Server &obj);
				
		Server	&operator=(const Server &obj);
				~Server();
		
		addrinfo 	*init_server(const char *port);
		void 		disconnectClient(size_t i);
		void		acceptNewClient();
		void		handleClientData(size_t i);
		void 		commandParser(int clientFd, const std::string &line);
		void 		startLoop();
		class initNetworkException : public std::exception
		{
			public:
				virtual const char* what() const throw(){
					return("Error : initNetwork failed !");
				}
		};
		
	void handlePass(int clientFd, const std::string& line);
	void handleUser(int clientFd, const std::string& line);
	void handleNick(int clientFd, const std::string& line);
	void handleQuit(int clientFd, const std::string& line);
	void handlePrivmsg(int clientFd, const std::string& line);

	void handleJoin(int clientFd, const std::string &line);
void handlePart(int clientFd, const std::string &line);
void handleKick(int clientFd, const std::string &line);
void handleInvite(int clientFd, const std::string &line);
void handleTopic(int clientFd, const std::string &line);
void handleMode(int clientFd, const std::string &line);

bool checkChannelRegistration(int clientFd);

void broadcastChannel(
    Channel &channel,
    const std::string &message,
    int exceptFd);

void sendChannelResult(
    int clientFd,
    ChannelResult result,
    const std::string &command,
    const std::string &channelName,
    const std::string &target);

void sendJoinState(
    int clientFd,
    Channel &channel);

	std::string clientPrefix(int clientFd) const;
	void sendMessage(int clientFd, const std::string &message);
	int findClientFdByNickname(const std::string &nickname) const;
};

#endif

