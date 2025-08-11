/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiBin.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:09:41 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 09:09:47 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::ServerConfig::CgiBin::CgiBin(const std::string & content) throw (std::exception) : Directive(CGIBIN), _path(content)
{
	if (_path.empty()) {
		throw Config::ErrorMessage(CGIBIN_WRONG_SYNTAX);
	}
}

void Config::ServerConfig::CgiBin::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._cgi_bin = _path;
	}
}

Config::ServerConfig::CgiBin::~CgiBin( void )
{
	return ;
}
