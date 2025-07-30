/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcpServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 12:37:10 by jspitz            #+#    #+#             */
/*   Updated: 2025/07/30 15:08:12 by jspitz           ###   ########.fr       */
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
# define MAX 1024

class TcpServer
{
	public:
								TcpServer(std::string ip_address, int port);		
								~TcpServer();
		void					startListen( void );

	protected:

	private:

		std::string 			m_ip_addr;
		int						m_port;
		int						m_socket;
		int						m_new_socket;
	//	long					m_incomingMessage;
		struct	sockaddr_in 	m_socketAddress;
		struct	sockaddr_in		csin;
		unsigned int			m_socketAddress_len;
		std::string				m_serverMessage;
		int						_efd; // epoll fd
		int						_ret;
		int						_nfds;
		int						_cfd;

		struct epoll_event		_ev;
		struct epoll_event		_evlist[MAX];

		int						startServer();
		void					closeServer();
		void					AcceptConnection(int &new_socket);
		std::string				buildResponse();
		void					sendResponse( void );
		bool					findClientFile( void );
		
		std::vector<int>			_cfds; // all accepted client fd stocked
};

extern int g_signal;
