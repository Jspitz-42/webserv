/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Root.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlonghin <tlonghin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:18:18 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/20 11:15:34 by tlonghin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "sys/stat.h"

Config::ServerConfig::Root::Root(const std::string & content) throw (std::exception) : Directive(ROOT),
																					  _path(content)
{
	if (content.empty() ||
			content.find(SEPARATORS) != std::string::npos ){
		throw Config::ErrorMessage(ROOT_WRONG_SYNTAX);
	}
}

void Config::ServerConfig::Root::setDirective(ServerConfig & serv_conf, int context) const
{
    if (context == SERVER_CONTEXT) {
        struct stat sb;
        if (stat(_path.c_str(), &sb) != 0 || (sb.st_mode & S_IFDIR))
            throw Config::ErrorMessage("ERROR: ROOT PATH DOES NOT EXIST OR IS NOT AN EXECUTABLE");

        serv_conf._root_path = _path;
    } 
    else if (context == LOCATION_CONTEXT) {
        if (serv_conf._locations.empty())
            throw Config::ErrorMessage("No locations defined for setting root");

			struct stat sb;
		if (stat(_path.c_str(), &sb) != 0)
			serv_conf._locations.back()._root_found = false;
		else
			serv_conf._locations.back()._root_found  = true;	
        serv_conf._locations.back()._root_path = _path;
    }
}



Config::ServerConfig::Root::~Root( void )
{
	return ;
}