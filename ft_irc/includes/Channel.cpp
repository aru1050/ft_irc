#include "Channel.hpp"

// Constructeur par défaut
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

// Constructeur avec nom du channel
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

// ================= GETTERS =================

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
	return (_clients.size());
}

// ================= TOPIC =================

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

// ================= CLIENTS =================

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

		// Quand un invité rejoint, son invitation est consommée
		removeInvite(clientFd);
	}
}

void Channel::removeClient(int clientFd)
{
	// On retire aussi ses droits secondaires
	removeOperator(clientFd);
	removeInvite(clientFd);

	for (std::vector<int>::iterator it = _clients.begin();
		it != _clients.end(); ++it)
	{
		if (*it == clientFd)
		{
			_clients.erase(it);
			return ;
		}
	}
}

// ================= OPERATEURS =================

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
	// Un opérateur doit d'abord être membre du channel
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
			return ;
		}
	}
}

// ================= INVITATIONS =================

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
			return ;
		}
	}
}

// ================= MODE +i =================

void Channel::setInviteOnly(bool value)
{
	_inviteOnly = value;
}

bool Channel::isInviteOnly() const
{
	return (_inviteOnly);
}

// ================= MODE +t =================

void Channel::setTopicRestricted(bool value)
{
	_topicRestricted = value;
}

bool Channel::isTopicRestricted() const
{
	return (_topicRestricted);
}

// ================= MODE +k =================

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

// ================= MODE +l =================

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

// ================= JOIN CHECK =================

bool Channel::canJoin(int clientFd, std::string password) const
{
	// Déjà dans le channel
	if (hasClient(clientFd))
		return (false);

	// Mode +i actif et client pas invité
	if (isInviteOnly() && !isInvited(clientFd))
		return (false);

	// Mode +k actif et mauvais mot de passe
	if (hasPassword() && password != getPassword())
		return (false);

	// Mode +l actif et limite atteinte
	if (hasUserLimit() && getClientCount() >= getUserLimit())
		return (false);

	return (true);
}