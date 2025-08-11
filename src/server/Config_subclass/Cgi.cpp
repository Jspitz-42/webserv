/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:09:10 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 09:10:27 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Config.hpp"

Config::ServerConfig::Cgi::Cgi(const std::string & content) throw (std::exception) : Directive(CGI)
{
	if (content.empty()) {
		throw Config::ErrorMessage(CGI_WRONG_SYNTAX);
	}

	_parseCgiContent(_cgi, content);
}

void Config::ServerConfig::Cgi::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._cgi_map[_cgi[0]].push_back(_cgi[1]);
	}
}

void Config::ServerConfig::Cgi::_parseCgiContent(std::vector<std::string> & _target, const std::string & content)
{
	char * token = strtok(const_cast<char*>(content.data()), " ");

	while (token)
	{
		_target.push_back(std::string(token));
		token = strtok(NULL, SEPARATORS);
	}

	if (_target.size() != 2) {
		throw Config::ErrorMessage(CGI_PARSCONTENT_INVALID_DIR);
	}
}

Config::ServerConfig::Cgi::~Cgi( void ) { return ;}
