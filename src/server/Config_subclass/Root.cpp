/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Root.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:18:18 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 09:18:21 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

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
		serv_conf._root_path = _path;
	} else if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._root_path = _path;
	}
}


Config::ServerConfig::Root::~Root( void )
{
	return ;
}