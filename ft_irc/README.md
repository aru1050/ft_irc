## IRC Commands

The server allows multiple clients to communicate using the IRC protocol.

### Client Registration

Before using the server, a client must register with:

- `PASS <password>`: checks the server password.
- `NICK <nickname>`: sets the client's nickname.
- `USER <username> 0 * :<realname>`: sets the user's information.

Example:

PASS pass
NICK Alice
USER alice 0 * :Alice Dupont

### Communication

- `PRIVMSG <nickname> :<message>`: sends a private message to a user.
- `PRIVMSG #channel :<message>`: sends a message to a channel.
- `QUIT :<message>`: disconnects the client from the server.


## Channels

Channels allow multiple users to communicate in the same chat room.

### Available Commands

- `JOIN #channel`: joins or creates a channel.
- `PART #channel`: leaves a channel.
- `PRIVMSG #channel :<message>`: sends a message to the channel.
- `TOPIC #channel :<topic>`: changes the channel topic.
- `INVITE <nickname> #channel`: invites a user to the channel.
- `KICK #channel <nickname>`: removes a user from the channel.
- `MODE #channel <mode>`: changes the channel settings.

### Channel Modes

- `+i`: sets the channel to invite-only.
- `+t`: only channel operators can change the topic.
- `+k <password>`: sets a channel key.
- `+l <limit>`: sets a user limit.
- `+o <nickname>`: gives operator privileges to a user.

The first user who creates a channel becomes its operator and can manage the channel settings.


## Documentation

The following resources were used during the development of the project:

- Beej's Guide to Network Programming
  - Used to understand socket programming, TCP connections, `send()`, `recv()`, non-blocking sockets and `poll()`.

- RFC 1459 - Internet Relay Chat Protocol
  - Used to understand the fundamentals of the IRC protocol and how an IRC server communicates with clients.

- RFC 2812 - Internet Relay Chat: Client Protocol
  - Used to understand IRC message syntax, client registration, IRC commands and numeric replies.