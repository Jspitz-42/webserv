/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 07:44:24 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/08 13:22:40 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"
#include "tcpServer.hpp"

int g_signal = 0;

void sigint_handle_main(int signum)
{
	g_signal |= signum;
	throw TCPServer::ErrorMessage("Server: quit with ctrl + c (SIGINT)");
}

int set_sig_handler(int signum, void (*fn)(int))
{
	struct sigaction sig;

	sig.sa_handler = fn;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	if (sigaction(signum, &sig, 0) < 0) {
		throw TCPServer::ErrorMessage("sigaction: failed");
	}
	return 0;
}

int main(int ac , char **av)
{
//	int port;
	std::string path;

	if (ac == 1) {/*port = 8080; */ path = "./conf/default.conf" ; }
	else if (ac == 2) { /*port = std::atoi(av[1]);*/ path = av[1]; }
	else { std::cerr << "ERROR: [Too many arguments] [please: only one is required] [tips: one is less than two]" << std::endl ;}

	try {
		set_sig_handler(SIGINT, &sigint_handle_main);

		TCPServer a(path);
		a.run();
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
