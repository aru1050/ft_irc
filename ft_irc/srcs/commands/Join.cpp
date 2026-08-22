/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 16:43:39 by athamilc          #+#    #+#             */
/*   Updated: 2026/08/22 16:43:44 by athamilc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

Command::Command() {}

Command::~Command() {}

void Command::pass(Client& client, const std::string& password)
{
    client.setPassword(password);
    client.setHasPass(true);
    if (client.isReadyToRegister())
        client.setRegistered(true);
}

void Command::nick(Client& client, const std::string& nickname)
{
    client.setNickname(nickname);
    client.setHasNick(true);
    if (client.isReadyToRegister())
        client.setRegistered(true);
}

void Command::user(Client& client,
                   const std::string& username,
                   const std::string& realname)
{
    client.setUsername(username);
    client.setRealname(realname);
    client.setHasUser(true);
    if (client.isReadyToRegister())
        client.setRegistered(true);
}