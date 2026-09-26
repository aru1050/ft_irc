/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 15:09:44 by marvin            #+#    #+#             */
/*   Updated: 2026/09/24 15:09:44 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Quit.hpp"

void quitCommand(
    int clientFd,
    std::map<std::string, Channel> &channels)
{
    std::map<std::string, Channel>::iterator it;

    it = channels.begin();

    /*
        On parcourt tous les channels.
    */
    while (it != channels.end())
    {
        /*
            Si le client est dans ce channel,
            on le retire.
        */
        if (it->second.hasClient(clientFd))
            it->second.removeClient(clientFd);

        /*
            Si plus personne n'est dans le channel,
            on supprime complètement le channel.
        */
        if (it->second.getClientCount() == 0)
        {
            std::map<std::string, Channel>::iterator toDelete;

            toDelete = it;

            ++it;

            channels.erase(toDelete);
        }
        else
        {
            ++it;
        }
    }
}