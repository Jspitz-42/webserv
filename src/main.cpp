/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 07:44:24 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/20 09:51:01 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"
#include "tcpServer.hpp"
#include "Error.hpp"

int g_signal = 0;

void sigint_handle_main(int signum)
{
	g_signal |= signum;
	throw TCPServer::ErrorMessage(SIGINT_QUIT_MSG);
}

int set_sig_handler(int signum, void (*fn)(int))
{
	struct sigaction sig;

	sig.sa_handler = fn;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	if (sigaction(signum, &sig, 0) < 0) {
		throw TCPServer::ErrorMessage(SIGINT_ERR_MSG);
	}
	return 0;
}

int main(int ac , char **av)
{
	std::string path;

	if (ac == 1) {
		path = "./conf/default.conf" ;
	} else if (ac == 2) {
		path = av[1];
	} else {
		std::cerr << MAIN_ARGUMENT_ERRROR << std::endl;
		return 1;
	}

	try {
		set_sig_handler(SIGINT, &sigint_handle_main);

		TCPServer a(path);
		a.printConfig();
		a.run();
	} catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
	}
	
	return 0;
}
