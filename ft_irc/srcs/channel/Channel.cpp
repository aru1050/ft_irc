/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 19:58:52 by athamilc          #+#    #+#             */
/*   Updated: 2026/09/24 15:06:02 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Channel.hpp"

Channel::Channel()
    : _name(""),
      _topic(""),
      _inviteOnly(false),
      _topicRestricted(false),
      _hasPassword(false),
      _password(""),
      _hasUserLimit(false),
      _userLimit(0)
{
}

Channel::Channel(std::string name)
    : _name(name),
      _topic(""),
      _inviteOnly(false),
      _topicRestricted(false),
      _hasPassword(false),
      _password(""),
      _hasUserLimit(false),
      _userLimit(0)
{
}

Channel::~Channel()
{
}

std::string Channel::getName() const
{
    return (_name);
}

std::string Channel::getTopic() const
{
    return (_topic);
}

std::string Channel::getPassword() const
{
    return (_password);
}

int Channel::getUserLimit() const
{
    return (_userLimit);
}

int Channel::getClientCount() const
{
    return (static_cast<int>(_clients.size()));
}

const std::vector<int> &Channel::getClients() const
{
    return (_clients);
}

void Channel::setTopic(std::string topic)
{
    _topic = topic;
}

bool Channel::hasClient(int clientFd) const
{
    for (std::vector<int>::const_iterator it = _clients.begin();
         it != _clients.end(); ++it)
    {
        if (*it == clientFd)
            return (true);
    }

    return (false);
}

void Channel::addClient(int clientFd)
{
    if (!hasClient(clientFd))
    {
        _clients.push_back(clientFd);

        // Une invitation est consommée après JOIN.
        removeInvite(clientFd);
    }
}

void Channel::removeClient(int clientFd)
{
    removeOperator(clientFd);
    removeInvite(clientFd);

    for (std::vector<int>::iterator it = _clients.begin();
         it != _clients.end(); ++it)
    {
        if (*it == clientFd)
        {
            _clients.erase(it);
            return;
        }
    }
}

bool Channel::isOperator(int clientFd) const
{
    for (std::vector<int>::const_iterator it = _operators.begin();
         it != _operators.end(); ++it)
    {
        if (*it == clientFd)
            return (true);
    }

    return (false);
}

void Channel::addOperator(int clientFd)
{
    if (hasClient(clientFd) && !isOperator(clientFd))
        _operators.push_back(clientFd);
}

void Channel::removeOperator(int clientFd)
{
    for (std::vector<int>::iterator it = _operators.begin();
         it != _operators.end(); ++it)
    {
        if (*it == clientFd)
        {
            _operators.erase(it);
            return;
        }
    }
}

bool Channel::isInvited(int clientFd) const
{
    for (std::vector<int>::const_iterator it = _invitedClients.begin();
         it != _invitedClients.end(); ++it)
    {
        if (*it == clientFd)
            return (true);
    }

    return (false);
}

void Channel::inviteClient(int clientFd)
{
    if (!isInvited(clientFd))
        _invitedClients.push_back(clientFd);
}

void Channel::removeInvite(int clientFd)
{
    for (std::vector<int>::iterator it = _invitedClients.begin();
         it != _invitedClients.end(); ++it)
    {
        if (*it == clientFd)
        {
            _invitedClients.erase(it);
            return;
        }
    }
}

void Channel::setInviteOnly(bool value)
{
    _inviteOnly = value;
}

bool Channel::isInviteOnly() const
{
    return (_inviteOnly);
}

void Channel::setTopicRestricted(bool value)
{
    _topicRestricted = value;
}

bool Channel::isTopicRestricted() const
{
    return (_topicRestricted);
}

void Channel::setPassword(std::string password)
{
    _hasPassword = true;
    _password = password;
}

void Channel::removePassword()
{
    _hasPassword = false;
    _password.clear();
}

bool Channel::hasPassword() const
{
    return (_hasPassword);
}

void Channel::setUserLimit(int limit)
{
    _hasUserLimit = true;
    _userLimit = limit;
}

void Channel::removeUserLimit()
{
    _hasUserLimit = false;
    _userLimit = 0;
}

bool Channel::hasUserLimit() const
{
    return (_hasUserLimit);
}

bool Channel::canJoin(int clientFd, std::string password) const
{
    if (hasClient(clientFd))
        return (false);

    if (isInviteOnly() && !isInvited(clientFd))
        return (false);

    if (hasPassword() && password != getPassword())
        return (false);

    if (hasUserLimit() && getClientCount() >= getUserLimit())
        return (false);

    return (true);
}