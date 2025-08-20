/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiBin.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlonghin <tlonghin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:09:41 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/20 11:16:06 by tlonghin         ###   ########.fr       */
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
		
		struct stat s;

		if (stat(_path.c_str(), &s) != 0) {
			throw Config::ErrorMessage("ERROR: [CGI-BIN] [DIRECTORY DOES NOT EXIST] : " + _path);
		}
		else if (!(s.st_mode & S_IFDIR)) {
			throw Config::ErrorMessage("ERROR: [CGI-BIN] [IS NOT A DIRECTORY] : " + _path);
		} else {
			serv_conf._locations.back()._cgi_bin = _path;
			serv_conf._locations.back()._cgi_bin_found = true;
		}
	}
}

Config::ServerConfig::CgiBin::~CgiBin( void )
{
	return ;
}
