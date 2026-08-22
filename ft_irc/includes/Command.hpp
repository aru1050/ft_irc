/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 16:44:34 by athamilc          #+#    #+#             */
/*   Updated: 2026/08/22 16:44:36 by athamilc         ###   ########.fr       */
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
    
    void user(Client& client, const std::string& username, 
            const std::string& realname);
    void nick(Client& client, const std::string& nickname);
    void pass(Client& client, const std::string& password);
};

#endif