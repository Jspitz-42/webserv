/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcpServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 12:37:10 by jspitz            #+#    #+#             */
/*   Updated: 2025/07/28 14:05:03 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <iostream>

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
		unsigned int			m_socketAddress_len;
		std::string				m_serverMessage;

		int						startServer();
		void					closeServer();
		void					AcceptConnection(int &new_socket);
		std::string				buildResponse();
		void					sendResponse();
};
