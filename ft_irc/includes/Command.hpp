/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yabou-da <yabou-da@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 19:07:48 by athamilc          #+#    #+#             */
/*   Updated: 2026/09/22 21:29:41 by yabou-da         ###   ########.fr       */
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
    
    static bool user(Client& client, const std::string& username, 
            const std::string& realname);
    static bool nick(Client& client, const std::string& nickname);
    static bool pass(Client& client, const std::string& password, const std::string& serverPassword);

    static bool isValidNickname(const std::string& nickname);
};

#endif