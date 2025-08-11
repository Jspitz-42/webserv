/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Upload.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jspitz <jspitz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 09:19:05 by jspitz            #+#    #+#             */
/*   Updated: 2025/08/11 09:19:14 by jspitz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::ServerConfig::Upload::Upload(const std::string & content) throw (std::exception) : Directive(ROOT), _upload_path(content)
{
	if (content.empty() ||
			content.find(SEPARATORS) != std::string::npos ||
				content[content.length() - 1] != '/') {
		throw Config::ErrorMessage(UPLOAD_WRONG_SYNTAX);
	}
}

void Config::ServerConfig::Upload::setDirective(ServerConfig & serv_conf, int context) const
{
	if (context == LOCATION_CONTEXT) {
		serv_conf._locations.back()._upload_path = _upload_path;
	}
}

Config::ServerConfig::Upload::~Upload( void )
{
	return ;
}
