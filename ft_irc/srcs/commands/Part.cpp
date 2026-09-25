/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 20:25:41 by athamilc          #+#    #+#             */
/*   Updated: 2026/09/25 20:25:42 by athamilc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Channel.hpp"

ChannelResult partCommand(
    int clientFd,
    const std::string &channelName,
    std::map<std::string, Channel> &channels)
{
    if (channelName.empty() || channelName[0] != '#')
        return (CHANNEL_BAD_NAME);

    std::map<std::string, Channel>::iterator it;

    it = channels.find(channelName);

    if (it == channels.end())
        return (CHANNEL_NO_SUCH_CHANNEL);

    if (!it->second.hasClient(clientFd))
        return (CHANNEL_NOT_ON_CHANNEL);

    it->second.removeClient(clientFd);

    if (it->second.getClientCount() == 0)
        channels.erase(it);

    return (CHANNEL_OK);
}