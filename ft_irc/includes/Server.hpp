/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 19:14:42 by yabou-da          #+#    #+#             */
/*   Updated: 2026/09/22 12:35:40 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <poll.h>
#include <map>
#include <cstring>
#include <sys/socket.h>
#include <exception>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>

// class Client;
// class Channel;

class Server
{
	private:
		std::string						_port;
		std::string						_password;
		int								_socketFd;
		std::vector<struct pollfd>		_pollVec;
		bool							_running;
		// std::map<int, Client>			_clients;
		// std::map<std::string, Channel>	_channel;
	public:
				Server(std::string port, std::string password);
				Server(const Server &obj);
		Server	&operator=(const Server &obj);
				~Server();
		
		addrinfo 	*init_server(const char *port);
		void 		disconnectClient(size_t i);
		void		acceptNewClient();
		void		handleClientData(size_t i);
		void 		startLoop();
		class initNetworkException : public std::exception
		{
			public:
				virtual const char* what() const throw(){
					return("Error : initNetwork failed !");
				}
		};
};

#endif