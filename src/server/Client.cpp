/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: altheven <altheven@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 12:58:15 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/17 06:38:39 by altheven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Socket.hpp"

Client :: Client(int fd, Socket & s): _fd(fd), _keep_alive(true), _time_to_die(timestamp_in_ms() + TIME_TO_DIE), _socket(s)
{
	return;
}

Client::Client(const Client & param): _fd(param._fd), _keep_alive(param._keep_alive), _time_to_die(param._time_to_die), _socket(param._socket)
{
	return ;
}

Client & Client :: operator=(const Client & other)
{
	if (this != &other)
	{
		*this = other;
	}
	return(*this);
}

Client :: ~Client(void)
{
	return ;
}

int	Client :: getFd(void) const
{
	return (_fd);
}

Socket const & Client :: getSocket(void) const
{
	return (_socket);
}

void Client :: handleRequest(Config :: ServerConfig conf)
{
	char			buffer[30000] = {0};
	int				valread;
	std :: string	buf;
	UINT64_T		ms = timestamp_in_ms();

	do {
		valread = read(_fd, buffer, 3000);
		if (valread <= 0)
			break ;
		std :: string tmp(buffer, valread);
		buf += tmp;
	} while (valread == 30000);
	if (buf.length() > 0)
	{
		_time_to_die = ms + TIME_TO_DIE;
		Request req(buf, conf);
		Response res(req, conf);
		std :: string full_response(res.createResponse());
		write(_fd, full_response.c_str(), full_response.length());
		_keep_alive = res.getKeepAlive();
		std::cout << " Completed " << res.getStatusCode() << " " << Response::_codeMessage[res.getStatusCode()] << " in " << timestamp_in_ms() - ms << "ms " << " at " << get_local_time() << std::endl;
	} else 
	{
		_keep_alive = false;
	}	
}


bool Client::keepAlive() const
{
	return (_keep_alive);
}

UINT64_T	const & Client :: timeToDie() const
{
	return (_time_to_die);
}