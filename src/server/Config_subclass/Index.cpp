/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Index.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:10:53 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 09:11:00 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::ServerConfig::Index::Index(const std::string & content) throw (std::exception) : Directive(INDEX)
{
	if (content.empty()) {
		throw Config::ErrorMessage(INDEX_WRONG_SYNTAX);
	}

	std::string tmp(content);

	char * token = strtok(const_cast<char*>(tmp.data()), SEPARATORS);
	
	while (token != NULL)
	{
		_indexes.push_back(token);
		token = strtok(NULL, SEPARATORS);
	}
}

void Config::ServerConfig::Index::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == SERVER_CONTEXT) {
		serv_conf._indexes = _indexes;
	} else if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._indexes =_indexes;
	}
}


Config::ServerConfig::Index::~Index( void )
{
	return ;
}
