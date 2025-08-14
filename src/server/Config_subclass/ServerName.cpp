/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerName.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:18:47 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/14 08:44:16 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Config.hpp"

Config::ServerConfig::ServerName::ServerName(const std::string & content) throw (std::exception) : Directive(SERVERNAME)
{
	std::istringstream iss(content);
	std::string			token;

	while (iss)
	{
		iss >> token;
		_server_names.push_back(token);
	}
}

void Config::ServerConfig::ServerName::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._names = _server_names;
	}
}

Config::ServerConfig::ServerName::~ServerName( void )
{
	return ;
}