/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 07:44:24 by jspitz            #+#    #+#             */
/*   Updated: 2025/07/29 11:47:01 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"
#include "tcpServer.hpp"

int g_signal = 0;

void sig_handle(int signal)
{
	if (signal == SIGINT)
		g_signal = 1;
	std::cerr << g_signal << std::endl;
}

int main(int ac , char **av)
{
	int port;

	if (ac == 1) port = 8080;
	if (ac == 2) port = std::atoi(av[1]);

	TcpServer server = TcpServer("127.0.0.1", port);

	server.startListen();
	return 0;
}

/* this is a code for client, not server
int main(void)
{
	int socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFd < 0) {
		perror("socket");
		exit(2);
	}

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(443);
	
	if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0)
	{
		close(socketFd);
		std::cerr << "invalid address" << std::endl;
		return 1;
	}

	if (connect(socketFd, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		close(socketFd);
		std::cerr << "connect failures" << std::endl;
		return 1;
	}

	std::cout << "connected" << std::endl;

	close(socketFd);
	return 0;
}
*/
