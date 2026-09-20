/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/21 16:39:07 by athamilc          #+#    #+#             */
/*   Updated: 2026/09/20 13:21:05 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include <iostream>

Command::Command() {}

Command::~Command() {}

bool Command::pass(Client& client, 
                    const std::string& password, 
                    const std::string& serverPassword)
{
    if (client.isRegistered())
        return false;

    if (client.hasPass())
        return false;
        
    if (password.empty())
        return false;

    if (password != serverPassword)
        return false;
    
    client.setPassword(password);
    client.setHasPass(true);
    if (client.isReadyToRegister())
        client.setRegistered(true);
    return true;
}

bool Command::isValidNickname(const std::string& nickname)
{
    if (nickname.empty())
        return false;

    if (nickname[0] >= '0' && nickname[0] <= '9')
        return false;

    for (size_t i = 0; i < nickname.size(); i++)
    {
        char c = nickname[i];

        if (!((c >= 'a' && c <= 'z') ||
              (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') ||
              c == '_' || c == '-'))
            return false;
    }

    return true;
}

bool Command::nick(Client& client, const std::string& nickname)
{
    if (nickname.empty())
        return false;

    if (!isValidNickname(nickname))
        return false;

    client.setNickname(nickname);
    client.setHasNick(true);

    if (client.isReadyToRegister())
        client.setRegistered(true);

    return true;
}

bool Command::user(Client& client,
                   const std::string& username,
                   const std::string& realname)
{
    if (client.isRegistered())
        return false;

    if (client.hasUser())
        return false;

    if (username.empty() || realname.empty())
        return false;
    
    client.setUsername(username);
    client.setRealname(realname);
    client.setHasUser(true);
    
    if (client.isReadyToRegister())
        client.setRegistered(true);

    return true;
}