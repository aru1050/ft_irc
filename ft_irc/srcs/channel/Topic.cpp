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