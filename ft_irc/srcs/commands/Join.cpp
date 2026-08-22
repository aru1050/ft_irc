/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 20:02:56 by athamilc          #+#    #+#             */
/*   Updated: 2026/08/22 20:03:36 by athamilc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <map>
#include <string>
#include <iostream>

/*
    Cette fonction est une version pédagogique de JOIN.

    Elle ne dépend pas encore de votre vraie classe Server ni Client.
    Elle utilise :
    - clientFd : le fd du client
    - channelName : nom du channel, exemple "#42"
    - password : mot de passe donné avec JOIN, exemple "secret"
    - channels : map qui contient tous les channels du serveur
*/

void joinCommand(int clientFd,
    std::string channelName,
    std::string password,
    std::map<std::string, Channel> &channels)
{
    // Vérifie que le nom du channel commence par #
    if (channelName.empty() || channelName[0] != '#')
    {
        std::cout << "Erreur : nom de channel invalide" << std::endl;
        return ;
    }

    // Si le channel n'existe pas encore
    if (channels.find(channelName) == channels.end())
    {
        // On crée le channel
        channels[channelName] = Channel(channelName);

        // On ajoute le client dedans
        channels[channelName].addClient(clientFd);

        // Le créateur devient opérateur
        channels[channelName].addOperator(clientFd);

        std::cout << "Channel cree : " << channelName << std::endl;
        std::cout << "Client " << clientFd << " rejoint "
                  << channelName << " comme operateur" << std::endl;
        return ;
    }

    // Si le channel existe déjà
    Channel &channel = channels[channelName];

    // On vérifie si le client a le droit de rejoindre
    if (!channel.canJoin(clientFd, password))
    {
        std::cout << "Erreur : impossible de rejoindre "
                  << channelName << std::endl;
        return ;
    }

    // Si tout est bon, on ajoute le client
    channel.addClient(clientFd);

    std::cout << "Client " << clientFd
              << " rejoint " << channelName << std::endl;
}

// #include "Channel.hpp"
// #include <map>
// #include <string>
// #include <iostream>

// /*
//     Version pédagogique de PART.

//     clientFd    = fd du client qui veut quitter
//     channelName = nom du channel à quitter
//     channels    = liste de tous les channels du serveur
// */

// void partCommand(int clientFd,
//     std::string channelName,
//     std::map<std::string, Channel> &channels)
// {
//     if (channelName.empty() || channelName[0] != '#')
//     {
//         std::cout << "Erreur : nom de channel invalide" << std::endl;
//         return ;
//     }

//     if (channels.find(channelName) == channels.end())
//     {
//         std::cout << "Erreur : channel inexistant" << std::endl;
//         return ;
//     }

//     Channel &channel = channels[channelName];

//     if (!channel.hasClient(clientFd))
//     {
//         std::cout << "Erreur : client pas dans le channel" << std::endl;
//         return ;
//     }

//     channel.removeClient(clientFd);

//     std::cout << "Client " << clientFd
//               << " quitte " << channelName << std::endl;

//     if (channel.getClientCount() == 0)
//     {
//         channels.erase(channelName);
//         std::cout << "Channel supprime car vide" << std::endl;
//     }
// }