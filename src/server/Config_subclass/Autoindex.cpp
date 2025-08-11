/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Autoindex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:08:32 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 09:08:39 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::ServerConfig::AutoIndex::AutoIndex( const std::string & content) throw (std::exception) : Directive(AUTOINDEX)
{
	if (content == "on") { 
		_option = true;
	} else if (content == "off") {
		_option = false;
	} else {
		throw Config::ErrorMessage(AUTOINDEX_INVALID_DIR);
	}

}

void Config::ServerConfig::AutoIndex::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._autoindex = _option;
	} else if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._autoindex = _option;
	}
}

Config::ServerConfig::AutoIndex::~AutoIndex( void )
{
	return ;
}