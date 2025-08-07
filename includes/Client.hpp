/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 10:28:16 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/07 13:13:19 by jspitz           ###   ########.fr       */
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
	private:
		int _fd;
		bool _keep_alive;
		uint64_t _time_to_die;
		Socket & _socket;
		Client();
	public:
		Client(int, Socket &);
		Client(const Client &);
		Client & operator= (const Client &);
		~Client();
		int					getFd() const;
		Socket const &		getSocket() const;
		void				handleRequest();
		bool 				keepAlive() const;
		uint64_t const &	timeToDie() const;
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