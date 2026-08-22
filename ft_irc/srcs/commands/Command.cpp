/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/21 16:39:07 by athamilc          #+#    #+#             */
/*   Updated: 2026/08/22 19:17:29 by athamilc         ###   ########.fr       */
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
    
    std::cout << "FD         : " << client.getFd() << std::endl;
    std::cout << "Nickname   : " << client.getNickname() << std::endl;
    std::cout << "Username   : " << client.getUsername() << std::endl;
    std::cout << "Realname   : " << client.getRealname() << std::endl;
    std::cout << "Password   : " << client.getPassword() << std::endl;
    std::cout << "hasPass    : " << client.hasPass() << std::endl;
    std::cout << "hasNick    : " << client.hasNick() << std::endl;
    std::cout << "hasUser    : " << client.hasUser() << std::endl;
    std::cout << "Registered : " << client.isRegistered() << std::endl;
    
    return true;
}