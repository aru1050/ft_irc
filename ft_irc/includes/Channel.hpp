/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/22 19:57:25 by athamilc          #+#    #+#             */
/*   Updated: 2026/09/24 15:06:17 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>
# include <map>

class Channel
{
    private:
        std::string _name;
        std::string _topic;

        std::vector<int> _clients;
        std::vector<int> _operators;
        std::vector<int> _invitedClients;

        bool _inviteOnly;
        bool _topicRestricted;

        bool _hasPassword;
        std::string _password;

        bool _hasUserLimit;
        int _userLimit;

    public:
        Channel();
        Channel(std::string name);
        ~Channel();

        std::string getName() const;
        std::string getTopic() const;
        std::string getPassword() const;

        int getUserLimit() const;
        int getClientCount() const;

        const std::vector<int> &getClients() const;

        void setTopic(std::string topic);

        void addClient(int clientFd);
        void removeClient(int clientFd);
        bool hasClient(int clientFd) const;

        void addOperator(int clientFd);
        void removeOperator(int clientFd);
        bool isOperator(int clientFd) const;

        void inviteClient(int clientFd);
        void removeInvite(int clientFd);
        bool isInvited(int clientFd) const;

        void setInviteOnly(bool value);
        bool isInviteOnly() const;

        void setTopicRestricted(bool value);
        bool isTopicRestricted() const;

        void setPassword(std::string password);
        void removePassword();
        bool hasPassword() const;

        void setUserLimit(int limit);
        void removeUserLimit();
        bool hasUserLimit() const;

        bool canJoin(int clientFd, std::string password) const;
};

enum ChannelResult
{
    CHANNEL_OK = 0,

    CHANNEL_BAD_NAME,
    CHANNEL_NO_SUCH_CHANNEL,

    CHANNEL_ALREADY_IN_CHANNEL,
    CHANNEL_NOT_ON_CHANNEL,

    CHANNEL_INVITE_ONLY,
    CHANNEL_BAD_KEY,
    CHANNEL_FULL,

    CHANNEL_NOT_OPERATOR,

    CHANNEL_TARGET_ALREADY_IN_CHANNEL,
    CHANNEL_TARGET_NOT_ON_CHANNEL,

    CHANNEL_BAD_MODE,
    CHANNEL_BAD_LIMIT,
    CHANNEL_MISSING_ARGUMENT
};

ChannelResult joinCommand(
    int clientFd,
    const std::string &channelName,
    const std::string &password,
    std::map<std::string, Channel> &channels
);

ChannelResult partCommand(
    int clientFd,
    const std::string &channelName,
    std::map<std::string, Channel> &channels
);

ChannelResult inviteCommand(
    int clientFd,
    int targetFd,
    const std::string &channelName,
    std::map<std::string, Channel> &channels
);

ChannelResult kickCommand(
    int clientFd,
    int targetFd,
    const std::string &channelName,
    std::map<std::string, Channel> &channels
);

ChannelResult topicCommand(
    int clientFd,
    const std::string &channelName,
    const std::string &newTopic,
    bool changeTopic,
    std::map<std::string, Channel> &channels
);

ChannelResult modeCommand(
    int clientFd,
    const std::string &channelName,
    char sign,
    char mode,
    const std::string &argument,
    int targetFd,
    std::map<std::string, Channel> &channels
);

#endif