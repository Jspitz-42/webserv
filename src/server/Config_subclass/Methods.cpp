/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:11:05 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/17 09:24:44 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Config.hpp"


Config::ServerConfig::Methods::Methods(const std::string & content) throw (std::exception) : Directive(LIMITMETHODS)
{
	std::istringstream iss(content);
	std::string s;

	iss >> s;
	if (!_validMethod(std::string(s))) {
		throw Config::ErrorMessage(METHODS_INVALID_DIR_1);
	}

	while (iss)
	{
		_methods.push_back(std::string(s));
		iss >> s;
	}
}

void  Config::ServerConfig::Methods::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._methods = _methods;
	}
}

bool Config::ServerConfig::Methods::_validMethod(const std::string & method)
{
	for (size_t i  = 0; i < 4 ; i++) {
		if (method == _valid_methods[i])
			return true;
	}

	return false;
}

Config::ServerConfig::Methods::~Methods( void )
{
	return ;
}