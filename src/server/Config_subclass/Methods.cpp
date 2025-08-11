/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:11:05 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 14:22:18 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Config.hpp"


Config::ServerConfig::Methods::Methods(const std::string & content) throw (std::exception) : Directive(LIMITMETHODS)
{
	char * str = std::strtok(const_cast<char*>(content.c_str()), " ");
		if (!_validMethod(std::string(str))) {
			throw Config::ErrorMessage(METHODS_INVALID_DIR_1);
		}

		for ( ; str != NULL ; ) {
			_methods.push_back(std::string(str));
			str = std::strtok(NULL, " ");
		}
}

void  Config::ServerConfig::Methods::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == LOCATION_CONTEXT) {
		std::vector<std::string>::const_iterator it = _methods.begin();
		for ( size_t i = 0 ;i < _methods.size(); i++) { 
			serv_conf._approved_methods.push_back(*it);
			it++;
		}
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