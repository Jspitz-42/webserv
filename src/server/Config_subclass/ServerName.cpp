/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerName.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:18:47 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 09:18:52 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Config.hpp"

Config::ServerConfig::ServerName::ServerName(const std::string & content) throw (std::exception) : Directive(SERVERNAME)
{
	std::string	tmp(content);
	char *		token = strtok(const_cast<char*>(tmp.c_str()), SEPARATORS);

	while (token)
	{
		_server_names.push_back(token);
		token = strtok(NULL, SEPARATORS);
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