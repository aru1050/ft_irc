#include "Channel.hpp"
#include <map>
#include <string>
#include <iostream>

/*
	Version pédagogique de PART.

	clientFd    = fd du client qui veut quitter
	channelName = nom du channel à quitter
	channels    = liste de tous les channels du serveur
*/

void partCommand(int clientFd,
	std::string channelName,
	std::map<std::string, Channel> &channels)
{
	if (channelName.empty() || channelName[0] != '#')
	{
		std::cout << "Erreur : nom de channel invalide" << std::endl;
		return ;
	}

	if (channels.find(channelName) == channels.end())
	{
		std::cout << "Erreur : channel inexistant" << std::endl;
		return ;
	}

	Channel &channel = channels[channelName];

	if (!channel.hasClient(clientFd))
	{
		std::cout << "Erreur : client pas dans le channel" << std::endl;
		return ;
	}

	channel.removeClient(clientFd);

	std::cout << "Client " << clientFd
			  << " quitte " << channelName << std::endl;

	if (channel.getClientCount() == 0)
	{
		channels.erase(channelName);
		std::cout << "Channel supprime car vide" << std::endl;
	}
}