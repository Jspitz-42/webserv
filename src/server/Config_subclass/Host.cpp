/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Host.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: altheven <altheven@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:13:38 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/23 15:02:59 by altheven         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Config.hpp"

Config::ServerConfig::Host::Host(const std::string & content) throw (std::exception): Directive(HOST),
																						 _port(80)
{
	if (content.empty() || content.find_first_of(SEPARATORS) != std::string::npos) {
		throw Config::ErrorMessage(LISTEN_WRONG_SYNTAX);
	}
	std::string 		tmp;
	std::stringstream	stoi_converter;
	_host_ip = content;
	if (content.find(':') != std::string::npos) {

		std::string ip_str = content.substr(0, content.find(':'));
		std::string	port_str = content.substr(content.find(':') + 1);

		if (!isIpValid(ip_str) || port_str.empty()) {
			throw Config::ErrorMessage(LISTEN_INVALID_IP);
		} else if (ip_str == "*") {
			_ip = "0.0.0.0";
		} else if (ip_str == "localhost") {
			_ip = "127.0.0.1";
		} else {
			_ip = ip_str;
		}

		stoi_converter << port_str;
		stoi_converter >> _port;

	} else if (isIpValid(content)) {
		tmp = content;
		if (!tmp.compare("localhost")) {
			_ip = "127.0.0.1";
		} else if (!tmp.compare("*")) {
			_ip = "0.0.0.0";
		} else {
			_ip = tmp;
		}

	} else {
		
		stoi_converter << content;
		stoi_converter >> _port;
		
		if (_port > PORT_MAX || _port <= PORT_MIN) {
			throw Config::ErrorMessage(LISTEN_OUT_OF_RANGE);
		}
	}
}

void Config::ServerConfig::Host::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context != SERVER_CONTEXT) return ;

	serv_conf._host_port = _port;
	serv_conf._host_ip = _ip;
	serv_conf._full_host = _host_ip;
}


bool Config::ServerConfig::Host::isIpValid(const std::string & ip)
{
	int counter = 0;

	if (!ip.compare("0.0.0.0") || !ip.compare("localhost") || !ip.compare("*"))
		return true;
	
	for (u_int16_t i = 0 ; i <= ip.length() ; i++) {
		if (ip[i] == '.') counter++ ;
	}

	if (counter != 3) return false;

	int	 part1;
	char ch;
	std::stringstream stoi_converter(ip);

	stoi_converter >> part1 >> ch;

	if (part1 < 1 || part1 > 255 || ch != '.')
		return false;
	
	int rest;

	while (stoi_converter.rdbuf()->in_avail() != 0)
	{
		stoi_converter >> rest >> ch;
		if (rest < 0 || rest > 255 || ch != '.') 
			return false;
	}

	return true;
}

Config::ServerConfig::Host::~Host( void )
{
	return ;
}
