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


void Server::handlePass(int clientFd, const std::string& line)
{
	std::string password = line.substr(5);

	Client& client = this->_clients[clientFd];
	Command command;

	if (command.pass(client, password, this->_password))
		std::cout << "PASS : OK" << std::endl;
	else
		std::cout << "PASS : ERROR" << std::endl;
}

void Server::handleUser(int clientFd, const std::string& line)
{
	std::string params = line.substr(5);

	size_t space = params.find(' ');
	size_t colon = params.find(':');

	if (space == std::string::npos || colon == std::string::npos)
	{
		std::cout << "USER : ERROR" << std::endl;
		return;
	}

	std::string username = params.substr(0, space);
	std::string realname = params.substr(colon + 1);

	Client& client = this->_clients[clientFd];
	Command command;

	if (command.user(client, username, realname))
		std::cout << "USER : OK" << std::endl;
	else
		std::cout << "USER : ERROR" << std::endl;
}

void Server::handleNick(int clientFd, const std::string& line)
{
	std::string nickname = line.substr(5);

	Client& client = this->_clients[clientFd];
	Command command;

	for (std::map<int, Client>::iterator it = this->_clients.begin();
		it != this->_clients.end(); ++it)
	{
		if (it->first != clientFd &&
			it->second.getNickname() == nickname)
		{
			std::cout << "NICK : ERROR (already used)" << std::endl;
			return;
		}
	}

	if (command.nick(client, nickname))
		std::cout << "NICK : OK" << std::endl;
	else
		std::cout << "NICK : ERROR" << std::endl;
}

void Server::handleQuit(int clientFd, const std::string& line)
{
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

		quitCommand(clientFd, _channel);

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

	std::string params;

	if (line.size() > 8)
		params = line.substr(8);

	size_t space = params.find(' ');

	if (space == std::string::npos)
	{
		std::cout << "PRIVMSG : ERROR" << std::endl;
		return;
	}

	std::string target = params.substr(0, space);
	std::string message = params.substr(space + 1);

	if (!message.empty() && message[0] == ':')
		message.erase(0, 1);

	Command commandHandler;

	if (!commandHandler.privmsg(client, target, message))
	{
		std::cout << "PRIVMSG : ERROR" << std::endl;
		return;
	}

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

    std::string reason =
        client.getNickname();

    if (tokens.size() >= 4)
        reason = tokens[3];

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