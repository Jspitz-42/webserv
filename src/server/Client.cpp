/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 12:58:15 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/07 13:13:55 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Socket.hpp"

Client::Client(int fd, Socket & s): _fd(fd), _keep_alive(true), _time_to_die(timestamp_in_ms() + TIME_TO_DIE), _socket(s)
{
	return ;
}

Client::Client(const Client & param): _fd(param._fd), _keep_alive(param._keep_alive), _time_to_die(param._time_to_die), _socket(param._socket)
{
	return ;
}

Client& Client::operator= (const Client& param)
{
	_fd = param._fd;
	_socket = param._socket;
	_time_to_die = param._time_to_die;
	_keep_alive = param._keep_alive;
	return (*this);
}

Client::~Client()
{
	return ;
}

int Client::getFd() const
{
	return	(_fd); 
}

Socket const & Client::getSocket() const
{
	return	(_socket);
}

void Client::handleRequest()
{
	char		buffer[30000] = {0};
	int			valread;
	uint64_t	ms_s;
	std::string	buf;

	ms_s = timestamp_in_ms();
	do {
		valread = read(_fd, buffer, 30000);
		if (valread <= 0)
			break ;
		std::string tmp(buffer, valread);
		buf += tmp;
	} while (valread == 30000); // CHECK FOR EOF... // READ BY CHUNCKS???
	if (buf.length() > 0) { // perhaps don't check this? return 408 if response is empty...?	
		_time_to_die = ms_s + TIME_TO_DIE;
		std::string line(buf.substr(0, buf.find_first_of("\n\r")));
		Request req(buf, _socket.getServerConfig(line));
		Response res(req, _socket.getServerConfig(line));
		std::string response_content(res.createResponse());
		write(_fd, response_content.c_str(), response_content.length());
		_keep_alive = res.getKeepAlive();
		std::cout << " Completed " << res.getStatusCode() << " " << Response::_codeMessage[res.getStatusCode()] << " in " << timestamp_in_ms() - ms_s << "ms " << " at " << get_local_time() << std::endl;
	} else {
		// RESPONSE 408!!!
		_keep_alive = false;
	}
}

bool Client::keepAlive() const
{
	return (_keep_alive);
}

uint64_t const & Client::timeToDie() const
{
	return (_time_to_die);
}

std::ostream& operator<<(std::ostream& s, const Client& param)
{
	s << param.getFd();
	return (s);
}