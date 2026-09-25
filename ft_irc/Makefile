NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

HEADERS = \
	includes/Server.hpp \
	includes/Client.hpp \
	includes/Channel.hpp \
	includes/Command.hpp \
	includes/Privmsg.hpp \
	includes/Quit.hpp 

SRCS = \
	srcs/main.cpp \
	srcs/server/Server.cpp \
	srcs/client/Client.cpp \
	srcs/channel/Channel.cpp \
	srcs/channel/Invite.cpp \
	srcs/channel/Kick.cpp \
	srcs/channel/Topic.cpp \
	srcs/commands/Command.cpp \
	srcs/commands/Join.cpp \
	srcs/commands/Part.cpp \
	srcs/commands/Privmsg.cpp \
	srcs/commands/Mode.cpp \
	srcs/commands/Quit.cpp \
	srcs/utils/Parser.cpp 

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