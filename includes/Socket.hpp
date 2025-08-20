/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlonghin <tlonghin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 10:29:52 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/20 19:44:46 by tlonghin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <unistd.h>
# include <cstring>
# include <cstdio>
# include <cstdlib>
# include <fcntl.h>

# include "utils.hpp"
# include "Error.hpp"
# include "Config.hpp"

class ServerConfig;

class Socket
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

											Socket(const std::string &, int);
											~Socket( void );
											Socket( void );
											Socket(const Socket &);

		int 								acceptConnection( void );
		int									getPort( void ) const;
		int									getSocketFd( void ) const;
		int									getAddressLen( void ) const;

		const std::string & 				getIpAdress( void ) const;
		struct sockaddr_in					getAdress( void ) const;
	
	protected:

	private:
	
		int									_socket_fd;
		int									_addrlen;
		
		std::string							_ip_address;
		int									_port;
		
		struct sockaddr_in					_address;		
		
};

std::ostream&	operator<<(std::ostream&, const Socket&);