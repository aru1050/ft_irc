NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

HEADERS = \
	includes/Server.hpp \
	includes/Client.hpp \
	includes/Channel.hpp \
	includes/Command.hpp \
	includes/Utils.hpp

SRCS = \
	srcs/main.cpp \
	srcs/server/Server.cpp \
	srcs/server/Socket.cpp \
	srcs/server/Poll.cpp \
	srcs/client/Client.cpp \
	srcs/client/Authentication.cpp \
	srcs/channel/Channel.cpp \
	srcs/channel/Invite.cpp \
	srcs/channel/Kick.cpp \
	srcs/channel/Topic.cpp \
	srcs/commands/Pass.cpp \
	srcs/commands/Nick.cpp \
	srcs/commands/User.cpp \
	srcs/commands/Join.cpp \
	srcs/commands/Privmsg.cpp \
	srcs/commands/Mode.cpp \
	srcs/commands/Quit.cpp \
	srcs/utils/Parser.cpp \
	srcs/utils/Replies.cpp \
	srcs/utils/Utils.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -Iincludes -o $(NAME) $(OBJS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -Iincludes -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re