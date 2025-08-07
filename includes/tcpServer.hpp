/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcpServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 12:37:10 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/07 13:07:44 by jspitz           ###   ########.fr       */
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
# include "Config.hpp"
# include "Client.hpp"
# include "Socket.hpp"
# define MAX 1024
# define MAX_EVENTS 10
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

		class AcceptException: public std::exception {
			public:
				virtual const char * what() const throw();
		};
		class EpollAddException: public std::exception {
			public:
				virtual const char * what() const throw();
		};
		class EpollCreateException: public std::exception {
			public:
				virtual const char * what() const throw();
		};
		class EpollDeleteException: public std::exception {
			public:
				virtual const char * what() const throw();
		};
		class EpollWaitException: public std::exception {
			public:
				virtual const char * what() const throw();
		};
		class ReadFdException: public std::exception {
			public:
				virtual const char * what() const throw();
		};
		TCPServer(std::string const &) throw (std::exception);
		~TCPServer();
		void	addSocket(Socket &) throw (std::exception);
		void	cleanEpollAndClientsList();
		void	removeClient(std::pair<int, int> &);
		int		getEpollFd() const;
		int		numSockets() const;
		void	run();
	private:
		int									_epollfd;
		std::vector<Socket>					_sockets;
		std::map<int, std::vector<Client> >	_clients;
		Config								_config;
		Socket &	getOrCreateSocket(std::string const &, int);
		void	acceptConnectionAt(int) throw (std::exception); 
		bool	isSocketFd(int);
		void	initMsg(void);
		TCPServer(const TCPServer &);
};


/** 
class TcpServer
{
	public:
								TcpServer(std::string const &) throw (std::exception);
		void					addSocket(Socket &) throw (std::exception);
		void					cleanEpollClientList( void );
		void					removeClient(std::pair<int, int> &);
		int						getEpollFd( void ) const;
		int						numSockets( void ) const;		
//								TcpServer(std::string & file);
								~TcpServer();
		void					run( void );

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

	protected:

	private:
		int									_epollfd;
		std::vector<Socket>					_sockets;
		std::map<int, std::vector<Client> >	_clients;
		Config								_config;
		Socket &							getOrCreateSocket(std::string const &, int);
		void								acceptConnectionAt( int ) throw(std::exception);
		bool								isSocketFd( int );
		void								initMsg( void );
//		std::string 			m_ip_addr;
//		int						m_port;
//		int						m_socket;
//		int						m_new_socket;
//	//	long					m_incomingMessage;
//		struct	sockaddr_in 	m_socketAddress;
//		struct	sockaddr_in		csin;
//		unsigned int			m_socketAddress_len;
//		std::string				m_serverMessage;
//		int						_efd; // epoll fd
//		int						_ret;
//		int						_nfds;
//		int						_cfd;
//
//		struct epoll_event		_ev;
//		struct epoll_event		_evlist[MAX];
//
//		int						startServer();
//		void					closeServer();
//		void					AcceptConnection(int &new_socket);
//		std::string				buildResponse();
//		void					sendResponse( void );
//		bool					findClientFile( void );
//		
//		std::vector<int>		_cfds; // all accepted client fd stocked
//		Config					_config;

};

*/
extern int g_signal;
