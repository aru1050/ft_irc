/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 16:43:39 by athamilc          #+#    #+#             */
/*   Updated: 2026/08/22 17:16:45 by athamilc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include <iostream>

Command::Command() {}

Command::~Command() {}

bool Command::pass(Client& client, const std::string& password, const std::string& serverPassword)
{
    if (client.isRegistered())
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

bool Command::nick(Client& client, const std::string& nickname)
{
    if (nickname.empty())
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
    
    std::cout << "PASS       : " << client.hasPass() << std::endl;
    std::cout << "NICK       : " << client.hasNick() << std::endl;
    std::cout << "USER       : " << client.hasUser() << std::endl;
    std::cout << "REGISTERED : " << client.isRegistered() << std::endl;
    return true;
}