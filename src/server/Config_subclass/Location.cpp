/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:15:49 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/19 07:14:29 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "utils.hpp"

Config::ServerConfig::Location::Location(std:: string const & content) throw (std::exception) : Directive(LOCATION),
																								_target(content),
																								_max_body_size(0),
																								_redirect_status(0),
																								_autoindex(false)
{
	if (content.empty() || content[content.length() - 1] != '{') {
		throw Config::ErrorMessage(LOCATION_WRONG_SYNTAX_1);
	}
	
	std::cout << "Location constructor _target" << std::endl;
	_target = _target.substr(0, content.length() - 1);
	_target = strtrim(_target);

	if (_target.empty() || _target.find(SEPARATORS) != std::string::npos) {
		throw Config::ErrorMessage(LOCATION_WRONG_SYNTAX_2);
	}
}

void Config::ServerConfig::Location::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._locations.push_back(*this);
	}
}

bool Config::ServerConfig::Location::checkMaxBody(int len) const
{
	if (len > _max_body_size && _max_body_size > 0) return false;

	return true;
}

bool Config::ServerConfig::Location::findMethod(const std::string & method) const
{
	std::vector<std::string>::const_iterator it = _methods.begin();
	
	for (; it != _methods.end() ; it++) {
		if (method == *it) return true;
	}
	return false;
}

Config::ServerConfig::Location::~Location( void )
{
	return ;
}
