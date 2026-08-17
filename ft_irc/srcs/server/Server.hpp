/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yabou-da <yabou-da@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 19:14:42 by yabou-da          #+#    #+#             */
/*   Updated: 2026/08/17 19:25:47 by yabou-da         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <cstdlib>
#include <exception>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

class Client;
class Channel;

class Server
{
    private:
        int                             _port;
        std::string                     _password;
        int                             _socketFd;
        bool                            _running;
        std::vector<struct pollfd>      _pollVec;
        // std::map<int, Client>        _clients;   // À décommenter quand Client sera prêt
        // std::map<std::string, Channel> _channels; // À décommenter quand Channel sera prêt

        void    _acceptNewClient();
        void    _handleClientData(size_t index);
        void    _disconnectClient(size_t index);

    public:
        Server(int port, const std::string &password);
        Server(const Server &obj);
        Server &operator=(const Server &obj);
        ~Server();

        void    initNetwork();
        void    startLoop();
        void    stop();

        // Classes d'exceptions canoniques
        class SocketException : public std::exception {
            public: virtual const char* what() const throw() { return "Erreur: Impossible d'initialiser la socket réseau."; }
        };
        class SetsockoptException : public std::exception {
            public: virtual const char* what() const throw() { return "Erreur: Échec de configuration de setsockopt (SO_REUSEADDR)."; }
        };
        class FcntlException : public std::exception {
            public: virtual const char* what() const throw() { return "Erreur: Échec du passage en mode non-bloquant (fcntl)."; }
        };
        class BindException : public std::exception {
            public: virtual const char* what() const throw() { return "Erreur: Impossible de lier la socket au port (bind)."; }
        };
        class ListenException : public std::exception {
            public: virtual const char* what() const throw() { return "Erreur: Impossible d'activer l'écoute sur la socket (listen)."; }
        };
};

#endif