/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 15:01:11 by marvin            #+#    #+#             */
/*   Updated: 2026/09/24 15:01:11 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Privmsg.hpp"

static Client *findClientByNickname(
    const std::string &nickname,
    std::map<int, Client> &clients)
{
    std::map<int, Client>::iterator it;

    for (it = clients.begin();
         it != clients.end();
         ++it)
    {
        if (it->second.getNickname() == nickname)
            return (&it->second);
    }

    return (NULL);
}

PrivmsgResult privmsgCommand(
    int clientFd,
    const std::string &target,
    const std::string &message,
    std::map<int, Client> &clients,
    std::map<std::string, Channel> &channels,
    std::vector<int> &recipients)
{
    recipients.clear();

    /*
        Pas de destinataire.
    */
    if (target.empty())
        return (PRIVMSG_NO_RECIPIENT);

    /*
        Pas de message.
    */
    if (message.empty())
        return (PRIVMSG_NO_TEXT);

    /*
        =================================
        MESSAGE VERS UN CHANNEL
        =================================

        Exemple :
        PRIVMSG #general :Salut
    */
    if (target[0] == '#')
    {
        std::map<std::string, Channel>::iterator channelIt;

        channelIt = channels.find(target);

        if (channelIt == channels.end())
            return (PRIVMSG_NO_SUCH_CHANNEL);

        Channel &channel = channelIt->second;

        /*
            L'expéditeur doit être membre.
        */
        if (!channel.hasClient(clientFd))
            return (PRIVMSG_NOT_ON_CHANNEL);

        const std::vector<int> &members =
            channel.getClients();

        /*
            Tous les membres reçoivent le message
            sauf celui qui l'envoie.
        */
        for (std::vector<int>::const_iterator it = members.begin();
             it != members.end();
             ++it)
        {
            if (*it != clientFd)
                recipients.push_back(*it);
        }

        return (PRIVMSG_OK);
    }

    /*
        =================================
        MESSAGE PRIVÉ
        =================================

        Exemple :
        PRIVMSG Bob :Salut
    */
    Client *targetClient;

    targetClient = findClientByNickname(
        target,
        clients);

    if (targetClient == NULL)
        return (PRIVMSG_NO_SUCH_NICK);

    recipients.push_back(
        targetClient->getFd());

    return (PRIVMSG_OK);
}