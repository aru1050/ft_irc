/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 19:05:02 by athamilc          #+#    #+#             */
/*   Updated: 2026/08/22 19:05:14 by athamilc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>
#include <cstdlib>   // Pour atoi
#include <csignal>   // Pour sigaction, SIGINT

Server *g_serverPtr = NULL;

void handleSignal(int signum)
{
    (void)signum;
    std::cout << "\n[Server] Signal d'arrêt reçu. Fermeture propre..." << std::endl;
    if (g_serverPtr)
        g_serverPtr->stop();
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    int port = atoi(argv[1]); // <- Sans 'std::'
    if (port <= 1024 || port > 65535)
    {
        std::cerr << "Erreur: Port invalide (utiliser une valeur entre 1025 et 65535)." << std::endl;
        return 1;
    }

    std::string password = argv[2];
    if (password.empty())
    {
        std::cerr << "Erreur: Le mot de passe ne peut pas être vide." << std::endl;
        return 1;
    }

    // Gestion du signal SIGINT (Ctrl+C)
    struct sigaction sa;
    sa.sa_handler = handleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    try
    {
        Server server(port, password);
        g_serverPtr = &server;

        server.initNetwork();
        server.startLoop();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}