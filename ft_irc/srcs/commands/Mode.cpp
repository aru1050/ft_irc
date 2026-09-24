#include "../../includes/Channel.hpp"
#include <climits>

static ChannelResult parseLimit(
    const std::string &argument,
    int &limit)
{
    if (argument.empty())
        return (CHANNEL_MISSING_ARGUMENT);

    long value = 0;

    for (size_t i = 0; i < argument.size(); ++i)
    {
        if (argument[i] < '0'
            || argument[i] > '9')
        {
            return (CHANNEL_BAD_LIMIT);
        }

        value =
            value * 10
            + (argument[i] - '0');

        if (value > INT_MAX)
            return (CHANNEL_BAD_LIMIT);
    }

    if (value <= 0)
        return (CHANNEL_BAD_LIMIT);

    limit = static_cast<int>(value);

    return (CHANNEL_OK);
}

ChannelResult modeCommand(
    int clientFd,
    const std::string &channelName,
    char sign,
    char mode,
    const std::string &argument,
    int targetFd,
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

    if (sign != '+' && sign != '-')
        return (CHANNEL_BAD_MODE);

    if (mode == 'i')
    {
        channel.setInviteOnly(sign == '+');

        return (CHANNEL_OK);
    }

    if (mode == 't')
    {
        channel.setTopicRestricted(sign == '+');

        return (CHANNEL_OK);
    }

    if (mode == 'k')
    {
        if (sign == '+')
        {
            if (argument.empty())
                return (CHANNEL_MISSING_ARGUMENT);

            channel.setPassword(argument);
        }
        else
        {
            channel.removePassword();
        }

        return (CHANNEL_OK);
    }

    if (mode == 'l')
    {
        if (sign == '+')
        {
            int limit = 0;

            ChannelResult result =
                parseLimit(argument, limit);

            if (result != CHANNEL_OK)
                return (result);

            channel.setUserLimit(limit);
        }
        else
        {
            channel.removeUserLimit();
        }

        return (CHANNEL_OK);
    }

    if (mode == 'o')
    {
        if (argument.empty())
            return (CHANNEL_MISSING_ARGUMENT);

        if (targetFd < 0
            || !channel.hasClient(targetFd))
        {
            return (CHANNEL_TARGET_NOT_ON_CHANNEL);
        }

        if (sign == '+')
            channel.addOperator(targetFd);
        else
            channel.removeOperator(targetFd);

        return (CHANNEL_OK);
    }

    return (CHANNEL_BAD_MODE);
}