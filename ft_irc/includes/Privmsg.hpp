/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 15:15:01 by marvin            #+#    #+#             */
/*   Updated: 2026/09/24 15:15:02 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRIVMSG_HPP
# define PRIVMSG_HPP

# include <string>
# include <vector>
# include <map>

# include "Client.hpp"
# include "Channel.hpp"

enum PrivmsgResult
{
    PRIVMSG_OK = 0,
    PRIVMSG_NO_RECIPIENT,
    PRIVMSG_NO_TEXT,
    PRIVMSG_NO_SUCH_NICK,
    PRIVMSG_NO_SUCH_CHANNEL,
    PRIVMSG_NOT_ON_CHANNEL
};

PrivmsgResult privmsgCommand(
    int clientFd,
    const std::string &target,
    const std::string &message,
    std::map<int, Client> &clients,
    std::map<std::string, Channel> &channels,
    std::vector<int> &recipients
);

#endif