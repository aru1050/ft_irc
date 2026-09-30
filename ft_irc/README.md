*This project has been created as part of the 42 curriculum by vcalma, yabou-da.*

# ft_irc

## Description

`ft_irc` is a C++98 project whose goal is to build a simple IRC (Internet Relay Chat) server.

The server accepts several TCP/IP clients at the same time and allows them to authenticate, choose a nickname and username, exchange private messages, join channels, and interact with other users.

The project also implements channel operators and the mandatory channel management commands required by the subject.

Main features:

- Multiple simultaneous client connections.
- Non-blocking socket communication.
- Client registration with `PASS`, `NICK` and `USER`.
- Private messages between users with `PRIVMSG`.
- Channel creation and management.
- Channel operators and permissions.
- IRC channel modes `i`, `t`, `k`, `o` and `l`.
- Handling of partial commands through a per-client input buffer.

## Instructions

### Compilation

From the project directory containing the `Makefile`, run:

    make

This creates the executable:

    ircserv

Other available Makefile rules:

    make clean
    make fclean
    make re

### Execution

Start the server with:

    ./ircserv <port> <password>

Example:

    ./ircserv 1080 pass

The port must be between `1025` and `65535`.

### Connecting with netcat

In another terminal:

    nc localhost 1080

Then register a client:

    PASS pass
    NICK Alice
    USER alice 0 * :Alice Dupont

A second client can be opened in another terminal in the same way.

## IRC Commands

### Client Registration

- `PASS <password>`: sends the connection password to the server.
- `NICK <nickname>`: sets or changes the client's nickname.
- `USER <username> 0 * :<realname>`: sets the user's information.

Example:

    PASS pass
    NICK Alice
    USER alice 0 * :Alice Dupont

### Communication

- `PRIVMSG <nickname> :<message>`: sends a private message to a user.
- `PRIVMSG #channel :<message>`: sends a message to a channel.
- `QUIT :<message>`: disconnects the client from the server.

Examples:

    PRIVMSG Bob :Hello Bob
    PRIVMSG #general :Hello everyone
    QUIT :Goodbye

## Channels

Channels allow several users to communicate in the same chat room.

### Available Commands

- `JOIN #channel`: joins a channel or creates it if it does not exist.
- `PART #channel`: leaves a channel while remaining connected to the server.
- `PRIVMSG #channel :<message>`: sends a message to the other members of the channel.
- `TOPIC #channel :<topic>`: changes the channel topic when permissions allow it.
- `INVITE <nickname> #channel`: invites a user to a channel.
- `KICK #channel <nickname>`: removes a user from a channel.
- `MODE #channel <mode>`: changes the channel settings.

The first user who creates a channel becomes a channel operator.

### Channel Modes

- `+i`: enables invite-only mode.
- `-i`: disables invite-only mode.
- `+t`: restricts topic changes to channel operators.
- `-t`: removes the topic restriction.
- `+k <key>`: sets a channel key/password.
- `-k`: removes the channel key.
- `+l <limit>`: sets the maximum number of users in the channel.
- `-l`: removes the user limit.
- `+o <nickname>`: gives operator privileges to a user.
- `-o <nickname>`: removes operator privileges from a user.

## Resources

The following resources were used to understand the IRC protocol and network programming concepts used in this project:

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
  - Used to understand TCP/IP sockets, client/server communication, `send()`, `recv()`, non-blocking sockets and `poll()`.

- [RFC 1459 - Internet Relay Chat Protocol](https://www.rfc-editor.org/rfc/rfc1459.html)
  - Used to understand the fundamentals of IRC, message structure and server/client communication.

- [RFC 2812 - Internet Relay Chat: Client Protocol](https://www.rfc-editor.org/rfc/rfc2812.html)
  - Used to understand client registration, IRC commands, message syntax and numeric replies.

### Use of AI

AI tools were used as a support during the development of the project for:

- Explaining networking concepts such as sockets, `poll()`, non-blocking I/O and input buffers.
- Reviewing and debugging parts of the IRC command handling, including client registration, private messages, disconnections and channel commands.
- Suggesting test cases for normal connections, partial commands, multiple clients and unexpected disconnections.
- Helping structure, review and improve the project documentation and README.

AI-generated suggestions were reviewed, tested and adapted before being used. The team remains responsible for understanding and being able to explain the code and technical choices made in the project.