/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 11:08:34 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 12:58:11 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "Config.hpp"

Socket::Socket(const std::string & ip, int port) : _ip_address(ip), _port(port)
{
	if ((_socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
		throw Socket::ErrorMessage(SOCKET_FAILURES);
	
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = inet_addr(_ip_address.c_str());

	if (_address.sin_addr.s_addr == INADDR_NONE)
		throw Socket::ErrorMessage(INADDR_NONE_ERR_MSG);
	
	_address.sin_port = htons(_port);
	_addrlen = sizeof(_address);

	bzero(_address.sin_zero, sizeof(_address.sin_zero));
	
	int opt = 1;

	if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw Socket::ErrorMessage(SETSOCKOPT_ERR_MSG);
	
	if (bind(_socket_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
		throw Socket::ErrorMessage(BIND_ERR_MSG);

	if (listen(_socket_fd, 10) < 0)
		throw Socket::ErrorMessage(LISTEN_ERR_MSG);
}

Socket::Socket(const Socket & other)
{
	_socket_fd		=	other._socket_fd;
	_addrlen		=	other._addrlen;
	_port			=	other._port;
	_ip_address		=	other._ip_address;
	_address		=	other._address;
	_conf_servers	=	other._conf_servers;
}

Socket::~Socket( void )
{
	return ;
}

int		Socket::getPort( void ) const 					{ return _port ;}
int		Socket::getSocketFd( void ) const 				{ return _socket_fd ;}
int 	Socket::getAddressLen( void ) const 			{ return _addrlen ;}
const 	std::string & Socket::getIpAdress( void ) const { return _ip_address ;}
struct 	sockaddr_in Socket::getAdress( void ) const 	{ return _address ;}

Config::ServerConfig const & Socket::getServerConfig(std::string const & line) const
{
	std::string											method;
	std::string 										target;
	Config::ServerConfig::Location * 					loc;
	std::vector<Config::ServerConfig>::const_iterator	it;

	method = line.substr(0, line. find_first_of(" \r\t"));
	target = line.substr(method.length(), line.find_last_of(" \r\t?#")- method.length());
	target = strtrim(target, " \r\t");
	
	for (it = _conf_servers.begin() ; it != _conf_servers.end() ; it++) {
		loc = it->findLocation(target);
		
		if (!loc) { continue ; }
		
		delete(loc);
		
		return (*it);
	}

	return _conf_servers.front();
}

void Socket::addServerConf(Config::ServerConfig & sc)
{
	_conf_servers.push_back(sc);
}

int Socket::acceptConnection( void )
{
	return accept(_socket_fd, (struct sockaddr *)&_address, (socklen_t *)&_addrlen);	
}

std::ostream& operator<<(std::ostream& s, const Socket& param)
{
	s << "http://" << param.getIpAdress() << ":" << param.getPort();
	return (s);
}