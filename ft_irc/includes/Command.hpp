/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 19:07:48 by athamilc          #+#    #+#             */
/*   Updated: 2026/09/22 21:51:56 by athamilc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include "Client.hpp"


class Command
{
    private:
    
    public:
    Command();
    ~Command();
    
    bool user(Client& client, const std::string& username, 
            const std::string& realname);
    bool nick(Client& client, const std::string& nickname);
    bool pass(Client& client, const std::string& password, const std::string& serverPassword);

    bool isValidNickname(const std::string& nickname);
    bool quit(Client& client, const std::string& reason);
    bool privmsg(Client& client, const std::string& target, const std::string& message);
};

#endif