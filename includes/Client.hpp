/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlonghin <tlonghin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 10:28:16 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/20 11:14:21 by tlonghin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
# include "Request.hpp"
# include "Response.hpp"

# define TIME_TO_DIE 1000

class Socket;

class Client
{
	public:
							Client(int, Socket &);
							Client(const Client &);
							~Client();
		
		int					getFd() const;
		bool 				keepAlive() const;
		void				handleRequest(Config :: ServerConfig conf);
		Socket const &		getSocket() const;
		UINT64_T const &	timeToDie() const;
		Client & operator=	(const Client &);
							
	protected:

	private:
		int					_fd;
		bool				_keep_alive;
		UINT64_T			_time_to_die;
		Socket &			 _socket;
							Client();

};
std::ostream &	operator<<(std::ostream &, const Client &);
