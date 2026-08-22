/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yabou-da <yabou-da@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 15:11:31 by yabou-da          #+#    #+#             */
/*   Updated: 2026/06/21 19:15:26 by yabou-da         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main(int c, char **v)
{
	(void) c;
	try
	{
		Server Server(v[1], v[2]);
		Server.ft_getaddrinfo();
		Server.initSocket();
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return(1);
	}
	return(0);
}