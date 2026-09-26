/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 20:25:28 by athamilc          #+#    #+#             */
/*   Updated: 2026/09/25 20:25:29 by athamilc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Channel.hpp"

ChannelResult joinCommand(
    int clientFd,
    const std::string &channelName,
    const std::string &password,
    std::map<std::string, Channel> &channels)
{
    if (channelName.empty() || channelName[0] != '#')
        return (CHANNEL_BAD_NAME);

    std::map<std::string, Channel>::iterator it;

    it = channels.find(channelName);

    if (it == channels.end())
    {
        channels[channelName] = Channel(channelName);

        channels[channelName].addClient(clientFd);
        channels[channelName].addOperator(clientFd);

        return (CHANNEL_OK);
    }

    Channel &channel = it->second;

    if (channel.hasClient(clientFd))
        return (CHANNEL_ALREADY_IN_CHANNEL);

    if (channel.isInviteOnly()
        && !channel.isInvited(clientFd))
    {
        return (CHANNEL_INVITE_ONLY);
    }

    if (channel.hasPassword()
        && channel.getPassword() != password)
    {
        return (CHANNEL_BAD_KEY);
    }

    if (channel.hasUserLimit()
        && channel.getClientCount() >= channel.getUserLimit())
    {
        return (CHANNEL_FULL);
    }

    channel.addClient(clientFd);

    return (CHANNEL_OK);
}