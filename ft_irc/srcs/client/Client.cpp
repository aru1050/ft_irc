/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 18:58:22 by athamilc          #+#    #+#             */
/*   Updated: 2026/08/22 18:58:23 by athamilc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
    : _fd(-1),
      _nickname(""),
      _username(""),
      _realname(""),
      _password(""),
      _buffer(""),
      _hasPass(false),
      _hasNick(false),
      _hasUser(false),
      _isRegistered(false)
{
}

Client::Client(int fd)
    : _fd(fd),
      _nickname(""),
      _username(""),
      _realname(""),
      _password(""),
      _buffer(""),
      _hasPass(false),
      _hasNick(false),
      _hasUser(false),
      _isRegistered(false)
{
}

Client::~Client()
{
}

int Client::getFd() const
{
    return _fd;
}

void Client::setFd(int fd)
{
    _fd = fd;
}

std::string Client::getNickname() const
{
    return _nickname;
}

void Client::setNickname(const std::string& nickname)
{
    _nickname = nickname;
}

std::string Client::getUsername() const
{
    return _username;
}

void Client::setUsername(const std::string& username)
{
    _username = username;
}

std::string Client::getRealname() const
{
    return _realname;
}

void Client::setRealname(const std::string& realname)
{
    _realname = realname;
}

std::string Client::getPassword() const
{
    return _password;
}

void Client::setPassword(const std::string& password)
{
    _password = password;
}

std::string Client::getBuffer() const
{
    return _buffer;
}

void Client::setBuffer(const std::string& buffer)
{
    _buffer = buffer;
}

void Client::appendBuffer(const std::string& data)
{
    _buffer += data;
}

void Client::clearBuffer()
{
    _buffer.clear();
}

bool Client::hasPass() const
{
    return _hasPass;
}

void Client::setHasPass(bool value)
{
    _hasPass = value;
}

bool Client::hasNick() const
{
    return _hasNick;
}

void Client::setHasNick(bool value)
{
    _hasNick = value;
}

bool Client::hasUser() const
{
    return _hasUser;
}

void Client::setHasUser(bool value)
{
    _hasUser = value;
}

bool Client::isRegistered() const
{
    return _isRegistered;
}

void Client::setRegistered(bool value)
{
    _isRegistered = value;
}

bool Client::isReadyToRegister() const
{
    return (_hasPass && _hasNick && _hasUser);
}