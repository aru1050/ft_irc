/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 20:25:02 by athamilc          #+#    #+#             */
/*   Updated: 2026/09/25 20:25:03 by athamilc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Channel.hpp"

ChannelResult kickCommand(
    int clientFd,
    int targetFd,
    const std::string &channelName,
    std::map<std::string, Channel> &channels)
{
    std::map<std::string, Channel>::iterator it;

    it = channels.find(channelName);

    if (it == channels.end())
        return (CHANNEL_NO_SUCH_CHANNEL);

    Channel &channel = it->second;

    if (!channel.hasClient(clientFd))
        return (CHANNEL_NOT_ON_CHANNEL);

    if (!channel.isOperator(clientFd))
        return (CHANNEL_NOT_OPERATOR);

    if (!channel.hasClient(targetFd))
        return (CHANNEL_TARGET_NOT_ON_CHANNEL);

    channel.removeClient(targetFd);

    if (channel.getClientCount() == 0)
        channels.erase(it);

    return (CHANNEL_OK);
}