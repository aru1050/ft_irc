/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 16:44:34 by athamilc          #+#    #+#             */
/*   Updated: 2026/08/22 17:02:31 by athamilc         ###   ########.fr       */
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
};

#endif