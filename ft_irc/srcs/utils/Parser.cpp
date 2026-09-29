/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 14:03:24 by marvin            #+#    #+#             */
/*   Updated: 2026/09/24 14:03:24 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"
#include "../../includes/Privmsg.hpp"
#include "../../includes/Quit.hpp"

// Commandes d'enregistrement et de communication

void Server::handlePass(int clientFd, const std::string& line)
{
	// Vérifie que le mot de passe est présent
	if (line.size() <= 5)
	{
		std::cout << "PASS : ERROR" << std::endl;
		sendMessage(
			clientFd,
			":ircserv 461 * PASS :Not enough parameters\r\n"
		);
		return;
	}
	// Récupère le mot de passe envoyé
	std::string password = line.substr(5);
	Client& client = this->_clients[clientFd];
	// PASS ne peut pas être envoyé une deuxième fois
	if (client.isRegistered() || client.hasPass())
	{
		std::cout << "PASS : ERROR (already registered)" << std::endl;
		sendMessage(
			clientFd,
			":ircserv 462 * :You may not reregister\r\n"
		);
		return;
	}
	// Compare avec le mot de passe du serveur
	Command command;
	if (command.pass(client, password, this->_password))
		std::cout << "PASS : OK" << std::endl;
	else
	{
		std::cout << "PASS : ERROR" << std::endl;
		sendMessage(
			clientFd,
			":ircserv 464 * :Password incorrect\r\n"
		);
	}
}

void Server::handleUser(int clientFd, const std::string& line)
{
	Client& client = this->_clients[clientFd];

	// USER ne peut être défini qu'une seule fois
	if (client.isRegistered() || client.hasUser())
	{
		std::cout << "USER : ERROR (already registered)" << std::endl;
		sendMessage(clientFd,
			":ircserv 462 * :You may not reregister\r\n");
		return;
	}
	// Vérifie que la commande contient des paramètres
	if (line.size() <= 5)
	{
		std::cout << "USER : ERROR" << std::endl;
		sendMessage(clientFd,
			":ircserv 461 * USER :Not enough parameters\r\n");
		return;
	}
	// Sépare les paramètres du realname
	std::string params = line.substr(5);
	size_t colon = params.find(" :");
	if (colon == std::string::npos)
	{
		std::cout << "USER : ERROR" << std::endl;
		sendMessage(clientFd,
			":ircserv 461 * USER :Not enough parameters\r\n");
		return;
	}
	std::string beforeColon = params.substr(0, colon);
	std::string realname = params.substr(colon + 2);

	// Cherche username, mode et paramètre inutilisé
	size_t firstSpace = beforeColon.find(' ');
	if (firstSpace == std::string::npos)
	{
		sendMessage(clientFd,
			":ircserv 461 * USER :Not enough parameters\r\n");
		return;
	}
	size_t secondSpace = beforeColon.find(' ', firstSpace + 1);
	if (secondSpace == std::string::npos)
	{
		sendMessage(clientFd,
			":ircserv 461 * USER :Not enough parameters\r\n");
		return;
	}
	// Récupère les différentes parties de USER
	std::string username = beforeColon.substr(0, firstSpace);
	std::string mode = beforeColon.substr(
		firstSpace + 1,
		secondSpace - firstSpace - 1
	);
	std::string unused = beforeColon.substr(secondSpace + 1);
	if (username.empty()
		|| mode.empty()
		|| unused.empty()
		|| realname.empty())
	{
		std::cout << "USER : ERROR" << std::endl;
		sendMessage(clientFd,
			":ircserv 461 * USER :Not enough parameters\r\n");
		return;
	}
	// Enregistre les informations dans le Client
	Command command;
	if (command.user(client, username, realname))
		std::cout << "USER : OK" << std::endl;
	else
	{
		std::cout << "USER : ERROR" << std::endl;
		sendMessage(clientFd,
			":ircserv 461 * USER :Not enough parameters\r\n");
	}
}

// Sert à comparer les nicknames sans tenir compte des majuscules
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

void Server::handleNick(int clientFd, const std::string& line)
{
	// Vérifie qu'un nickname est présent
	if (line.size() <= 5)
	{
		std::cout << "NICK : ERROR" << std::endl;
		sendMessage(
			clientFd,
			":ircserv 431 * :No nickname given\r\n"
		);
		return;
	}
	std::string nickname = line.substr(5);
	Client& client = this->_clients[clientFd];
	Command command;

	// Vérifie si le nickname est déjà utilisé
	for (std::map<int, Client>::iterator it = this->_clients.begin();
		it != this->_clients.end(); ++it)
	{
		if (it->first != clientFd &&
			toLowerNickname(it->second.getNickname()) == toLowerNickname(nickname))
		{
			std::cout << "NICK : ERROR (already used)" << std::endl;
			sendMessage(
				clientFd,
				":ircserv 433 * " + nickname + " :Nickname is already in use\r\n"
			);
			return;
		}
	}
	// Garde les anciennes informations avant le changement
	std::string oldPrefix = clientPrefix(clientFd);
	std::string oldNickname = client.getNickname();
	if (command.nick(client, nickname))
	{
		std::cout << "NICK : OK" << std::endl;
		// Informe les autres clients du changement
		if (!oldNickname.empty())
		{
			std::string message = oldPrefix
				+ " NICK :" + nickname + "\r\n";
			for (std::map<int, Client>::iterator it = this->_clients.begin();
				it != this->_clients.end(); ++it)
			{
				if (it->first != clientFd)
					sendMessage(it->first, message);
			}
		}
	}
	else
	{
		std::cout << "NICK : ERROR" << std::endl;
		sendMessage(
			clientFd,
			":ircserv 432 * " + nickname + " :Erroneous nickname\r\n"
		);
	}
}

void Server::handleQuit(int clientFd, const std::string& line)
{
	// Récupère la raison éventuelle du départ
	std::cout << "QUIT reconnu pour fd " << clientFd << std::endl;
	std::string reason;
	if (line.size() > 5)
		reason = line.substr(5);
	if (!reason.empty() && reason[0] == ':')
		reason.erase(0, 1);
	Client &client = this->_clients[clientFd];
	Command command;
	if (command.quit(client, reason))
	{
		std::cout << "Command::quit = TRUE" << std::endl;
		std::string msg = clientPrefix(clientFd)
			+ " QUIT :" + reason + "\r\n";
		// Évite d'envoyer QUIT plusieurs fois au même client
		std::vector<int> notified;
		for (std::map<std::string, Channel>::iterator ch = _channel.begin();
			 ch != _channel.end(); ++ch)
		{
			if (!ch->second.hasClient(clientFd))
				continue;
			const std::vector<int> &members = ch->second.getClients();
			for (std::vector<int>::const_iterator member = members.begin();
				 member != members.end(); ++member)
			{
				if (*member == clientFd)
					continue;
				bool alreadyNotified = false;
				for (std::vector<int>::const_iterator seen = notified.begin();
					 seen != notified.end(); ++seen)
				{
					if (*seen == *member)
					{
						alreadyNotified = true;
						break;
					}
				}
				if (!alreadyNotified)
				{
					sendMessage(*member, msg);
					notified.push_back(*member);
				}
			}
		}
		// Retire le client de tous les channels
		quitCommand(clientFd, _channel);
		// Retire ensuite le client du serveur
		for (size_t i = 0; i < this->_pollVec.size(); ++i)
		{
			if (this->_pollVec[i].fd == clientFd)
			{
				std::cout << "Déconnexion fd "
						  << clientFd << std::endl;
				this->disconnectClient(i);
				return;
			}
		}
	}
	else
	{
		std::cout << "Command::quit = FALSE" << std::endl;
	}
}

void Server::handlePrivmsg(int clientFd, const std::string& line)
{
	Client &client = this->_clients[clientFd];

	// Le client doit être complètement enregistré
	if (!client.isRegistered())
	{
		std::cout << "PRIVMSG : ERROR (not registered)" << std::endl;
		sendMessage(
			clientFd,
			":ircserv 451 * :You have not registered\r\n"
		);
		return;
	}
	// Récupère la cible et le texte du message
	std::string params;
	if (line.size() > 8)
		params = line.substr(8);
	// PRIVMSG sans destinataire
	if (params.empty())
	{
		std::cout << "PRIVMSG : ERROR (no recipient)" << std::endl;
		sendMessage(
			clientFd,
			":ircserv 411 "
			+ client.getNickname()
			+ " :No recipient given (PRIVMSG)\r\n"
		);
		return;
	}
	size_t space = params.find(' ');
	// Destinataire présent mais aucun message
	if (space == std::string::npos)
	{
		std::cout << "PRIVMSG : ERROR (no text)" << std::endl;
		sendMessage(
			clientFd,
			":ircserv 412 "
			+ client.getNickname()
			+ " :No text to send\r\n"
		);
		return;
	}
	std::string target = params.substr(0, space);
	std::string message = params.substr(space + 1);
	if (!message.empty() && message[0] == ':')
		message.erase(0, 1);
	// Message vide
	if (message.empty())
	{
		std::cout << "PRIVMSG : ERROR (no text)" << std::endl;
		sendMessage(
			clientFd,
			":ircserv 412 "
			+ client.getNickname()
			+ " :No text to send\r\n"
		);
		return;
	}
	Command commandHandler;
	if (!commandHandler.privmsg(client, target, message))
	{
		std::cout << "PRIVMSG : ERROR" << std::endl;
		return;
	}
	// PRIVMSG vers un channel
	// Envoi vers un channel
	if (!target.empty() && target[0] == '#')
	{
		std::vector<int> recipients;
		PrivmsgResult result = privmsgCommand(
			clientFd,
			target,
			message,
			_clients,
			_channel,
			recipients
		);
		if (result == PRIVMSG_NO_SUCH_CHANNEL)
		{
			sendMessage(
				clientFd,
				":ircserv 403 "
				+ client.getNickname()
				+ " "
				+ target
				+ " :No such channel\r\n"
			);
			return;
		}
		if (result == PRIVMSG_NOT_ON_CHANNEL)
		{
			sendMessage(
				clientFd,
				":ircserv 442 "
				+ client.getNickname()
				+ " "
				+ target
				+ " :You're not on that channel\r\n"
			);
			return;
		}
		if (result != PRIVMSG_OK)
		{
			std::cout << "PRIVMSG : ERROR" << std::endl;
			return;
		}
		std::string msg = clientPrefix(clientFd)
			+ " PRIVMSG "
			+ target
			+ " :"
			+ message
			+ "\r\n";
		for (size_t i = 0; i < recipients.size(); ++i)
			sendMessage(recipients[i], msg);
		std::cout << "PRIVMSG : OK - channel "
				  << target
				  << std::endl;
	}
	// PRIVMSG vers un utilisateur
	// Envoi direct vers un utilisateur
	else
	{
		int targetFd = findClientFdByNickname(target);
		if (targetFd == -1)
		{
			std::cout << "PRIVMSG : ERROR (user not found)"
					  << std::endl;
			sendMessage(
				clientFd,
				":ircserv 401 "
				+ client.getNickname()
				+ " "
				+ target
				+ " :No such nick\r\n"
			);
			return;
		}
		std::string msg = clientPrefix(clientFd)
			+ " PRIVMSG "
			+ target
			+ " :"
			+ message
			+ "\r\n";
		sendMessage(targetFd, msg);
		std::cout << "PRIVMSG : OK - "
				  << target
				  << " found on fd "
				  << targetFd
				  << std::endl;
	}
}

// Découpe une ligne IRC en paramètres
static std::vector<std::string> splitIrcLine(
	const std::string &line)
{
	std::vector<std::string> tokens;
	size_t i = 0;
	while (i < line.size())
	{
		while (i < line.size()
			&& line[i] == ' ')
		{
			++i;
		}
		if (i >= line.size())
			break;
		if (line[i] == ':')
		{
			tokens.push_back(
				line.substr(i + 1));
			break;
		}
		size_t end =
			line.find(' ', i);
		if (end == std::string::npos)
		{
			tokens.push_back(
				line.substr(i));
			break;
		}
		tokens.push_back(
			line.substr(i, end - i));
		i = end + 1;
	}
	return (tokens);
}

// Découpe les valeurs séparées par des virgules
static std::vector<std::string> splitComma(
	const std::string &value)
{
	std::vector<std::string> result;
	size_t start = 0;
	while (start <= value.size())
	{
		size_t comma =
			value.find(',', start);
		if (comma == std::string::npos)
		{
			result.push_back(
				value.substr(start));
			break;
		}
		result.push_back(
			value.substr(
				start,
				comma - start));
		start = comma + 1;
	}
	return (result);
}

// Vérifie que le client peut utiliser les commandes de channel
bool Server::checkChannelRegistration(
	int clientFd)
{
	std::map<int, Client>::iterator it;
	it = _clients.find(clientFd);
	if (it == _clients.end())
		return (false);
	if (!it->second.isRegistered())
	{
		sendMessage(
			clientFd,
			":ircserv 451 * :You have not registered\r\n");
		return (false);
	}
	return (true);
}

// Envoie un message à tous les membres d'un channel
void Server::broadcastChannel(
	Channel &channel,
	const std::string &message,
	int exceptFd)
{
	const std::vector<int> &members =
		channel.getClients();
	for (
		std::vector<int>::const_iterator it =
			members.begin();
		it != members.end();
		++it)
	{
		if (*it != exceptFd)
			sendMessage(*it, message);
	}
}

// Convertit un résultat Channel en réponse IRC
void Server::sendChannelResult(
	int clientFd,
	ChannelResult result,
	const std::string &command,
	const std::string &channelName,
	const std::string &target)
{
	if (result == CHANNEL_OK
		|| result == CHANNEL_ALREADY_IN_CHANNEL)
	{
		return;
	}
	std::string nick = "*";
	std::map<int, Client>::iterator it =
		_clients.find(clientFd);
	if (it != _clients.end()
		&& !it->second.getNickname().empty())
	{
		nick =
			it->second.getNickname();
	}
	if (result == CHANNEL_BAD_NAME
		|| result == CHANNEL_NO_SUCH_CHANNEL)
	{
		sendMessage(
			clientFd,
			":ircserv 403 "
			+ nick
			+ " "
			+ channelName
			+ " :No such channel\r\n");
	}
	else if (result == CHANNEL_NOT_ON_CHANNEL)
	{
		sendMessage(
			clientFd,
			":ircserv 442 "
			+ nick
			+ " "
			+ channelName
			+ " :You're not on that channel\r\n");
	}
	else if (result == CHANNEL_INVITE_ONLY)
	{
		sendMessage(
			clientFd,
			":ircserv 473 "
			+ nick
			+ " "
			+ channelName
			+ " :Cannot join channel (+i)\r\n");
	}
	else if (result == CHANNEL_BAD_KEY)
	{
		sendMessage(
			clientFd,
			":ircserv 475 "
			+ nick
			+ " "
			+ channelName
			+ " :Cannot join channel (+k)\r\n");
	}
	else if (result == CHANNEL_FULL)
	{
		sendMessage(
			clientFd,
			":ircserv 471 "
			+ nick
			+ " "
			+ channelName
			+ " :Cannot join channel (+l)\r\n");
	}
	else if (result == CHANNEL_NOT_OPERATOR)
	{
		sendMessage(
			clientFd,
			":ircserv 482 "
			+ nick
			+ " "
			+ channelName
			+ " :You're not channel operator\r\n");
	}
	else if (
		result
		== CHANNEL_TARGET_ALREADY_IN_CHANNEL)
	{
		sendMessage(
			clientFd,
			":ircserv 443 "
			+ nick
			+ " "
			+ target
			+ " "
			+ channelName
			+ " :is already on channel\r\n");
	}
	else if (
		result
		== CHANNEL_TARGET_NOT_ON_CHANNEL)
	{
		sendMessage(
			clientFd,
			":ircserv 441 "
			+ nick
			+ " "
			+ target
			+ " "
			+ channelName
			+ " :They aren't on that channel\r\n");
	}
	else if (result == CHANNEL_BAD_MODE)
	{
		sendMessage(
			clientFd,
			":ircserv 472 "
			+ nick
			+ " "
			+ target
			+ " :is unknown mode char to me\r\n");
	}
	else if (
		result == CHANNEL_BAD_LIMIT
		|| result == CHANNEL_MISSING_ARGUMENT)
	{
		sendMessage(
			clientFd,
			":ircserv 461 "
			+ nick
			+ " "
			+ command
			+ " :Not enough or invalid parameters\r\n");
	}
}

// Envoie le topic et la liste des membres après un JOIN
void Server::sendJoinState(
	int clientFd,
	Channel &channel)
{
	std::string nick =
		_clients[clientFd].getNickname();
	if (channel.getTopic().empty())
	{
		sendMessage(
			clientFd,
			":ircserv 331 "
			+ nick
			+ " "
			+ channel.getName()
			+ " :No topic is set\r\n");
	}
	else
	{
		sendMessage(
			clientFd,
			":ircserv 332 "
			+ nick
			+ " "
			+ channel.getName()
			+ " :"
			+ channel.getTopic()
			+ "\r\n");
	}
	std::string names;
	const std::vector<int> &members =
		channel.getClients();
	for (
		std::vector<int>::const_iterator it =
			members.begin();
		it != members.end();
		++it)
	{
		std::map<int, Client>::iterator found =
			_clients.find(*it);
		if (found == _clients.end())
			continue;
		if (!names.empty())
			names += " ";
		if (channel.isOperator(*it))
			names += "@";
		names +=
			found->second.getNickname();
	}
	sendMessage(
		clientFd,
		":ircserv 353 "
		+ nick
		+ " = "
		+ channel.getName()
		+ " :"
		+ names
		+ "\r\n");
	sendMessage(
		clientFd,
		":ircserv 366 "
		+ nick
		+ " "
		+ channel.getName()
		+ " :End of /NAMES list\r\n");
}

// JOIN : rejoint un ou plusieurs channels
void Server::handleJoin(
	int clientFd,
	const std::string &line)
{
	if (!checkChannelRegistration(clientFd))
		return;
	std::vector<std::string> tokens =
		splitIrcLine(line);
	Client &client =
		_clients[clientFd];
	if (tokens.size() < 2)
	{
		sendMessage(
			clientFd,
			":ircserv 461 "
			+ client.getNickname()
			+ " JOIN :Not enough parameters\r\n");
		return;
	}
	// Plusieurs channels peuvent être donnés avec des virgules
	std::vector<std::string> channelNames =
		splitComma(tokens[1]);
	std::vector<std::string> keys;
	if (tokens.size() >= 3)
		keys = splitComma(tokens[2]);
	for (
		size_t i = 0;
		i < channelNames.size();
		++i)
	{
		std::string key;
		if (i < keys.size())
			key = keys[i];
		// Essaie de rejoindre le channel
		ChannelResult result =
			joinCommand(
				clientFd,
				channelNames[i],
				key,
				_channel);
		if (result != CHANNEL_OK)
		{
			sendChannelResult(
				clientFd,
				result,
				"JOIN",
				channelNames[i],
				"");
			continue;
		}
		// JOIN réussi : prévient les membres puis envoie l'état du channel
		Channel &channel =
			_channel[channelNames[i]];
		std::string msg =
			clientPrefix(clientFd)
			+ " JOIN :"
			+ channelNames[i]
			+ "\r\n";
		broadcastChannel(
			channel,
			msg,
			-1);
		sendJoinState(
			clientFd,
			channel);
	}
}

// PART : quitte un ou plusieurs channels
void Server::handlePart(
	int clientFd,
	const std::string &line)
{
	if (!checkChannelRegistration(clientFd))
		return;
	std::vector<std::string> tokens =
		splitIrcLine(line);
	Client &client =
		_clients[clientFd];
	if (tokens.size() < 2)
	{
		sendMessage(
			clientFd,
			":ircserv 461 "
			+ client.getNickname()
			+ " PART :Not enough parameters\r\n");
		return;
	}
	// Récupère les channels et la raison du départ
	std::vector<std::string> channelNames =
		splitComma(tokens[1]);
	std::string reason =
		client.getNickname();
	if (tokens.size() >= 3)
		reason = tokens[2];
	for (
		size_t i = 0;
		i < channelNames.size();
		++i)
	{
		// Retire le client du channel
		ChannelResult result =
			partCommand(
				clientFd,
				channelNames[i],
				_channel);
		if (result != CHANNEL_OK)
		{
			sendChannelResult(
				clientFd,
				result,
				"PART",
				channelNames[i],
				"");
			continue;
		}
		// Prépare le message PART à envoyer aux membres
		std::string msg =
			clientPrefix(clientFd)
			+ " PART "
			+ channelNames[i]
			+ " :"
			+ reason
			+ "\r\n";
		sendMessage(
			clientFd,
			msg);
		std::map<
			std::string,
			Channel
		>::iterator channel;
		channel =
			_channel.find(
				channelNames[i]);
		if (channel != _channel.end())
		{
			broadcastChannel(
				channel->second,
				msg,
				clientFd);
		}
	}
}

// INVITE : invite un utilisateur dans un channel
void Server::handleInvite(
	int clientFd,
	const std::string &line)
{
	if (!checkChannelRegistration(clientFd))
		return;
	std::vector<std::string> tokens =
		splitIrcLine(line);
	Client &client =
		_clients[clientFd];
	if (tokens.size() < 3)
	{
		sendMessage(
			clientFd,
			":ircserv 461 "
			+ client.getNickname()
			+ " INVITE :Not enough parameters\r\n");
		return;
	}
	// Recherche le client à inviter
	int targetFd =
		findClientFdByNickname(
			tokens[1]);
	if (targetFd == -1)
	{
		sendMessage(
			clientFd,
			":ircserv 401 "
			+ client.getNickname()
			+ " "
			+ tokens[1]
			+ " :No such nick\r\n");
		return;
	}
	// Vérifie les droits et ajoute l'invitation
	ChannelResult result =
		inviteCommand(
			clientFd,
			targetFd,
			tokens[2],
			_channel);
	if (result != CHANNEL_OK)
	{
		sendChannelResult(
			clientFd,
			result,
			"INVITE",
			tokens[2],
			tokens[1]);
		return;
	}
	// Confirme à l'auteur et prévient le client invité
	sendMessage(
		clientFd,
		":ircserv 341 "
		+ client.getNickname()
		+ " "
		+ tokens[1]
		+ " "
		+ tokens[2]
		+ "\r\n");
	sendMessage(
		targetFd,
		clientPrefix(clientFd)
		+ " INVITE "
		+ tokens[1]
		+ " :"
		+ tokens[2]
		+ "\r\n");
}

// KICK : expulse un utilisateur d'un channel
void Server::handleKick(
	int clientFd,
	const std::string &line)
{
	if (!checkChannelRegistration(clientFd))
		return;
	std::vector<std::string> tokens =
		splitIrcLine(line);
	Client &client =
		_clients[clientFd];
	if (tokens.size() < 3)
	{
		sendMessage(
			clientFd,
			":ircserv 461 "
			+ client.getNickname()
			+ " KICK :Not enough parameters\r\n");
		return;
	}
	// Recherche le client à expulser
	int targetFd =
		findClientFdByNickname(
			tokens[2]);
	if (targetFd == -1)
	{
		sendMessage(
			clientFd,
			":ircserv 401 "
			+ client.getNickname()
			+ " "
			+ tokens[2]
			+ " :No such nick\r\n");
		return;
	}
	// Utilise le nickname comme raison par défaut
	std::string reason =
		client.getNickname();
	if (tokens.size() >= 4)
		reason = tokens[3];
	// Vérifie que le KICK est autorisé
	ChannelResult result =
		kickCommand(
			clientFd,
			targetFd,
			tokens[1],
			_channel);
	if (result != CHANNEL_OK)
	{
		sendChannelResult(
			clientFd,
			result,
			"KICK",
			tokens[1],
			tokens[2]);
		return;
	}
	// Prépare puis diffuse le KICK
	std::string msg =
		clientPrefix(clientFd)
		+ " KICK "
		+ tokens[1]
		+ " "
		+ tokens[2]
		+ " :"
		+ reason
		+ "\r\n";
	sendMessage(
		targetFd,
		msg);
	std::map<
		std::string,
		Channel
	>::iterator channel;
	channel =
		_channel.find(tokens[1]);
	if (channel != _channel.end())
	{
		broadcastChannel(
			channel->second,
			msg,
			-1);
	}
}

// TOPIC : affiche ou modifie le topic
void Server::handleTopic(
	int clientFd,
	const std::string &line)
{
	if (!checkChannelRegistration(clientFd))
		return;
	std::vector<std::string> tokens =
		splitIrcLine(line);
	Client &client =
		_clients[clientFd];
	if (tokens.size() < 2)
	{
		sendMessage(
			clientFd,
			":ircserv 461 "
			+ client.getNickname()
			+ " TOPIC :Not enough parameters\r\n");
		return;
	}
	// Avec un troisième paramètre, le client veut modifier le topic
	bool changeTopic =
		(tokens.size() >= 3);
	std::string newTopic;
	if (changeTopic)
		newTopic = tokens[2];
	ChannelResult result =
		topicCommand(
			clientFd,
			tokens[1],
			newTopic,
			changeTopic,
			_channel);
	if (result != CHANNEL_OK)
	{
		sendChannelResult(
			clientFd,
			result,
			"TOPIC",
			tokens[1],
			"");
		return;
	}
	Channel &channel =
		_channel[tokens[1]];
	// Sans nouveau topic, renvoie simplement le topic actuel
	if (!changeTopic)
	{
		if (channel.getTopic().empty())
		{
			sendMessage(
				clientFd,
				":ircserv 331 "
				+ client.getNickname()
				+ " "
				+ tokens[1]
				+ " :No topic is set\r\n");
		}
		else
		{
			sendMessage(
				clientFd,
				":ircserv 332 "
				+ client.getNickname()
				+ " "
				+ tokens[1]
				+ " :"
				+ channel.getTopic()
				+ "\r\n");
		}
		return;
	}
	// Diffuse le nouveau topic à tous les membres
	std::string msg =
		clientPrefix(clientFd)
		+ " TOPIC "
		+ tokens[1]
		+ " :"
		+ newTopic
		+ "\r\n";
	broadcastChannel(
		channel,
		msg,
		-1);
}

// MODE : affiche ou modifie les modes d'un channel
void Server::handleMode(
	int clientFd,
	const std::string &line)
{
	if (!checkChannelRegistration(clientFd))
		return;
	std::vector<std::string> tokens =
		splitIrcLine(line);
	Client &client =
		_clients[clientFd];
	if (tokens.size() < 2)
	{
		sendMessage(
			clientFd,
			":ircserv 461 "
			+ client.getNickname()
			+ " MODE :Not enough parameters\r\n");
		return;
	}
	std::map<
		std::string,
		Channel
	>::iterator found;
	found =
		_channel.find(tokens[1]);
	if (found == _channel.end())
	{
		sendChannelResult(
			clientFd,
			CHANNEL_NO_SUCH_CHANNEL,
			"MODE",
			tokens[1],
			"");
		return;
	}
	// MODE #channel seul : affiche les modes actifs
	if (tokens.size() == 2)
	{
		Channel &channel =
			found->second;
		std::string modes = "+";
		std::string args;
		if (channel.isInviteOnly())
			modes += "i";
		if (channel.isTopicRestricted())
			modes += "t";
		if (channel.hasPassword())
		{
			modes += "k";
			args +=
				" "
				+ channel.getPassword();
		}
		if (channel.hasUserLimit())
		{
			modes += "l";
			char limit[32];
			std::sprintf(
				limit,
				"%d",
				channel.getUserLimit());
			args +=
				" "
				+ std::string(limit);
		}
		sendMessage(
			clientFd,
			":ircserv 324 "
			+ client.getNickname()
			+ " "
			+ tokens[1]
			+ " "
			+ modes
			+ args
			+ "\r\n");
		return;
	}
	// Sinon on parcourt les modes à appliquer
	const std::string &modeString =
		tokens[2];
	char sign = 0;
	size_t argIndex = 3;
	for (
		size_t i = 0;
		i < modeString.size();
		++i)
	{
		char mode =
			modeString[i];
		// Garde le signe pour les modes qui suivent
		if (mode == '+'
			|| mode == '-')
		{
			sign = mode;
			continue;
		}
		if (sign == 0)
		{
			sendChannelResult(
				clientFd,
				CHANNEL_BAD_MODE,
				"MODE",
				tokens[1],
				std::string(1, mode));
			continue;
		}
		std::string argument;
		std::string targetNick;
		int targetFd = -1;
		// +k, +l et +/-o ont besoin d'un argument
		bool needsArgument =
			(
				(mode == 'k'
					&& sign == '+')
				||
				(mode == 'l'
					&& sign == '+')
				||
				mode == 'o'
			);
		if (needsArgument)
		{
			if (argIndex >= tokens.size())
			{
				sendChannelResult(
					clientFd,
					CHANNEL_MISSING_ARGUMENT,
					"MODE",
					tokens[1],
					std::string(1, mode));
				continue;
			}
			argument =
				tokens[argIndex++];
		}
		// Pour le mode o, retrouve le client grâce à son nickname
		if (mode == 'o')
		{
			targetNick =
				argument;
			targetFd =
				findClientFdByNickname(
					targetNick);
			if (targetFd == -1)
			{
				sendMessage(
					clientFd,
					":ircserv 401 "
					+ client.getNickname()
					+ " "
					+ targetNick
					+ " :No such nick\r\n");
				continue;
			}
		}
		// Applique le mode au channel
		ChannelResult result =
			modeCommand(
				clientFd,
				tokens[1],
				sign,
				mode,
				argument,
				targetFd,
				_channel);
		if (result != CHANNEL_OK)
		{
			std::string errorTarget =
				targetNick;
			if (result == CHANNEL_BAD_MODE)
			{
				errorTarget =
					std::string(1, mode);
			}
			sendChannelResult(
				clientFd,
				result,
				"MODE",
				tokens[1],
				errorTarget);
			continue;
		}
		// Le changement a réussi : le diffuse aux membres
		std::string msg =
			clientPrefix(clientFd)
			+ " MODE "
			+ tokens[1]
			+ " "
			+ std::string(1, sign)
			+ std::string(1, mode);
		if (!argument.empty())
		{
			msg +=
				" "
				+ argument;
		}
		msg += "\r\n";
		broadcastChannel(
			_channel[tokens[1]],
			msg,
			-1);
	}
}
