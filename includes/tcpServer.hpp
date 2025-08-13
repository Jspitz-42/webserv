/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcpServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 12:37:10 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/13 07:18:28 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <sys/socket.h>
# include <arpa/inet.h>
# include <cstdlib>
# include <string>
# include <cstring>
# include <sstream>
# include <unistd.h>
# include <sys/epoll.h>
# include <iostream>
# include <fstream>
# include <fcntl.h>
# include <vector>
# include <list>
# include <algorithm>
# include <exception>
# include "Client.hpp"
# include "Socket.hpp"
# include "Error.hpp"
# define MAX 1024
# define MAX_EVENTS 10
class Congig;
class Client;
class Socket;

class TCPServer
{
	public:

		class ErrorMessage : virtual public std::exception
		{
			std::string msg;
			
			public:

			ErrorMessage(const std::string & message ) : msg(message) {}

			virtual ~ErrorMessage() throw() {}

			const char * what() const throw()
			{
				return msg.c_str();
			}	
		};

											TCPServer(std::string const &) throw (std::exception);
											~TCPServer();
		void								addSocket(Socket &) throw (std::exception);
		void								cleanEpollAndClientsList();
		void								removeClient(std::pair<int, int> &);
		int									getEpollFd() const;
		int									numSockets() const;
		void								printConfig( void ) const throw (std::exception);
		void								run();

	private:
		int									_epollfd;
		std::vector<Socket>					_sockets;
		std::map<int, std::vector<Client> >	_clients;
		Config								_config;
		Socket &							getOrCreateSocket(std::string const &, int);
		void								acceptConnectionAt(int) throw (std::exception); 
		bool								isSocketFd(int);
		void								initMsg(void);
											TCPServer(const TCPServer &);
};

extern int g_signal;
