/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 07:44:24 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/05 13:22:43 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"
#include "tcpServer.hpp"

int g_signal = 0;

void sigint_handle_main(int signum)
{
	g_signal |= signum;
	throw TcpServer::ErrorMessage("Server: quit with ctrl + c (SIGINT)");
}

int set_sig_handler(int signum, void (*fn)(int))
{
	struct sigaction sig;

	sig.sa_handler = fn;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	if (sigaction(signum, &sig, 0) < 0) {
		throw TcpServer::ErrorMessage("sigaction: failed");
	}
	return 0;
}

int main(int ac , char **av)
{
	int port;

	if (ac == 1) port = 8080;
	if (ac == 2) port = std::atoi(av[1]);

	try {
		set_sig_handler(SIGINT, &sigint_handle_main);
		if (ac == 1) {
			TcpServer server1("127.0.0.1", port);
			server1.startListen();
		}
		if (ac == 2) {
			std::string	path(av[1]);
			TcpServer server2(path);
		}
	} catch (const std::exception & e) { // exception and throw are still work to do
		std::cerr << e.what() << std::endl;
	}
	
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
