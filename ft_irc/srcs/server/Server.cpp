/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yabou-da <yabou-da@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 15:11:29 by yabou-da          #+#    #+#             */
/*   Updated: 2026/08/17 19:26:03 by yabou-da         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, const std::string &password)
    : _port(port), _password(password), _socketFd(-1), _running(false)
{
}

Server::Server(const Server &obj)
{
    *this = obj;
}

Server &Server::operator=(const Server &obj)
{
    if (this != &obj)
    {
        this->_port = obj._port;
        this->_password = obj._password;
        this->_socketFd = obj._socketFd;
        this->_running = obj._running;
        this->_pollVec = obj._pollVec;
    }
    return *this;
}

Server::~Server()
{
    // Fermeture de toutes les sockets ouvertes (clients + écoute)
    for (size_t i = 0; i < this->_pollVec.size(); ++i)
    {
        if (this->_pollVec[i].fd >= 0)
            close(this->_pollVec[i].fd);
    }
    this->_pollVec.clear();
}

void Server::initNetwork()
{
    // 1. Création de la socket IPv4 / TCP
    this->_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socketFd == -1)
        throw Server::SocketException();

    // 2. Libération immédiate du port à l'arrêt du serveur
    int opt = 1;
    if (setsockopt(this->_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        close(this->_socketFd);
        throw Server::SetsockoptException();
    }

    // 3. Passage en mode non-bloquant
    int flags = fcntl(this->_socketFd, F_GETFL, 0);
    if (flags == -1 || fcntl(this->_socketFd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        close(this->_socketFd);
        throw Server::FcntlException();
    }

    // 4. Configuration de l'adresse et liaison (bind)
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Écoute sur toutes les interfaces réseau
    serverAddr.sin_port = htons(this->_port); // Conversion en ordre d'octets réseau (Big-Endian)

    if (bind(this->_socketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        close(this->_socketFd);
        throw Server::BindException();
    }

    // 5. Activation du mode écoute passive
    if (listen(this->_socketFd, SOMAXCONN) == -1)
    {
        close(this->_socketFd);
        throw Server::ListenException();
    }

    std::cout << "[Server] Initialisation réussie sur le port " << this->_port << std::endl;
}

void Server::startLoop()
{
    // Enregistrement de la socket d'écoute dans poll
    struct pollfd listenPollFd;
    listenPollFd.fd = this->_socketFd;
    listenPollFd.events = POLLIN;
    listenPollFd.revents = 0;
    this->_pollVec.push_back(listenPollFd);

    this->_running = true;
    std::cout << "[Server] Boucle événementielle lancée..." << std::endl;

    while (this->_running)
    {
        int pollCount = poll(&this->_pollVec[0], this->_pollVec.size(), -1);
        if (pollCount < 0)
        {
            if (this->_running)
                std::cerr << "[Server] Erreur lors de poll()" << std::endl;
            break;
        }

        // Parcours des descripteurs surveillés
        for (size_t i = 0; i < this->_pollVec.size(); ++i)
        {
            // Vérification de la présence de données à lire
            if (this->_pollVec[i].revents & POLLIN)
            {
                if (this->_pollVec[i].fd == this->_socketFd)
                {
                    // Nouvelle connexion entrante
                    this->_acceptNewClient();
                }
                else
                {
                    // Données envoyées par un client existant
                    this->_handleClientData(i);
                }
            }
            // Gestion des erreurs / déconnexions matérielles
            else if (this->_pollVec[i].revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                if (this->_pollVec[i].fd != this->_socketFd)
                {
                    this->_disconnectClient(i);
                    --i; // Ajustement de l'index suite au redimensionnement du vecteur
                }
            }
        }
    }
}

void Server::_acceptNewClient()
{
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    int clientFd = accept(this->_socketFd, (struct sockaddr *)&clientAddr, &clientLen);
    if (clientFd == -1)
    {
        std::cerr << "[Server] Échec de l'appel à accept()" << std::endl;
        return;
    }

    // Passage obligatoire de la socket client en mode non-bloquant
    int flags = fcntl(clientFd, F_GETFL, 0);
    if (flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "[Server] Échec de fcntl() sur le client." << std::endl;
        close(clientFd);
        return;
    }

    // Ajout dans la surveillance de poll()
    struct pollfd clientPollFd;
    clientPollFd.fd = clientFd;
    clientPollFd.events = POLLIN;
    clientPollFd.revents = 0;
    this->_pollVec.push_back(clientPollFd);

    // POINT D'ANCRAGE POUR TES AMIS :
    // this->_clients[clientFd] = Client(clientFd);

    std::cout << "[Server] Nouveau client connecté (FD: " << clientFd << ")" << std::endl;
}

void Server::_handleClientData(size_t index)
{
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));

    int clientFd = this->_pollVec[index].fd;
    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead <= 0)
    {
        // 0 = Déconnexion normale par le client ; < 0 = Erreur réseau
        this->_disconnectClient(index);
    }
    else
    {
        std::string rawData(buffer);
        std::cout << "[Client " << clientFd << "] Reçu : " << rawData;

        // POINT D'ANCRAGE POUR TES AMIS :
        // 1. Ajouter rawData au buffer interne de l'objet Client.
        // 2. Vérifier la présence d'un délimiteur "\r\n" ou "\n".
        // 3. Envoyer la ligne complète à votre parseur de commandes (NICK, USER, JOIN, etc.).
    }
}

void Server::_disconnectClient(size_t index)
{
    int clientFd = this->_pollVec[index].fd;

    std::cout << "[Server] Client déconnecté (FD: " << clientFd << ")" << std::endl;

    close(clientFd);
    this->_pollVec.erase(this->_pollVec.begin() + index);

    // POINT D'ANCRAGE POUR TES AMIS :
    // this->_clients.erase(clientFd);
    // Retirer le client de tous les salons (_channels) où il était inscrit.
}

void Server::stop()
{
    this->_running = false;
}