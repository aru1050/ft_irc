/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: athamilc <athamilc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/25 20:25:07 by athamilc          #+#    #+#             */
/*   Updated: 2026/09/25 20:25:08 by athamilc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Channel.hpp"

ChannelResult topicCommand(
    int clientFd,
    const std::string &channelName,
    const std::string &newTopic,
    bool changeTopic,
    std::map<std::string, Channel> &channels)
{
    std::map<std::string, Channel>::iterator it;

    it = channels.find(channelName);

    if (it == channels.end())
        return (CHANNEL_NO_SUCH_CHANNEL);

    Channel &channel = it->second;

    if (!channel.hasClient(clientFd))
        return (CHANNEL_NOT_ON_CHANNEL);

    if (!changeTopic)
        return (CHANNEL_OK);

    if (channel.isTopicRestricted()
        && !channel.isOperator(clientFd))
    {
        return (CHANNEL_NOT_OPERATOR);
    }

    channel.setTopic(newTopic);

    return (CHANNEL_OK);
}