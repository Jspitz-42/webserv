/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientMaxBodySize.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:08:45 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 09:09:03 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::ServerConfig::ClientMaxBodySize::ClientMaxBodySize(const std::string & content) throw (std::exception) : Directive(CLIENTMAXBODYSIZE), _max_size(0)
{
	if (content.find(SEPARATORS) != std::string::npos) {
		throw Config::ErrorMessage(CLIENTMAXBODYSIZE_WRONG_SYNTAX);
	}
	
	std::stringstream intValue(content);

	intValue >> _max_size;
	
	if (_max_size < 0) {
		throw Config::ErrorMessage(CLIENTMAXBODYSIZE_INVALID_DIR);
	}
}

void Config::ServerConfig::ClientMaxBodySize::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._max_body_size = _max_size;
	} else if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._max_body_size = _max_size;
	}
}

Config::ServerConfig::ClientMaxBodySize::~ClientMaxBodySize( void )
{
	return ;
}
