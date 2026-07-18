#include "Channel.hpp"
#include <iostream>

int main()
{
	Channel channel("#42");

	std::cout << "Channel : " << channel.getName() << std::endl;

	channel.addClient(4);
	channel.addClient(7);

	std::cout << "Nombre de clients : " << channel.getClientCount() << std::endl;

	channel.addOperator(4);

	if (channel.isOperator(4))
		std::cout << "Client 4 est operateur" << std::endl;

	channel.setTopic("Projet ft_irc");
	std::cout << "Topic : " << channel.getTopic() << std::endl;

	channel.setInviteOnly(true);

	if (channel.isInviteOnly())
		std::cout << "Mode +i active" << std::endl;

	channel.setPassword("secret");

	if (channel.hasPassword())
		std::cout << "Mot de passe : " << channel.getPassword() << std::endl;

	channel.setUserLimit(10);

	std::cout << "Limite : " << channel.getUserLimit() << std::endl;

	channel.inviteClient(12);

	if (channel.isInvited(12))
		std::cout << "Client 12 est invite" << std::endl;

	channel.removeClient(4);

	std::cout << "Nombre de clients apres remove : "
			  << channel.getClientCount() << std::endl;

	return (0);
}