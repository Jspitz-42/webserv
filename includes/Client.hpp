/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 10:28:16 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/08 10:26:47 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
# include "Request.hpp"
# include "Response.hpp"

# define TIME_TO_DIE 5000

class Socket;

class Client
{
	public:
							Client(int, Socket &);
							Client(const Client &);
							~Client();
		
		int					getFd() const;
		bool 				keepAlive() const;
		void				handleRequest();
		Socket const &		getSocket() const;
		uint64_t const &	timeToDie() const;
		Client & operator=	(const Client &);
							
	protected:

	private:
		int					_fd;
		bool				_keep_alive;
		uint64_t			_time_to_die;
		Socket &			 _socket;
							Client();

};
std::ostream &	operator<<(std::ostream &, const Client &);

/** 

class Client
{
	public:

		class ErrorMessage : virtual public std::exception
		{
			std::string	msg;
			
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
};

*/